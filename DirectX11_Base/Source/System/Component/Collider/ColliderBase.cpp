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
#include "System/DebugManager/DebugItem.hpp"

ColliderBase::ColliderBase(_In_ std::string In_Name)
	: Component(In_Name)
	, m_IsTrigger(true), m_IsCollision(false), m_Type(COLLIDER_NONE)
	, m_CollisionManager(CollisionManager::GetInstance())
	, m_pTreeData(nullptr)
	, m_Center{ 0.0f, 0.0f, 0.0f }
{
	m_pTreeData = std::make_shared<TreeData>();
	m_pTreeData->m_pCollider = this;
}

ColliderBase::~ColliderBase()
{
	m_CollisionManager.RemoveColliderComponent(this);
}

void ColliderBase::SaveLoad(_In_ DataAccessor *In_Data)
{
	In_Data->AccessValue<bool>("IsTrigger", & m_IsTrigger);
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

void ColliderBase::CallOnEnter(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pTransform || !m_pTransform->IsActive())
		return;

	if(m_IsTrigger)
	{
		m_pTransform->CallOnTriggerEnter(In_Other);
	}
	else
	{
		m_pTransform->CallOnCollisionEnter(In_Other);
	}
}

void ColliderBase::CallOnStay(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pTransform || !m_pTransform->IsActive())
		return;
	if(m_IsTrigger)
	{
		m_pTransform->CallOnTriggerStay(In_Other);
	}
	else
	{
		m_pTransform->CallOnCollisionStay(In_Other);
	}
}

void ColliderBase::CallOnExit(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pTransform || !m_pTransform->IsActive())
		return;
	if(m_IsTrigger)
	{
		m_pTransform->CallOnTriggerExit(In_Other);
	}
	else
	{
		m_pTransform->CallOnCollisionExit(In_Other);
	}
}

#ifdef _DEBUG
void ColliderBase::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	ItemGroup *group = In_pWindow->CreateItem<ItemGroup>(m_Name);
	group->CreateGroupItem<ItemBind>("IsTrigger", DebugItem::Kind::Bool, &m_IsTrigger);
}
#endif
