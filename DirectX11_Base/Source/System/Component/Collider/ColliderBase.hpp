/*+===================================================================
	File: ColliderBase.hpp
	Summary: 当たり判定のベースクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 05:06:21 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Component.hpp"

// ==============================
//  前方宣言
// ==============================
class TreeData;

// ==============================
//  列挙型定義
// ==============================
enum ColliderType
{
	COLLIDER_NONE = -1, // 無効
	COLLIDER_SPHERE, // 球
	COLLIDER_BOX,   // 箱



	COLLIDER_MAX,   // 最大値
};

/// <summary>
/// ColliderBaseクラス
/// </summary>
class ColliderBase : public Component
{
public:
	ColliderBase(_In_ std::string In_Name);
	virtual ~ColliderBase();

	void SaveLoad(_In_ DataAccessor *In_Data) final;

	void Init() noexcept override;

	void Update(_In_ float In_Tick) noexcept override;

	void FixedUpdate(_In_ double In_FixedTick) noexcept override;

	virtual void CheckCollision(_In_ ColliderBase *In_Other) noexcept = 0;

	ColliderType GetType() const noexcept { return m_Type; }

	const bool &IsTrigger() const noexcept { return m_IsTrigger; }
	/// <summary>
	/// 衝突が発生しているかどうかを取得する。
	/// </summary>
	/// <returns>衝突状態を示す真偽値への参照。</returns>
	const bool &IsCollision() const noexcept { return m_IsCollision; }

	void SetIsTrigger(_In_ const bool &In_IsTrigger) noexcept { m_IsTrigger = In_IsTrigger; }

	void SetIsCollision(_In_ const bool &In_IsCollision) noexcept { m_IsCollision = In_IsCollision; }

	TreeData *GetTreeData() noexcept { return m_pTreeData.get(); }

protected:
	std::shared_ptr<TreeData> m_pTreeData; // オクツリー用データ
	ColliderType m_Type; // コライダーの形状
	bool m_IsTrigger; // トリガーかどうか
	bool m_IsCollision; // 衝突しているかどうか

private:
	CollisionManager &m_CollisionManager;
};
