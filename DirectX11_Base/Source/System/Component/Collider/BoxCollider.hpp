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

	void Update(_In_ float In_Tick) noexcept override;

	inline DirectX::XMFLOAT3 GetCenter() const { return m_Center; }
	inline DirectX::XMFLOAT3 GetHalfExtents() const { return m_HalfExtents; }
	
	inline void SetSize(_In_ const DirectX::XMFLOAT3 &size) { m_HalfExtents = size * 0.5f; }
	inline void SetCenter(_In_ const DirectX::XMFLOAT3 &center) { m_Center = center; }
	inline void SetHalfExtents(_In_ const DirectX::XMFLOAT3 &halfExtents) { m_HalfExtents = halfExtents; }
	
	// ワールド座標系での情報を取得
	inline DirectX::XMFLOAT3 GetWorldCenter() const { return m_WorldCenter; }
	inline DirectX::XMFLOAT3 GetAxisX() const { return m_AxisX; }
	inline DirectX::XMFLOAT3 GetAxisY() const { return m_AxisY; }
	inline DirectX::XMFLOAT3 GetAxisZ() const { return m_AxisZ; }

	/// <summary>
	/// 他のコライダーとの衝突判定を行います
	/// </summary>
	/// <param name="[In_Other]">衝突判定を行う対象のコライダー</param>
	/// <returns>衝突している場合はtrue、そうでない場合はfalse</returns>
	bool CheckCollision(_In_ ColliderBase *In_Other) noexcept override;

	/// <summary>
	/// 軸平行境界ボックス (AABB) を取得します
	/// </summary>
	/// <param name="[Out_LeftTopFront]">境界ボックスの左上前の頂点座標</param>
	/// <param name="[Out_RightBottomBack]">境界ボックスの右下後ろの頂点座標</param>
	void GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept override;

	void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept override;

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

	// 相手がSphereColliderの場合の当たり判定
	bool IsCollidingBoxToSphere(_In_ ColliderBase *In_Other) const noexcept;

	bool IsCollidingBoxToBox(_In_ ColliderBase *In_Other) const noexcept;

protected:
	DirectX::XMFLOAT3 m_Center; // 中心位置（ローカル）
	// サイズ
	DirectX::XMFLOAT3 m_HalfExtents;

	// ワールド空間での軸方向
	DirectX::XMFLOAT3 m_WorldCenter;  // 中心位置（ワールド）
	DirectX::XMFLOAT3 m_AxisX; // ローカルX軸のワールド方向
	DirectX::XMFLOAT3 m_AxisY; // ローカルY軸のワールド方向
	DirectX::XMFLOAT3 m_AxisZ; // ローカルZ軸のワールド方向
};
