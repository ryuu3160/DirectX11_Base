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
#include "System/CollisionManager/CollisionManager.hpp"
#include "System/CollisionManager/TreeData.hpp"

ColliderBase::ColliderBase(_In_ std::string In_Name)
	: Component(In_Name)
	, m_IsTrigger(true), m_IsCollision(false), m_Type(COLLIDER_NONE)
	, m_CollisionManager(CollisionManager::GetInstance())
	, m_pTreeData(nullptr)
{
	m_pTreeData = std::make_shared<TreeData>();
	m_pTreeData->m_pCollider = this;
}

ColliderBase::~ColliderBase()
{
	m_CollisionManager.RemoveColliderComponent(this);

}

void ColliderBase::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<bool>(&m_IsTrigger);
}

void ColliderBase::Init() noexcept
{
	m_CollisionManager.AddColliderComponent(this);
}

void ColliderBase::Update(_In_ float In_Tick) noexcept
{
}

void ColliderBase::FixedUpdate(_In_ double In_FixedTick) noexcept
{
	m_CollisionManager.UpdateCollisionCells(this);
}
