/*+===================================================================
	File: CapsuleCollider.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/22 Mon AM 05:11:56 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ColliderBase.hpp"

/// <summary>
/// CapsuleColliderクラス
/// </summary>
class CapsuleCollider : public ColliderBase
{
	friend class SphereCollider;
public:
	CapsuleCollider();
	virtual ~CapsuleCollider();

	void Init() noexcept override;
	void Update(_In_ float In_Tick) noexcept override;

	float GetHeight() const { return m_Height; }
	float GetRadius() const { return m_Radius; }
	DirectX::XMFLOAT3 GetDirection() const { return m_Direction; }

	void SetHeight(_In_ float height) { m_Height = height; }
	void SetRadius(_In_ float radius) { m_Radius = radius; }
	void SetDirection(_In_ DirectX::XMFLOAT3 &direction) { m_Direction = direction; }

	// ワールド座標での線分の端点を取得
	void GetWorldSegment(_Out_ DirectX::XMFLOAT3 &Out_PointA, _Out_ DirectX::XMFLOAT3 &Out_PointB) const noexcept;
	DirectX::XMFLOAT3 GetWorldCenter() const { return m_WorldCenter; }

	// ワールド座標での線分端点
	DirectX::XMFLOAT3 GetWorldPointA() const { return m_WorldPointA; }
	DirectX::XMFLOAT3 GetWorldPointB() const { return m_WorldPointB; }

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
	// 相手がBoxColliderの場合の当たり判定
	bool IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がSphereColliderの場合の当たり判定
	bool IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がCapsuleColliderの場合の当たり判定
	bool IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept override;

	// ワールド座標系での線分情報を更新
	void UpdateWorldSegment() noexcept;

	// 点と線分の最短距離の二乗を計算
	float PointToSegmentDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_Point, _In_ const DirectX::XMFLOAT3 &In_SegmentA,
		_In_ const DirectX::XMFLOAT3 &In_SegmentB, _Out_ float *Out_Param = nullptr) const noexcept;

	// 線分同士の最短距離の二乗を計算
	float SegmentToSegmentDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_A1, _In_ const DirectX::XMFLOAT3 &In_A2,
		_In_ const DirectX::XMFLOAT3 &In_B1, _In_ const DirectX::XMFLOAT3 &In_B2, _Out_ float* Out_Seg = nullptr, _Out_ float* Out_Param = nullptr) const noexcept;

	DirectX::XMFLOAT3 GetLocalPointA() const;
	DirectX::XMFLOAT3 GetLocalPointB() const;

protected:
	// ローカル座標系のパラメータ
	float m_Height;                  // 全体の高さ(線分の長さ + 半径*2)
	float m_Radius;                  // 半径
	DirectX::XMFLOAT3 m_Direction;  // 向き(ローカル、通常は(0,1,0))


	// ワールド座標系(Update で計算)
	DirectX::XMFLOAT3 m_WorldCenter;
	DirectX::XMFLOAT3 m_WorldPointA; // 線分の上端
	DirectX::XMFLOAT3 m_WorldPointB; // 線分の下端
	DirectX::XMFLOAT3 m_WorldDirection;
};
