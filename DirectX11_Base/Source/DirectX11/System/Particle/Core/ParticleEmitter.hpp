/*+===================================================================
	File: ParticleEmitter.hpp
	Summary: ParticleEmitterクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:21:26 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Particle.hpp"

/// <summary>
/// パーティクルの放出設定
/// </summary>
struct EmitterSettings
{
    // 基本設定
    DirectX::XMFLOAT3 Position{ 0.0f, 0.0f, 0.0f };
    float EmitRate = 10.0f;              // 1秒あたりの放出数
    int MaxParticles = 100;              // 最大パーティクル数
    float Duration = -1.0f;              // 放出時間 (-1で無限)
    bool IsLooping = true;               // ループするか

    // パーティクル初期設定
    DirectX::XMFLOAT3 VelocityMin{ -1.0f, 1.0f, -1.0f };
    DirectX::XMFLOAT3 VelocityMax{ 1.0f, 3.0f, 1.0f };

    DirectX::XMFLOAT4 ColorStart{ 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 ColorEnd{ 1.0f, 1.0f, 1.0f, 0.0f };

    DirectX::XMFLOAT2 SizeStart{ 1.0f, 1.0f };
    DirectX::XMFLOAT2 SizeEnd{ 0.5f, 0.5f };

    float LifeTimeMin = 1.0f;
    float LifeTimeMax = 3.0f;

    float GravityScale = 0.0f;

    // 放出形状
    enum class Shape
    {
        Point,      // 点
        Sphere,     // 球
        Cone,       // コーン
        Box,        // ボックス
    };
    Shape EmitShape = Shape::Point;

    float ShapeRadius = 1.0f;    // Sphere/Cone用
    float ShapeAngle = 30.0f;    // Cone用 (度数)
    DirectX::XMFLOAT3 ShapeSize{ 1.0f, 1.0f, 1.0f }; // Box用
};

/// <summary>
/// ParticleEmitterクラス
/// </summary>
class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter() = default;

    /// <summary>
    /// エミッター設定を使用して初期化
    /// </summary>
    /// <param name="[In_Settings]">初期化に使用するエミッター設定への定数参照</param>
    void Init(_In_ const EmitterSettings &In_Settings);

    /// <summary>
    /// 再生開始
    /// </summary>
    void Play();

    /// <summary>
    /// 停止
    /// </summary>
    void Stop();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="[In_DeltaTime]">デルタタイム(秒)</param>
	/// <param name="[In_Gravity]">重力ベクトル</param>
    void Update(_In_ float In_DeltaTime, _In_ const DirectX::XMFLOAT3 &In_Gravity);

    /// <summary>
    /// パーティクルリストの取得
    /// </summary>
	/// <returns>パーティクルのベクターへの定数参照を返します</returns>
    inline const std::vector<Particle> &GetParticles() const noexcept { return m_Particles; }

    /// <summary>
    /// アクティブなパーティクル数を取得
    /// </summary>
	/// <returns>アクティブなパーティクル数を返します</returns>
    inline size_t GetActiveCount() const noexcept { return m_Particles.size(); }

    /// <summary>
    /// 設定の取得
    /// </summary>
	/// <returns>エミッター設定への参照を返します</returns>
    inline EmitterSettings &GetSettings() noexcept { return m_Settings; }

	/// <summary>
	/// 設定の変更
	/// </summary>
	/// <param name="[In_Settings]">新しいエミッター設定への定数参照</param>
    inline void SetSettings(_In_ const EmitterSettings &In_Settings) { m_Settings = In_Settings; }

private:
    /// <summary>
    /// パーティクルを1つ放出
    /// </summary>
    void EmitParticle();

    /// <summary>
    /// 放出位置の生成
    /// </summary>
	/// <returns>生成された位置ベクトルを返します</returns>
    DirectX::XMFLOAT3 GeneratePosition();

    /// <summary>
    /// 速度の生成
    /// </summary>
	/// <returns>生成された速度ベクトルを返します</returns>
    DirectX::XMFLOAT3 GenerateVelocity();

    /// <summary>
    /// ランダム値生成
    /// </summary>
	/// <param name="[In_Min]">最小値</param>
	/// <param name="[In_Max]">最大値</param>
	/// <returns>生成されたランダムな浮動小数点数を返します</returns>
    float RandomRange(_In_ float In_Min, _In_ float In_Max);

private:
    EmitterSettings m_Settings;
    std::vector<Particle> m_Particles;

    float m_EmitTimer;
    float m_DurationTimer;
    bool m_IsPlaying;

    std::mt19937 m_RandomEngine;
};
