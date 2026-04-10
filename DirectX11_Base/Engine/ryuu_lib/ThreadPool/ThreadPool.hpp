/*+===================================================================
	File: ThreadPool.hpp
	Summary: スレッドプール
			 このスレッドプールはタスクの実行順序を保証しない
	Author: 青木雄一郎
	Date: 2026/4/5 Sun AM 06:33:48 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"

/**
 * @brief スレッドプール
 */
class ThreadPool : public Singleton<ThreadPool>
{
    friend class Singleton<ThreadPool>;

public:
    enum class ShutdownMode
    {
        Drain,     // 受付停止→全タスク完了待ち→終了
        Terminate, // 受付停止→即終了(残タスクは破棄)
    };

    class TaskHandle
    {
        friend class ThreadPool;
    public:
        TaskHandle() = default;

        inline bool IsValid() const { return static_cast<bool>(m_State); }

        bool IsDone() const;

        void Wait() const;

        template <typename Rep, typename Period>
        bool WaitFor(const std::chrono::duration<Rep, Period> &d) const
        {
            if(!m_State) return true;
            std::unique_lock<std::mutex> lock(m_State->mtx);
            return m_State->cv.wait_for(lock, d, [&]() { return m_State->done.load(std::memory_order_acquire); });
        }

    private:
        struct State
        {
            std::atomic<bool> done{ false };
            std::mutex mtx;
            std::condition_variable cv;
        };

        explicit TaskHandle(std::shared_ptr<State> state)
            : m_State(std::move(state))
        {}

        std::shared_ptr<State> m_State;
    };

public:
	
    /**
	 * @brief タスク追加(Shutdown開始後は拒否してfalseを返す)
	 * @tparam T タスクの型(呼び出し可能オブジェクト)
	 * @param In_Task 実行したいタスク(引数無しの呼び出し可能オブジェクト。ラムダ式もOK)
	 * @return タスクハンドル(受付拒否ならstd::nullopt)
     */
    template <typename T>
    std::optional<TaskHandle> AddTask(_In_ T &&In_Task);

    /**
	 * @brief タスク追加(引数付きオーバーロード)
	 * @brief 例1: AddTask(Function, arg1, arg2);
	 * @brief 例2: AddTask(&MyClass::MemberFunction, &obj, arg1, arg2);
	 * @tparam T タスクの型(呼び出し可能オブジェクト)
	 * @tparam ...Args タスクの引数の型
	 * @param In_Task 実行したいタスク
	 * @param ...In_Args タスクの引数
	 * @return タスクハンドル(受付拒否ならstd::nullopt)
     */
    template <typename T, typename... Args>
	std::optional<TaskHandle> AddTask(_In_ T &&In_Task, _In_ Args&&... In_Args);

    /**
	 * @brief (受付停止後に)全タスク完了まで待つ
     */
    void WaitUntilIdle();
	
    /**
	 * @brief シャットダウン
	 * @param In_Mode シャットダウンモード(Drain/Terminate)
     */
    void Shutdown(_In_ ShutdownMode In_Mode);

    /**
	 * @brief タスク受付中か(Shutdown開始後はfalseになる)
	 * @return trueなら受付中、falseなら受付停止
     */
    inline bool IsAcceptingTasks() const
    {
        return m_AcceptTasks.load(std::memory_order_acquire);
    }

private:
    class Worker
    {
    public:
        Worker(_In_ ThreadPool &In_Pool, _In_ int In_Index);
        ~Worker();

        Worker(const Worker &) = delete;
        Worker &operator=(const Worker &) = delete;
        Worker(Worker &&) noexcept = delete;
        Worker &operator=(Worker &&) noexcept = delete;

        void RequestTermination();
        void Join();
        void Wakeup();

        template <typename T>
        void PushBack(T &&task)
        {
            {
                std::unique_lock<std::mutex> lock(m_LocalMutex);
                m_LocalTasks.emplace_back(std::forward<T>(task));
            }
            m_LocalCond.notify_one();
        }

        std::function<void()> Steal();

    private:
        void WorkerProc();

    private:
        ThreadPool &m_Pool;
        const int m_Index;
        std::uint32_t m_PopCounter = 0;

        std::atomic<bool> m_IsRequestedTermination{ false };
        std::thread m_Thread;

        std::deque<std::function<void()>> m_LocalTasks;
        std::mutex m_LocalMutex;
        std::condition_variable m_LocalCond;
    };

private:
    ThreadPool();
    ~ThreadPool();

    // TLSから取得(ワーカースレッド以外は -1)

    /**
	 * @brief 現在のスレッドがワーカースレッドならそのIndexを、そうでないなら-1を返す
     */
    int GetCurrentThreadIndex() const;

    /**
	 * @brief タスクを盗むかグローバルから引っ張る(ワーカースレッドは自分以外のワーカーから盗み、ワーカースレッド以外はグローバルから引っ張る)
	 * @param In_Index ワーカーのIndex(ワーカースレッド以外は-1)
	 * @return タスク(無ければ空)
     */
    std::function<void()> TaskStealOrPull(_In_ int In_Index);

    /**
	 * @brief 全ワーカーを起こす(ワーカースレッド以外は全て、ワーカースレッドは自分以外を起こす)
	 * @param In_Index ワーカーのIndex(ワーカースレッド以外は-1)
     */
    void WakeupAll(_In_ int In_Index);

    /**
	 * @brief タスク完了通知(タスクが完了したら呼ぶ。pending減算と、待ち状態なら起こすのを行う)
     */
    void OnTaskFinished();

private:
    std::vector<std::unique_ptr<Worker>> m_Workers;

    std::atomic<bool> m_AcceptTasks{ true };
    std::atomic<bool> m_IsRequestedTermination{ false };

    std::atomic<int> m_PendingTasks{ 0 };
    std::atomic<std::uint64_t> m_GlobalWakeSeq{ 0 };

    std::deque<std::function<void()>> m_GlobalTasks;
    mutable std::mutex m_GlobalMutex;
    std::condition_variable m_GlobalCond;
};

template<typename T>
inline std::optional<ThreadPool::TaskHandle> ThreadPool::AddTask(_In_ T &&In_Task)
{
    if(!m_AcceptTasks.load(std::memory_order_acquire))
        return std::nullopt;

    m_PendingTasks.fetch_add(1, std::memory_order_relaxed);

    auto state = std::make_shared<TaskHandle::State>();
    TaskHandle handle{ state };

    auto wrapped = [this, state, task = std::forward<T>(In_Task)]() mutable
        {
            try { task(); }
            catch(...)
            {
                OutputDebugStringA("error : ThreadPool: Unhandled exception in task\n");
            }

            // タスク完了を通知
            state->done.store(true, std::memory_order_release);
            state->cv.notify_all();

            OnTaskFinished();
        };

    const int idx = GetCurrentThreadIndex();

    if(idx >= 0)
    {
        m_Workers[static_cast<std::size_t>(idx)]->PushBack(std::move(wrapped));
        WakeupAll(idx);
    }
    else
    {
        {
            std::unique_lock<std::mutex> lock(m_GlobalMutex);
            m_GlobalTasks.emplace_back(std::move(wrapped));
        }
        m_GlobalWakeSeq.fetch_add(1, std::memory_order_release);
        m_GlobalCond.notify_all();
        WakeupAll(-1);
    }

    return handle;
}

template<typename T, typename ...Args>
inline std::optional<ThreadPool::TaskHandle> ThreadPool::AddTask(_In_ T &&In_Task, _In_ Args && ...In_Args)
{
	if(!m_AcceptTasks.load(std::memory_order_acquire))
		return std::nullopt;

	m_PendingTasks.fetch_add(1, std::memory_order_relaxed);

	auto state = std::make_shared<TaskHandle::State>();
	TaskHandle handle{ state };

	// In_TaskとIn_Argsを値として保持して、実行時にinvokeする
	using Fnc = std::decay_t<T>;
	using Tup = std::tuple<std::decay_t<Args>...>;

	auto fnc = Fnc(std::forward<T>(In_Task));
	auto tup = Tup(std::forward<Args>(In_Args)...);

	auto wrapped = [this, state, fnc = std::move(fnc), tup = std::move(tup)]() mutable
		{
			try
			{
				std::apply(
					[&](auto&&... unpacked)
					{
						std::invoke(fnc, std::forward<decltype(unpacked)>(unpacked)...);
					},
					tup
				);
			}
			catch(...)
			{
				OutputDebugStringA("error : ThreadPool: Unhandled exception in task\n");
			}

			state->done.store(true, std::memory_order_release);
			state->cv.notify_all();

			OnTaskFinished();
		};

	const int idx = GetCurrentThreadIndex();
	if(idx >= 0)
	{
		m_Workers[static_cast<std::size_t>(idx)]->PushBack(std::move(wrapped));
		WakeupAll(idx);
	}
	else
	{
		{
			std::unique_lock<std::mutex> lock(m_GlobalMutex);
			m_GlobalTasks.emplace_back(std::move(wrapped));
		}
		m_GlobalWakeSeq.fetch_add(1, std::memory_order_release); // ←あなたのwakeSeq方式
		m_GlobalCond.notify_all();
		WakeupAll(-1);
	}

	return handle;
}
