/*+===================================================================
	File: SkyBoxObj.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/30 Sat AM 01:13:26 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SkyBoxObj.hpp"
#include "Core/DirectX11/ResourceManager/ShaderManager.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

SkyBoxObj::SkyBoxObj()
	: SkyBoxObj("SkyBoxObj")
{
}

SkyBoxObj::SkyBoxObj(_In_ std::string_view In_Name)
	: GameObject(In_Name)
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr)
{
}

SkyBoxObj::~SkyBoxObj()
{
	m_pCameraObj = nullptr;
	m_pRenderComponent = nullptr;
}

void SkyBoxObj::Awake() noexcept
{
	// スカイボックス描画コンポーネントの生成
	m_pRenderComponent = AddComponent<SkyBoxRenderer>();
	m_pRenderComponent->SetAssetPath("Engine/Assets/Model/SkyBox/sky.obj");
	auto &ShaderManager = ShaderManager::GetInstance();
	m_pRenderComponent->SetVertexShader(ShaderManager.GetShader("VS_Object"));
	m_pRenderComponent->SetPixelShader(ShaderManager.GetShader("PS_TexColor"));
}
