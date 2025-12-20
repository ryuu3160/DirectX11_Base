/*+===================================================================
	File: SphereCollider.hpp
	Summary: スフィアコライダーのコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 07:51:52 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Collider/ColliderBase.hpp"

/// <summary>
/// SphereColliderクラス
/// </summary>
class SphereCollider : public ColliderBase
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	/// <summary>
	/// 半径を設定します。
	/// </summary>
	/// <param name="[In_Radius]">設定する半径の値。</param>
	void SetRadius(_In_ const float &In_Radius) noexcept { m_Radius = In_Radius; }

	bool CheckCollision(_In_ ColliderBase *In_Other) noexcept override;

	void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept override;

private:

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollidingSphereToSphere(_In_ ColliderBase *In_Other) const noexcept;

protected:
	float m_Radius = 1.0f; // 半径

};
