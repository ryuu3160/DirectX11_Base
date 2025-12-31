/*+===================================================================
	File: Particle.hpp
	Summary: パーティクルクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:20:41 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

/// <summary>
/// Particleクラス
/// </summary>
class Particle
{
	friend class ParticleEmitter;
	friend class ParticleEffect;
public:
	Particle();
	~Particle() = default;

    /// <summary>
    /// パーティクルの更新処理
    /// </summary>
	/// <param name="[In_DeltaTime]">デルタタイム（秒）</param>
	/// <param name="[In_Gravity]">重力ベクトル</param>
    void Update(_In_ float In_DeltaTime, _In_ const DirectX::XMFLOAT3 &In_Gravity);

    /// <summary>
    /// パーティクルのリセット
    /// </summary>
	void Reset();

    /// <summary>
    /// 生存率を取得 (0.0～1.0)
    /// </summary>
	/// <returns>生存率を返します</returns>
	float GetLifeRatio() const noexcept;

private:
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT4 m_Color;
	DirectX::XMFLOAT2 m_Size;
	DirectX::XMFLOAT3 m_Rotation; // Pitch, Yaw, Roll

	float m_LifeTime;      // 生存時間
	float m_MaxLifeTime;   // 最大生存時間
	float m_GravityScale;  // 重力の影響度

	bool m_IsActive;
};
