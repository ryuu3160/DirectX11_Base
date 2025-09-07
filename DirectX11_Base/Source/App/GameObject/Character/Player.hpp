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
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	void Update() override;

	void SetCamera(_In_ GameObject *In_Camera) noexcept;

private:

	void UpdateMovement();

	void UpdateShoot();

	void UpdateReload();

	float GetSharpPitch();

private:
	GameObject *m_pCamera;
	float m_fSpeed;
	int m_MissileIndex;

	std::vector<std::pair<int, float>> m_ReloadTimer; // リロードタイマー
};
