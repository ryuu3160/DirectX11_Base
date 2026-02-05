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
#include "Core/System/Managers/CollisionManager/CollisionManager.hpp"
#include "Core/System/Managers/CollisionManager/TreeData.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Managers/DebugManager/DebugItem.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

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
	m_pTreeData = nullptr;
}

void ColliderBase::SaveLoad(_In_ DataAccessor *In_Data)
{
	In_Data->AccessValue<bool>("IsTrigger", &m_IsTrigger);
	In_Data->AccessValue<DirectX::XMFLOAT3>("LocalCenter", &m_Center);
}

void ColliderBase::Init() noexcept
{
	m_CollisionManager.AddColliderComponent(this);
}

void ColliderBase::Update(_In_ float In_DeltaTime) noexcept
{
}

void ColliderBase::FixedUpdate(_In_ double In_FixedDeltaTime) noexcept
{
	m_CollisionManager.UpdateCollisionCells(this);
}

bool ColliderBase::CheckCollision(_In_ ColliderBase *In_Other) noexcept
{
	// 相手がいなければ処理しない
	if(!In_Other)
		return false;
	// 自分自身とは当たらない
	if(In_Other == this)
		return false;

	switch(In_Other->GetType())
	{
	case COLLIDER_SPHERE:
		// 球対球の当たり判定
		if(IsCollisionToSphere(In_Other))
		{
			m_IsCollision = true;
			In_Other->SetIsCollision(true);
			return true;
		}
		else
		{
			m_IsCollision = false;
			In_Other->SetIsCollision(false);
			return false;
		}
		break;

	case COLLIDER_BOX:
		// 球対箱の当たり判定
		if(IsCollisionToBox(In_Other))
		{
			m_IsCollision = true;
			In_Other->SetIsCollision(true);
			return true;
		}
		else
		{
			m_IsCollision = false;
			In_Other->SetIsCollision(false);
			return false;
		}
		break;
	case COLLIDER_CAPSULE:
		// 球対カプセルの当たり判定
		if(IsCollisionToCapsule(In_Other))
		{
			m_IsCollision = true;
			In_Other->SetIsCollision(true);
			return true;
		}
		else
		{
			m_IsCollision = false;
			In_Other->SetIsCollision(false);
			return false;
		}
		break;
	}

	return false;
}

void ColliderBase::CallOnEnter(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pGameObject || !m_pGameObject->IsActive())
		return;

	if(m_IsTrigger)
	{
		m_pGameObject->CallOnTriggerEnter(In_Other);
	}
	else
	{
		m_pGameObject->CallOnCollisionEnter(In_Other);
	}
}

void ColliderBase::CallOnStay(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pGameObject || !m_pGameObject->IsActive())
		return;
	if(m_IsTrigger)
	{
		m_pGameObject->CallOnTriggerStay(In_Other);
	}
	else
	{
		m_pGameObject->CallOnCollisionStay(In_Other);
	}
}

void ColliderBase::CallOnExit(_In_ ColliderBase *In_Other) noexcept
{
	if(!m_pGameObject || !m_pGameObject->IsActive())
		return;
	if(m_IsTrigger)
	{
		m_pGameObject->CallOnTriggerExit(In_Other);
	}
	else
	{
		m_pGameObject->CallOnCollisionExit(In_Other);
	}
}

void ColliderBase::DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept
{
	if(!m_pGameObject || !m_pGameObject->IsActive())
		return;
	DrawColliderOutline(In_Gizmos);
}

#ifdef _DEBUG
void ColliderBase::Inspector(_In_ ItemGroup *In_pGroup)
{
	In_pGroup->CreateGroupItem<ItemBind>("IsTrigger", DebugItem::Kind::Bool, &m_IsTrigger);
	In_pGroup->CreateGroupItem<ItemBind>("LocalCenter", DebugItem::Kind::Vector, &m_Center);
}
#endif
