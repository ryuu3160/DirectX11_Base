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
// ==============================
//	定数定義
// ==============================
namespace
{

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

private:

	void UpdateMovement();

	void UpdateShoot();

private:
	float m_fSpeed;
};
