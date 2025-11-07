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
#include "DirectX11/System/RenderManager.hpp"
#include "System/Component/Camera.hpp"
#include "System/Object/CameraDCC.hpp"

RenderComponent::RenderComponent(_In_ std::string In_Name)
	: Component(In_Name)
	, m_nLayer(0), m_LayerGroup(LayerGroup::LayerGroup_Default)
	, m_pViewCamera(nullptr), m_pCameraObj(nullptr)
	, m_RenderManager(RenderManager::GetInstance())
{
}

RenderComponent::~RenderComponent()
{
	m_RenderManager.RemoveRenderComponent(this, m_LayerGroup);

	m_pViewCamera = nullptr;
	m_pCameraObj = nullptr;
	m_defVS = nullptr;
	m_defPS = nullptr;
}

void RenderComponent::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	if (NullCheck(In_Camera, NCMode::OUTPUT, "error: Camera is null in RenderComponent::SetCamera."))
		return;

	m_pViewCamera = In_Camera->GetComponent<Camera>();

	if(NullCheck(m_pViewCamera, NCMode::OUTPUT, "error: Camera component is null in RenderComponent::SetCamera."))
		return;

	m_pCameraObj = In_Camera;
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
#ifdef _DEBUG
	if(!m_pCameraObj)
		m_pCameraObj = GetGameObject()->GetScene()->GetObject<CameraDCC>("MainCamera");
#endif
	if (m_pViewCamera == nullptr && m_pCameraObj)
		m_pViewCamera = m_pCameraObj->GetComponent<Camera>();

	if (NullCheck(m_pViewCamera, NCMode::OUTPUT, "error: Camera is null in RenderComponent::ExecuteDraw."))
		return;

	m_RenderManager.AddRenderComponent(this, m_LayerGroup);
}

void RenderComponent::Update() noexcept
{
}

void RenderComponent::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<FilePathHold>(&m_AssetPath);
}
