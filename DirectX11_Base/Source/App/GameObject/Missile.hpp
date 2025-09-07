/*+===================================================================
	File: Missile.hpp
	Summary: ミサイルオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/8 Mon AM 12:44:37 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/GameObject.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float MissileScale = 0.007f; // ミサイルのスケール
}

/// <summary>
/// Missileクラス
/// </summary>
class Missile : public GameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Missile();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Missile();

	void Update() override;

	void SetSpeed(_In_ const float &In_Speed) noexcept { m_Speed = In_Speed; }

private:
	float m_Speed;
};
