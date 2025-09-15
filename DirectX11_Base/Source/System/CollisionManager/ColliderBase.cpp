/*+===================================================================
	File: ColliderBase.cpp
	Summary: 当たり判定のベースクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 05:06:28 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ColliderBase.hpp"
#include "CollisionManager.hpp"

ColliderBase::ColliderBase()
	: m_IsTrigger(true), m_IsCollision(false), m_Type(COLLIDER_NONE)
{
}

ColliderBase::~ColliderBase()
{
}

void ColliderBase::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<bool>(&m_IsTrigger);
	In_Data->Access<bool>(&m_IsCollision);
}

void ColliderBase::ExecuteUpdate() noexcept
{
	CollisionManager::GetInstance().AddColliderComponent(this);
}
