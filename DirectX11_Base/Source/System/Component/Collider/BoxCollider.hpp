/*+===================================================================
	File: BoxCollider.hpp
	Summary: BOXコライダークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/21 Sun AM 04:08:36 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Collider/ColliderBase.hpp"

/// <summary>
/// BoxColliderクラス
/// </summary>
class BoxCollider : public ColliderBase
{
public:
	BoxCollider();
	virtual ~BoxCollider();

	DirectX::XMFLOAT3 GetLeftTopFront() const noexcept { return m_LeftTopFront; }
	void SetLeftTopFront(_In_ const DirectX::XMFLOAT3 &In_LeftTopFront) noexcept { m_LeftTopFront = In_LeftTopFront; }
	DirectX::XMFLOAT3 GetRightBottomBack() const noexcept { return m_RightBottomBack; }
	void SetRightBottomBack(_In_ const DirectX::XMFLOAT3 &In_RightBottomBack) noexcept { m_RightBottomBack = In_RightBottomBack; }

	DirectX::XMFLOAT3 GetHalfExtents() const noexcept
	{
		return DirectX::XMFLOAT3(
			(m_RightBottomBack.x - m_LeftTopFront.x) * 0.5f,
			(m_LeftTopFront.y - m_RightBottomBack.y) * 0.5f,
			(m_RightBottomBack.z - m_LeftTopFront.z) * 0.5f
		);
	}

	bool CheckCollision(_In_ ColliderBase *In_Other) noexcept override;

	void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept override;

private:

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollidingBoxToSphere(_In_ ColliderBase *In_Other) const noexcept;

	bool IsCollidingBoxToBox(_In_ ColliderBase *In_Other) const noexcept;

protected:
	DirectX::XMFLOAT3 m_LeftTopFront; // 左上前の頂点座標
	DirectX::XMFLOAT3 m_RightBottomBack; // 右下後ろの頂点座標
};
