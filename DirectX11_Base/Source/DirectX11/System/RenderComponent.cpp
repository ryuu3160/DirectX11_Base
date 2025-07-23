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
#include "RenderManager.hpp"
#include "System/Component/Camera.hpp"

RenderComponent::RenderComponent()
	: m_nLayer(0), m_pViewCamera(nullptr), m_pCameraObj(nullptr)
	, m_RenderManager(RenderManager::GetInstance())
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	if (NullCheck(In_Camera, NCMode::OUTPUT, "Camera is null in RenderComponent::SetCamera."))
		return;

	m_pViewCamera = In_Camera->GetComponent<Camera>();

	if(NullCheck(m_pViewCamera, NCMode::OUTPUT, "Camera component is null in RenderComponent::SetCamera."))
		return;

	m_pCameraObj = In_Camera;
}

void RenderComponent::ExecuteUpdate() noexcept
{
}

void RenderComponent::ExecuteDraw() noexcept
{
	if(NullCheck(m_pViewCamera, NCMode::OUTPUT, "Camera is null in RenderComponent::ExecuteDraw."))
		return;

	m_RenderManager.AddRenderComponent(this, m_nLayer);
}

void RenderComponent::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<FilePath>(&m_AssetPath);
}
