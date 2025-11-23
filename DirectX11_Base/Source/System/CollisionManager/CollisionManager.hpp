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
#include "ColliderBase.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

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
	/// すべての衝突を検査します。
	/// </summary>
	void CheckAllCollisions() noexcept;

private:



private:
	std::vector<ColliderBase *> m_ColliderList; // コライダーリスト
};
