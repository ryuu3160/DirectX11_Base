/*+===================================================================
	File: MissileObj.hpp
	Summary: ミサイルオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/8 Mon AM 12:44:37 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "sal.h"
#include "Engine.hpp"
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
class MissileObj : public GameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MissileObj(_In_ std::string In_Name, _In_ const bool &In_IsAddCollider);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MissileObj();

	void Awake() noexcept override;

	void SetSpeed(_In_ const float &In_Speed) noexcept;

	void SetStartPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept { m_StartPos = In_Pos; }

	void SetTarget(_In_ GameObject *In_pTarget) noexcept { m_pTarget = In_pTarget; }

private:
	bool m_IsAddCollider;
	float m_Speed;
	std::string m_MissileName;
	DirectX::XMFLOAT3 m_StartPos;
	Ease::DataDuration m_EaseData;
	GameObject *m_pTarget;
};
