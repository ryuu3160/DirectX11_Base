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
	/// 半径を取得します
	/// </summary>
	/// <returns>現在の半径の値</returns>
	float GetRadius() const noexcept { return m_Radius; }

	/// <summary>
	/// ワールド座標系での中心点を取得します
	/// </summary>
	/// <returns>ワールド座標系での中心座標を表す3次元ベクトル</returns>
	DirectX::XMFLOAT3 GetWorldCenter() const noexcept;

	/// <summary>
	/// 半径を設定します
	/// </summary>
	/// <param name="[In_Radius]">設定する半径の値</param>
	void SetRadius(_In_ const float &In_Radius) noexcept { m_Radius = In_Radius; }

	/// <summary>
	/// 他のコライダーとの衝突を検出します
	/// </summary>
	/// <param name="[In_Other]">衝突を判定する対象のコライダー</param>
	/// <returns>衝突が発生した場合はtrue、それ以外の場合はfalse</returns>
	bool CheckCollision(_In_ ColliderBase *In_Other) noexcept override;

	/// <summary>
	/// 軸平行境界ボックス (AABB) を取得します
	/// </summary>
	/// <param name="[Out_LeftTopFront]">AABBの左上前の角の座標を受け取ります</param>
	/// <param name="[Out_RightBottomBack]">AABBの右下後ろの角の座標を受け取ります</param>
	void GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept override;

	void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept override;

private:

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollidingSphereToSphere(_In_ ColliderBase *In_Other) const noexcept;

	// 相手がBoxColliderの場合の当たり判定
	bool IsCollidingSphereToBox(_In_ ColliderBase *In_Other) const noexcept;

protected:
	float m_Radius = 1.0f; // 半径
};
