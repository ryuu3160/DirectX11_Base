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
#include "Core/System/Component/Collider/ColliderBase.hpp"

/// <summary>
/// BoxColliderクラス
/// </summary>
class BoxCollider : public ColliderBase
{
	friend class CapsuleCollider;
public:
	BoxCollider();
	virtual ~BoxCollider();

	void SaveLoad(_In_ DataAccessor *In_Data) override;

	void Init() noexcept override;
	void Update(_In_ float In_DeltaTime) noexcept override;

	inline DirectX::XMFLOAT3 GetSize() const { return m_HalfExtents * 2.0f; }
	inline DirectX::XMFLOAT3 GetHalfExtents() const { return m_HalfExtents; }
	inline DirectX::XMFLOAT3 GetRotation() const { return m_Rotation; }
	
	inline void SetSize(_In_ const DirectX::XMFLOAT3 &In_Size) { m_HalfExtents = In_Size * 0.5f; }
	inline void SetHalfExtents(_In_ const DirectX::XMFLOAT3 &In_HalfExtents) { m_HalfExtents = In_HalfExtents; }
	inline void SetRotation(_In_ const DirectX::XMFLOAT3 &In_EulerAnglesDegrees) { m_Rotation = In_EulerAnglesDegrees; }
	
	// ワールド座標系での情報を取得
	inline DirectX::XMFLOAT3 GetWorldCenter() const { return m_WorldCenter; }
	inline DirectX::XMFLOAT3 GetAxisX() const { return m_AxisX; }
	inline DirectX::XMFLOAT3 GetAxisY() const { return m_AxisY; }
	inline DirectX::XMFLOAT3 GetAxisZ() const { return m_AxisZ; }

	/// <summary>
	/// 軸平行境界ボックス (AABB) を取得します
	/// </summary>
	/// <param name="[Out_LeftTopFront]">境界ボックスの左上前の頂点座標</param>
	/// <param name="[Out_RightBottomBack]">境界ボックスの右下後ろの頂点座標</param>
	void GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept override;

	void DrawColliderOutline(_In_ Gizmos *In_Gizmos) noexcept override;

protected:
#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif // _DEBUG

private:
	/// <summary>
	/// DrawGizmosのヘルパー関数：ワールド座標系での頂点座標を取得する
	/// </summary>
	void GetLocalVertices(_Out_ DirectX::XMFLOAT3 outVertices[8]) const noexcept;

	/// <summary>
	/// 指定された軸上に投影されたバウンディングボックスの半径を計算します
	/// </summary>
	/// <param name="[In_HalfExtents]">バウンディングボックスの半分のサイズ(各軸方向の範囲)</param>
	/// <param name="[In_AxisX]">バウンディングボックスのローカルX軸方向ベクトル</param>
	/// <param name="[In_AxisY]">バウンディングボックスのローカルY軸方向ベクトル</param>
	/// <param name="[In_AxisZ]">バウンディングボックスのローカルZ軸方向ベクトル</param>
	/// <param name="[In_Axis]">投影先の軸を表すベクトル</param>
	/// <returns>指定された軸上に投影されたバウンディングボックスの半径</returns>
	float GetProjectedRadius(_In_ const DirectX::XMFLOAT3 &In_HalfExtents, _In_ const DirectX::XMFLOAT3 &In_AxisX,
		_In_ const DirectX::XMFLOAT3 &In_AxisY, _In_ const DirectX::XMFLOAT3 &In_AxisZ, _In_ const DirectX::XMFLOAT3 &In_Axis) const noexcept;

	/// <summary>
	/// 分離軸チェックのヘルパー関数
	/// </summary>
	/// <param name="[In_Diff]">2つの物体の中心間の差分ベクトル</param>
	/// <param name="[In_Axis]">分離テストを実行する軸</param>
	/// <param name="[In_RadiusA]">最初の物体の有効半径</param>
	/// <param name="[In_RadiusB]">2番目の物体の有効半径</param>
	/// <returns>重なっていればtrueを返す</returns>
	bool CheckAxis(_In_ const DirectX::XMFLOAT3 &In_Diff, _In_ const DirectX::XMFLOAT3 &In_Axis,
		_In_ float In_RadiusA, _In_ float In_RadiusB) const noexcept;

	/// <summary>
	/// 線分とOBBの最短距離の二乗を計算
	/// </summary>
	/// <param name="[In_SegA]">線分の始点</param>
	/// <param name="[In_SegB]">線分の終点</param>
	/// <returns>最短距離の二乗</returns>
	float SegmentToOBBDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_SegA, _In_ const DirectX::XMFLOAT3 &In_SegB) const noexcept;

	/// <summary>
	/// 点とOBBの最短距離の二乗を計算
	/// </summary>
	/// <param name="[In_Point]">点の座標</param>
	/// <returns>最短距離の二乗</returns>
	float PointToOBBDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_Point) const noexcept;

	// ワールド座標系での線分情報を更新
	void UpdateWorldSegment() noexcept;

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がBoxColliderの場合の当たり判定
	bool IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept override;
	// 相手がCapsuleColliderの場合の当たり判定
	bool IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept override;

protected:
	// サイズ
	DirectX::XMFLOAT3 m_HalfExtents;
	// コライダー自体の回転
	DirectX::XMFLOAT3 m_Rotation; // (オイラー角、度数法)

	// ワールド空間での軸方向
	DirectX::XMFLOAT3 m_WorldCenter;  // 中心位置（ワールド）
	DirectX::XMFLOAT3 m_AxisX; // ローカルX軸のワールド方向
	DirectX::XMFLOAT3 m_AxisY; // ローカルY軸のワールド方向
	DirectX::XMFLOAT3 m_AxisZ; // ローカルZ軸のワールド方向
};
