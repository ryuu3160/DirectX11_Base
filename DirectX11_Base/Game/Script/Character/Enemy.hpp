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
class Enemy : public Component
{
public:
	Enemy(_In_ std::string_view In_Name = "Enemy");
	~Enemy();

	void Awake() noexcept override;
	void Init() noexcept override;

	void Update(_In_ float In_DeltaTime) noexcept override;

	void SetCamera(_In_ GameObject *In_Camera) noexcept;

	/// <summary>
	/// 機体が破壊されたかどうか
	/// </summary>
	/// <returns>機体が破壊されている場合はtrueを返します</returns>
	bool IsDestroyed() const noexcept { return m_IsDestroyed; }

private:
	GameObject *m_pCamera;

	bool m_IsDestroyed;		// 自機が破壊されたかどうか
};
