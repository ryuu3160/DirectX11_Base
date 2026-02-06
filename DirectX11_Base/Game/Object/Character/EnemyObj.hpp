/*+===================================================================
	File: Enemy.hpp
	Summary: 敵キャラクター
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 02:08:15 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// Enemyクラス
/// </summary>
class EnemyObj : public GameObject
{
public:
	EnemyObj(_In_ std::string In_Name);
	~EnemyObj();

	void Awake() noexcept override;

private:
};
