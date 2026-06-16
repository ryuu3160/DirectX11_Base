/*+===================================================================
	File: ParticlePoolManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/31 Wed AM 10:26:39 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ParticlePoolManager.hpp"
#include "Core/System/Utility/Utility.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

// -------------------------------
//	パーティクルプール管理
// -------------------------------
void ParticlePoolManager::InitializeParticlePool(_In_ int In_InitialSize)
{
    std::lock_guard<std::mutex> lock(m_ParticleMutex);

    m_ParticlePool.resize(In_InitialSize);
    m_AvailableParticles.reserve(In_InitialSize);

    for(auto &itr : m_ParticlePool)
    {
        itr.Reset();
        m_AvailableParticles.push_back(&itr);
    }

#ifdef _DEBUG
    std::string log = "ParticlePoolInitialized:   " + ToString(In_InitialSize) + " Particles\n";
    DebugManager::GetInstance().DebugLog(log.c_str());
#endif
}

Particle *ParticlePoolManager::GetParticleFromPool()
{
    std::lock_guard<std::mutex> lock(m_ParticleMutex);

    // プールが空なら拡張
    if(m_AvailableParticles.empty())
    {
        int CurrentSize = static_cast<int>(m_ParticlePool.size());
        int ExpandSize = std::max<int>(CurrentSize / 2, 100); // 50%増やす (最低100個)

#ifdef _DEBUG
        std::string log = "ParticlePoolExpanding: +" + ToString(ExpandSize) + "Particles\n";
        DebugManager::GetInstance().DebugLog(log.c_str());
#endif

        ExpandParticlePool(ExpandSize);
    }

    // 末尾から取得
    Particle *particle = m_AvailableParticles.back();
    m_AvailableParticles.pop_back();

    particle->Reset();
    return particle;
}

void ParticlePoolManager::ReleaseParticle(_In_ Particle *In_pParticle)
{
    if(!In_pParticle)
        return;

    std::lock_guard<std::mutex> lock(m_ParticleMutex);

    In_pParticle->Reset();
    m_AvailableParticles.push_back(In_pParticle);
}

void ParticlePoolManager::ExpandParticlePool(_In_ int In_AdditionalSize)
{
    int OldSize = static_cast<int>(m_ParticlePool.size());
    m_ParticlePool.resize(OldSize + In_AdditionalSize);

    // 新しく追加された要素をプールに追加
    for(int i = OldSize; i < m_ParticlePool.size(); ++i)
    {
        m_ParticlePool[i].Reset();
        m_AvailableParticles.push_back(&m_ParticlePool[i]);
    }
}

// -------------------------------
//	エミッタープール管理
// -------------------------------
void ParticlePoolManager::InitializeEmitterPool(_In_ int In_InitialSize)
{
    std::lock_guard<std::mutex> lock(m_EmitterMutex);

    m_EmitterPool.reserve(In_InitialSize);
    m_AvailableEmitters.reserve(In_InitialSize);

    for(int i = 0; i < In_InitialSize; ++i)
    {
        auto emitter = std::make_unique<ParticleEmitter>();
        m_AvailableEmitters.push_back(emitter.get());
        m_EmitterPool.push_back(std::move(emitter));
    }

#ifdef _DEBUG
    std::string log = "EmitterPoolInitialized:  " + ToString(In_InitialSize) + "Emitters\n";
    DebugManager::GetInstance().DebugLog(log.c_str());
#endif
}

ParticleEmitter *ParticlePoolManager::GetEmitterFromPool()
{
    std::lock_guard<std::mutex> lock(m_EmitterMutex);

    if(m_AvailableEmitters.empty())
    {
        int ExpandSize = std::max<int>(static_cast<int>(m_EmitterPool.size()) / 2, 5);

#ifdef _DEBUG
        std::string log = "EmitterPoolExpanding: +" + std::to_string(ExpandSize) + "Emitters\n";
        DebugManager::GetInstance().DebugLog(log.c_str());
#endif

        ExpandEmitterPool(ExpandSize);
    }

    ParticleEmitter *emitter = m_AvailableEmitters.back();
    m_AvailableEmitters.pop_back();

    return emitter;
}

void ParticlePoolManager::ReleaseEmitter(_In_ ParticleEmitter *In_pEmitter)
{
    if(!In_pEmitter)
        return;

    std::lock_guard<std::mutex> lock(m_EmitterMutex);

    In_pEmitter->Stop();
    // エミッターの状態をリセット (必要に応じて)

    m_AvailableEmitters.push_back(In_pEmitter);
}

void ParticlePoolManager::ExpandEmitterPool(_In_ int In_AdditionalSize)
{
    for(int i = 0; i < In_AdditionalSize; ++i)
    {
        auto emitter = std::make_unique<ParticleEmitter>();
        m_AvailableEmitters.push_back(emitter.get());
        m_EmitterPool.push_back(std::move(emitter));
    }
}

ParticlePoolManager::PoolStats ParticlePoolManager::GetStats()
{
    PoolStats Stats = {};

    {
        std::lock_guard<std::mutex> lock(m_ParticleMutex);
        Stats.ParticleTotalCount = static_cast<int>(m_ParticlePool.size());
        Stats.ParticleActiveCount = Stats.ParticleTotalCount - static_cast<int>(m_AvailableParticles.size());
    }
    {
        std::lock_guard<std::mutex> lock(m_EmitterMutex);
        Stats.EmitterTotalCount = static_cast<int>(m_EmitterPool.size());
        Stats.EmitterActiveCount = Stats.EmitterTotalCount - static_cast<int>(m_AvailableEmitters.size());
    }

    return Stats;
}

ParticlePoolManager::ParticlePoolManager()
{
	InitializeParticlePool(5000); // デフォルトで5000個のパーティクルプールを作成
	InitializeEmitterPool(20);    // デフォルトで50個のエミッタープールを作成
}
ParticlePoolManager::~ParticlePoolManager()
{
	m_ParticlePool.clear();
	m_AvailableParticles.clear();
	m_EmitterPool.clear();
	m_AvailableEmitters.clear();
}
