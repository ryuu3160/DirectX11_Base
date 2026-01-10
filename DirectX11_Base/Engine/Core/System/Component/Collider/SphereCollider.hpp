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
#include "Core/System/Component/Collider/ColliderBase.hpp"

/// <summary>
/// SphereColliderクラス
/// </summary>
class SphereCollider : public ColliderBase
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void SaveLoad(_In_ DataAccessor *In_Data) override;

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
	/// 軸平行境界ボックス (AABB) を取得します
	/// </summary>
	/// <param name="[Out_LeftTopFront]">AABBの左上前の角の座標を受け取ります</param>
	/// <param name="[Out_RightBottomBack]">AABBの右下後ろの角の座標を受け取ります</param>
	void GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept override;

	void DrawColliderOutline(_In_ Gizmos *In_Gizmos) noexcept override;

protected:

#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif // _DEBUG

private:

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がBoxColliderの場合の当たり判定
	bool IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がCapsuleColliderの場合の当たり判定
	bool IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept override;

protected:
	float m_Radius = 1.0f; // 半径
};
