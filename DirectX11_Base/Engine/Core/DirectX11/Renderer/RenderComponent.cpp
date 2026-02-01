/*+===================================================================
	File: RenderComponent.cpp
	Summary: 描画用の基底コンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/20 Sun PM 10:33:28 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderComponent.hpp"
#include "Core/DirectX11/System/RenderManager.hpp"
#include "Core/System/Component/Camera/Camera.hpp"
#include "Core/System/Component/Camera/CameraDCC.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

RenderComponent::RenderComponent(_In_ std::string In_Name)
	: Component(In_Name)
	, m_nLayer(0), m_LayerGroup(LayerGroup::LayerGroup_Default), m_PrevLayerGroup(LayerGroup::LayerGroup_Default)
	, m_RenderManager(RenderManager::GetInstance())
{
}

RenderComponent::~RenderComponent()
{
	m_RenderManager.RemoveRenderComponent(this, m_LayerGroup);

	m_defVS = nullptr;
	m_defPS = nullptr;
}

void RenderComponent::SetLayerGroup(_In_ LayerGroup In_LayerGroup) noexcept
{
	if (In_LayerGroup == m_LayerGroup)
		return;

	m_RenderManager.LayerGroupSortRequest(m_LayerGroup);
	m_LayerGroup = In_LayerGroup;
}

void RenderComponent::SetLayer(_In_ int In_Layer) noexcept
{
	if(In_Layer == m_nLayer)
		return;
	m_RenderManager.LayerSortRequest(m_LayerGroup);
	m_nLayer = In_Layer;
}

void RenderComponent::Init() noexcept
{
	m_RenderManager.AddRenderComponent(this, m_LayerGroup);
}

void RenderComponent::Update(_In_ float In_DeltaTime) noexcept
{
}

void RenderComponent::SaveLoad(_In_ DataAccessor *In_Data)
{
	In_Data->AccessValue<FilePathHold>("AssetPath", &m_AssetPath);
	int LayerGroupInt = static_cast<int>(m_LayerGroup);
	In_Data->AccessValue<int>("LayerGroup", &LayerGroupInt);
	m_LayerGroup = static_cast<LayerGroup>(LayerGroupInt);
	In_Data->AccessValue<int>("Layer", &m_nLayer);
}

#ifdef _DEBUG
void RenderComponent::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	ItemGroup *group = In_pWindow->CreateItem<ItemGroup>(m_Name);
	auto Bind1 = group->CreateGroupItem<ItemBind>("LayerGroup", DebugItem::Kind::Int, reinterpret_cast<void *>(&m_LayerGroup));
	Bind1->SetNoticeFunc([this]() { CheckLayerGroupChange(); });
	auto Bind2 = group->CreateGroupItem<ItemBind>("Layer", DebugItem::Kind::Int, &m_nLayer);
	Bind2->SetNoticeFunc([this]() { CallLayerSortRequest(); });
	group->CreateGroupItem<ItemBind>("AssetPath", DebugItem::Kind::Path, &m_AssetPath);
	group->CreateGroupItem<ItemBind>("IsActive##RenderCmp", DebugItem::Kind::Bool, &m_IsActive);
}
#endif

void RenderComponent::CheckLayerGroupChange()
{
	if(m_LayerGroup != m_PrevLayerGroup)
	{
		m_RenderManager.LayerGroupSortRequest(m_PrevLayerGroup);
		m_PrevLayerGroup = m_LayerGroup;
	}
}

void RenderComponent::CallLayerSortRequest()
{
	m_RenderManager.LayerSortRequest(m_LayerGroup);
}
