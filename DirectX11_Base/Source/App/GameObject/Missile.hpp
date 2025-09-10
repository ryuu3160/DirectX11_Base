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
#include "sal.h"
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

	void SetSpeed(_In_ const float &In_Speed) noexcept;

	void SetStartPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept { m_StartPos = In_Pos; }

private:
	
	/// <summary>
	/// 開始地点からの距離が一定以上になったら自動で消滅させる
	/// </summary>
	/// <returns>自動消滅させたらtrueを返す</returns>
	bool IsAutoDestroy() noexcept;

private:
	float m_Speed;
	DirectX::XMFLOAT3 m_StartPos;
	Ease::DataDuration m_EaseData;
};
