/*+===================================================================
	File: ThreadPool.cpp
	Summary: スレッドプール
             このスレッドプールはタスクの実行順序を保証しない
	Author: 青木雄一郎
	Date: 2026/4/5 Sun AM 06:33:54 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ThreadPool.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	inline constexpr unsigned int cx_DefaultThreadCount = 4;
    // TLS: ワーカースレッドなら自分のIndex、外部スレッドなら -1
    thread_local int t_WorkerIndex = -1;
}

// ==============================
// ThreadPool
// ==============================

ThreadPool::ThreadPool()
{
    unsigned int count = std::thread::hardware_concurrency();
    if(!count)
        count = cx_DefaultThreadCount;

    m_Workers.reserve(count);
    for(unsigned int i = 0; i < count; ++i)
    {
        m_Workers.emplace_back(std::make_unique<Worker>(*this, static_cast<int>(i)));
    }
}

ThreadPool::~ThreadPool()
{
    // デストラクタでは、必ず終了させる
    Shutdown(ShutdownMode::Terminate);
}

int ThreadPool::GetCurrentThreadIndex() const
{
    return t_WorkerIndex;
}

void ThreadPool::Shutdown(_In_ ShutdownMode In_Mode)
{
    // 受付停止(再帰AppendTaskを止める)
    m_AcceptTasks.store(false, std::memory_order_release);

    // Drainならpending==0を待つ(受付停止済みなので増えない)
    if(In_Mode == ShutdownMode::Drain)
        WaitUntilIdle();

    // Termination & Wakeup
    m_IsRequestedTermination.store(true, std::memory_order_release);
    m_GlobalCond.notify_all();

    for(auto &worker : m_Workers)
        worker->RequestTermination();

    WakeupAll(-1);

    // Join
    for(auto &worker : m_Workers)
        worker->Join();
}

void ThreadPool::WaitUntilIdle()
{
    std::unique_lock<std::mutex> lock(m_GlobalMutex);
    m_GlobalCond.wait(lock, [this]()
        {
            return (m_PendingTasks.load(std::memory_order_acquire) == 0)
                || m_IsRequestedTermination.load(std::memory_order_acquire);
        });
}

std::function<void()> ThreadPool::TaskStealOrPull(_In_ int In_Index)
{
    // 他ワーカーからSteal
    const int count = static_cast<int>(m_Workers.size());
    for(int i = 0; i < count; ++i)
    {
        if(i == In_Index)
            continue;

        auto task = m_Workers[i]->Steal();
        if(task)
            return task;
    }

    // グローバルからPull
    std::unique_lock<std::mutex> lock(m_GlobalMutex);
    if(m_GlobalTasks.empty())
        return {};

    auto task = std::move(m_GlobalTasks.front());
    m_GlobalTasks.pop_front();
    return task;
}

void ThreadPool::WakeupAll(_In_ int In_Index)
{
    for(int i = 0; i < static_cast<int>(m_Workers.size()); ++i)
    {
        if(i == In_Index)
            continue;
        m_Workers[i]->Wakeup();
    }
}

void ThreadPool::OnTaskFinished()
{
    if(m_PendingTasks.fetch_sub(1, std::memory_order_acq_rel) == 1)
    {
        std::unique_lock<std::mutex> lock(m_GlobalMutex);
        m_GlobalCond.notify_all();
    }
}

// ==============================
// Worker
// ==============================

ThreadPool::Worker::Worker(_In_ ThreadPool &In_Pool, _In_ int In_Index)
    : m_Pool(In_Pool)
    , m_Index(In_Index)
    , m_Thread([this]() { WorkerProc(); })
{}

ThreadPool::Worker::~Worker()
{
    // JoinはThreadPool側が管理する(寿命を明確にしてStealのアクセス違反を潰す)
}

void ThreadPool::Worker::RequestTermination()
{
    m_IsRequestedTermination.store(true, std::memory_order_release);
    m_LocalCond.notify_all();
}

void ThreadPool::Worker::Join()
{
    if(m_Thread.joinable())
        m_Thread.join();
}

void ThreadPool::Worker::Wakeup()
{
    m_LocalCond.notify_one();
}

std::function<void()> ThreadPool::Worker::Steal()
{
    std::unique_lock<std::mutex> lock(m_LocalMutex);
    if(m_LocalTasks.empty())
        return {};

    auto task = std::move(m_LocalTasks.front());
    m_LocalTasks.pop_front();
    return task;
}

void ThreadPool::Worker::WorkerProc()
{
    // TLSに自分のIndexを登録
    t_WorkerIndex = m_Index;

    for(;;)
    {
        if(m_IsRequestedTermination.load(std::memory_order_acquire)
            || m_Pool.m_IsRequestedTermination.load(std::memory_order_acquire))
        {
            break;
        }

        std::function<void()> task;

        // ローカルから取得
        {
            std::unique_lock<std::mutex> lock(m_LocalMutex);
            if(!m_LocalTasks.empty())
            {
                if((m_PopCounter & 0x7) == 0)
                {
                    task = std::move(m_LocalTasks.front());
                    m_LocalTasks.pop_front();
                }
                else
                {
                    task = std::move(m_LocalTasks.back());
                    m_LocalTasks.pop_back();
                }
                ++m_PopCounter;
            }
        }

        // 無ければSteal/Pull
        if(!task)
            task = m_Pool.TaskStealOrPull(m_Index);

        // それでも無ければ待つ
        if(!task)
        {
            std::uint64_t seen = m_Pool.m_GlobalWakeSeq.load(std::memory_order_acquire);

            std::unique_lock<std::mutex> lock(m_LocalMutex);
            m_LocalCond.wait(lock, [&]()
                {
                    return m_IsRequestedTermination.load(std::memory_order_acquire)
                        || m_Pool.m_IsRequestedTermination.load(std::memory_order_acquire)
                        || !m_LocalTasks.empty()
                        || m_Pool.m_GlobalWakeSeq.load(std::memory_order_acquire) != seen;
                });
            continue;
        }

        // 実行(pending減算はwrappedが責任を持つ)
        task();
    }

    // TLSを外部扱いに戻す
    t_WorkerIndex = -1;
}

// ==============================
// TaskHandle
// ==============================

bool ThreadPool::TaskHandle::IsDone() const
{
    return m_State && m_State->done.load(std::memory_order_acquire);
}

void ThreadPool::TaskHandle::Wait() const
{
    if(!m_State)
        return;
    std::unique_lock<std::mutex> lock(m_State->mtx);
    m_State->cv.wait(lock, [&]() { return m_State->done.load(std::memory_order_acquire); });
}
