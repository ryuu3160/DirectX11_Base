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

	struct ColliderPairKey
	{
		ColliderBase *pColliderA;
		ColliderBase *pColliderB;

		ColliderPairKey(_In_ ColliderBase *In_pColl1, _In_ ColliderBase *In_pColl2)
		{
			if(In_pColl1 < In_pColl2)
			{
				pColliderA = In_pColl1;
				pColliderB = In_pColl2;
			}
			else
			{
				pColliderA = In_pColl2;
				pColliderB = In_pColl1;
			}
		}
		bool operator==(_In_ const ColliderPairKey &In_Other) const noexcept
		{
			return (pColliderA == In_Other.pColliderA) && (pColliderB == In_Other.pColliderB);
		}
	};
	struct ColliderPairHash
	{
		size_t operator()(_In_ ColliderPairKey const &In_CollPair) const noexcept
		{
			// ColliderBaseへのポインタをuintptr_tに変換してハッシュ合成
			auto ha = std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(In_CollPair.pColliderA));
			auto hb = std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(In_CollPair.pColliderB));
			return ha ^ (hb + 0x9e3779b97f4a7c15ULL + (ha << 6) + (ha >> 2));
		}
	};

	struct ColliderPairInfo
	{
		bool bIsHit = false;
		bool bIsHitPrev = false;
	};

	using ColliderPairMap = std::unordered_map<ColliderPairKey, ColliderPairInfo, ColliderPairHash>;

private:
	CollisionManager();
	~CollisionManager();

	bool CreateNewCell(_In_ int In_MortonNumber);

	int GetMortonNumber(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack);

	int Get3DMortonOrder(_In_ BYTE In_X, _In_ BYTE In_Y, _In_ BYTE In_Z) const noexcept;

	// 座標->線形8分木要素番号変換
	int GetPointElem(_In_ const DirectX::XMFLOAT3 In_Point) const noexcept;

	/// <summary>
	/// コライダーのモートン番号を取得する関数
	/// </summary>
	/// <param name="[In_Collider]">モートン番号を取得する対象のコライダーへのポインタ</param>
	/// <returns>コライダーが属するモートン番号を返します。</returns>
	int GetMortonNumberOfCollider(_In_ ColliderBase *In_Collider) noexcept;

	/// <summary>
	/// オクツリーにオブジェクトを登録する関数
	/// </summary>
	/// <param name="[In_Collider]">登録するコライダーへのポインタ</param>
	/// <returns>登録が成功した場合は true、失敗した場合は false を返します。</returns>
	bool RegisterObjectToOctree(_In_ ColliderBase *In_Collider) noexcept;

	// 衝突判定リストを作成する
	int GetAllCollisionList(_In_ ColliderPairMap &In_ColPairs);

	// 空間内で衝突リストを作成する
	bool GetCollisionList(_In_ int In_Elem, _Inout_ ColliderPairMap &Inout_ColPairs, _Inout_ std::list<ColliderBase *> &Inout_ColStac);
	
	/// <summary>
	/// 指定したコライダーに関連する衝突ペアを削除する関数
	/// </summary>
	/// <param name="[In_Collider]">削除するコライダーのポインタ</param>
	void RemoveColliderPair(_In_ ColliderBase *In_Collider);

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
	ColliderPairMap m_ColliderPairList; // 衝突判定リスト
};
