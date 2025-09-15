/*+===================================================================
	File: Player.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 04:04:37 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/GameObject.hpp"
#include "App/GameObject/Missile.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	// ミサイル関連
	const inline constexpr int cx_MissileMax = 4; // ミサイルの最大数
}

/// <summary>
/// Playerクラス
/// </summary>
class Player : public GameObject
{
public:
	Player();
	~Player();

	void Update() override;

	void SetCamera(_In_ GameObject *In_Camera) noexcept;

	/// <summary>
	/// 機体が破壊されたかどうか
	/// </summary>
	/// <returns>機体が破壊されている場合はtrueを返します</returns>
	bool IsDestroyed() const noexcept { return m_IsDestroyed; }

	void SetTarget(_In_ GameObject *In_Target) noexcept { m_pTarget = In_Target; }

private:

	void UpdateMovement();

	void UpdateShoot();

	void UpdateReload();

	void UpdateChildMissile();

	float GetSharpPitch();

private:
	GameObject *m_pCamera;
	GameObject *m_pTarget; // 攻撃対象
	float m_fSpeed;			// 現在の速度
	int m_ShotMissileNum;	// 発射したミサイルの番号

	std::vector<int> m_MissileIndices; // ミサイルのインデックスリスト
	std::vector<std::pair<int, float>> m_ReloadTimer; // リロードタイマー

	bool m_IsDestroyed;		// 自機が破壊されたかどうか
};
