/*+===================================================================
	File: ParticlePoolManager.hpp
	Summary: パーティクルデータのプール管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/31 Wed AM 10:26:33 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include "ryuu_lib/Singleton/Singleton.hpp"

/// <summary>
/// ParticlePoolManagerクラス
/// </summary>
class ParticlePoolManager : public Singleton<ParticlePoolManager>
{
	friend class Singleton<ParticlePoolManager>;
public:
    /// <summary>
    /// Particleのプールを初期化
    /// </summary>
    /// <param name="[In_InitialSize]">初期プールサイズ</param>
    void InitializeParticlePool(_In_ int In_InitialSize = 1000);

    /// <summary>
    /// ParticleEmitterのプールを初期化
    /// </summary>
    /// <param name="[In_InitialSize]">初期プールサイズ</param>
    void InitializeEmitterPool(_In_ int In_InitialSize = 10);

    /// <summary>
    /// Particleをプールから取得
    /// </summary>
    /// <returns>使用可能な Particle へのポインタ</returns>
    Particle *GetParticleFromPool();

    /// <summary>
    /// Particle をプールに返却
    /// </summary>
    /// <param name="[In_pParticle]">返却する Particle</param>
    void ReleaseParticle(_In_ Particle *In_pParticle);

    /// <summary>
    /// ParticleEmitter をプールから取得
    /// </summary>
    /// <returns>使用可能な ParticleEmitter へのポインタ</returns>
    ParticleEmitter *GetEmitterFromPool();

    /// <summary>
    /// ParticleEmitter をプールに返却
    /// </summary>
    /// <param name="[In_pEmitter]">返却する ParticleEmitter</param>
    void ReleaseEmitter(_In_ ParticleEmitter *In_pEmitter);

    /// <summary>
    /// プールの統計情報を取得
    /// </summary>
    struct PoolStats
    {
        int ParticleTotalCount;      // Particle プールの総数
        int ParticleActiveCount;     // 使用中の Particle 数
        int EmitterTotalCount;       // Emitter プールの総数
        int EmitterActiveCount;      // 使用中の Emitter 数
    };
    PoolStats GetStats();

private:
	ParticlePoolManager();
	~ParticlePoolManager();

    /// <summary>
    /// Particle プールを拡張
    /// </summary>
    void ExpandParticlePool(_In_ int In_AdditionalSize);

    /// <summary>
    /// Emitter プールを拡張
    /// </summary>
    void ExpandEmitterPool(_In_ int In_AdditionalSize);

private:
    // パーティクルプール
    std::vector<Particle> m_ParticlePool;           // 実体を保持
    std::vector<Particle *> m_AvailableParticles;    // 使用可能なポインタ
    std::mutex m_ParticleMutex;                     // スレッドセーフ用
	// エミッタープール
    std::vector<std::unique_ptr<ParticleEmitter>> m_EmitterPool;
    std::vector<ParticleEmitter *> m_AvailableEmitters;
    std::mutex m_EmitterMutex;
};
