#include "SkyBoxObj.hpp"
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
#include "DirectX11/Resource/ShaderManager.hpp"

SkyBoxObj::SkyBoxObj()
	: GameObject("SkyBoxObj")
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr)
{
	// スカイボックス描画コンポーネントの生成
	m_pRenderComponent = AddComponent<SkyBoxRenderer>();
	m_pRenderComponent->SetAssetPath("Assets/Model/SkyBox/sky.obj");
	m_pRenderComponent->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	m_pRenderComponent->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	m_pRenderComponent->IsUseMaterialShader(true); // マテリアルシェーダーを使用する
}

SkyBoxObj::~SkyBoxObj()
{
	m_pRenderComponent = nullptr;
}

void SkyBoxObj::SetFilePath(_In_ const FilePath &In_Path) noexcept
{
	if (m_pRenderComponent)
		m_pRenderComponent->SetAssetPath(In_Path);
}

void SkyBoxObj::SetCamera(_In_ GameObject * In_Cam) noexcept
{
	m_pCameraObj = In_Cam;
	if (m_pRenderComponent && m_pCameraObj)
	{
		m_pRenderComponent->SetCamera(m_pCameraObj);
	}
}

void SkyBoxObj::LateUpdate()
{
	if (NullCheck(m_pCameraObj, NCMode::OUTPUT, "error: SkyBoxObj Error, CameraObj is nullptr"))
		return;

	m_Pos = m_pCameraObj->GetPos();
}

