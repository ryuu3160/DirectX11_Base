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

ColliderBase::ColliderBase(_In_ std::string In_Name)
	: Component(In_Name)
	, m_IsTrigger(true), m_IsCollision(false), m_Type(COLLIDER_NONE)
{
}

ColliderBase::~ColliderBase()
{
}

void ColliderBase::ReadWrite(_In_ DataAccessor *In_Data)
{
}

void ColliderBase::Update(_In_ float In_Tick) noexcept
{
	CollisionManager::GetInstance().AddColliderComponent(this);
}
