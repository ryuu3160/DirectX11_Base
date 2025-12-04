/*+===================================================================
	File: CollisionManager.hpp
	Summary: 当たり判定管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 05:05:18 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Collider/ColliderBase.hpp"
#include "OctreeCell.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	inline constexpr int cx_MaxLevel = 7;
	inline constexpr int cx_DivisionNumber = 8; // オクツリーの分割数
}

/// <summary>
/// CollisionManagerクラス
/// </summary>
class CollisionManager : public Singleton<CollisionManager>
{
	friend class Singleton<CollisionManager>;
public:
	CollisionManager();
	~CollisionManager();

	void InitOctreeSpace(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack, _In_ const int In_Level) noexcept;

	/// <summary>
	/// ColliderBase ポインタを受け取り、コライダーコンポーネントを追加します。
	/// </summary>
	/// <param name="[In_Collider]">追加するコライダーの基底クラスへのポインタ。</param>
	void AddColliderComponent(_In_ ColliderBase *In_Collider) noexcept;

	/// <summary>
	/// 指定したコライダのコンポーネントを削除する関数
	/// </summary>
	/// <param name="[In_Collider]">削除対象の ColliderBase への入力ポインタ</param>
	void RemoveColliderComponent(_In_ ColliderBase *In_Collider);

	/// <summary>
	/// 指定したコライダーに関連する衝突セル情報を更新する
	/// </summary>
	/// <param name="[In_Collider]">更新対象のコライダーを指すポインタ。呼び出し側は有効な ColliderBase オブジェクトへのポインタを渡すことが想定されます</param>
	void UpdateCollisionCells(_In_ ColliderBase *In_Collider);

	/// <summary>
	/// すべての衝突を検査します。
	/// </summary>
	void CheckAllCollisions() noexcept;

private:

	bool CreateNewCell(_In_ int In_MortonNumber);

	int GetMortonNumber(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack);

	int Get3DMortonOrder(_In_ BYTE In_X, _In_ BYTE In_Y, _In_ BYTE In_Z) const noexcept;

	// 座標->線形8分木要素番号変換
	int GetPointElem(_In_ const DirectX::XMFLOAT3 In_Point) const noexcept;

	int GetMortonNumberOfCollider(_In_ ColliderBase *In_Collider) noexcept;

	bool RegisterObjectToOctree(_In_ ColliderBase *In_Collider) noexcept;

private:

	int m_OctreeLevel; // オクツリーのレベル
	int m_MaxCellNum; // オクツリーの最大セル数
	int m_ParentShift; // 親ノードを求めるためのシフト量

	DirectX::XMFLOAT3 m_LeftTopFront;
	DirectX::XMFLOAT3 m_RightBottomBack;
	float m_Width;
	float m_Height;
	float m_Depth;
	DirectX::XMFLOAT3 m_Unit; // 奥ツリーの1ユニットの大きさ

	std::array<int, cx_MaxLevel + 1> m_Pow; // 8の累乗を格納する配列

	std::vector<OctreeCell*> m_OctreeCells; // オクツリーセルの配列

	std::vector<ColliderBase *> m_ColliderList; // コライダーリスト
};
