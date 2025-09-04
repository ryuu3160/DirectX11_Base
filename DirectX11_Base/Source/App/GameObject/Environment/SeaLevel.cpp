/*+===================================================================
	File: SeaLevel.cpp
	Summary: 海面オブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/09/04 Thu PM 02:17:15 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SeaLevel.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

SeaLevel::SeaLevel()
	: GameObject("SeaLevelObject")
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr)
	, m_pPlayer(nullptr)
{
	SetPos({ 0.0f,0.0f,0.0f });
	SetScale({ 1.0f,1.0f,1.0f });
	SetQuat({ 0.0f,0.0f,0.0f,0.0f });

	// レンダーコンポーネントの設定
	m_pRenderComponent = AddComponent<InstancedModelRenderer>();
	m_pRenderComponent->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
	m_pRenderComponent->SetVertexShader(ShaderManager::GetInstance().GetShader("IVS_InstancedObject"));
	m_pRenderComponent->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	m_pRenderComponent->SetLayer(RenderLayer_Ground); // レイヤーの設定

	// インスタンシングの設定
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 200;
	instanceData.CountZ = 200;
	instanceData.CountY = 1;
	instanceData.StartPos = GetPos();
	instanceData.Scale = GetScale();
	instanceData.Quaternion = GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };
	instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

	m_pRenderComponent->SetAlignInstanceData(instanceData);
}

SeaLevel::~SeaLevel()
{
	m_pRenderComponent = nullptr;
}

void SeaLevel::SetFilePath(_In_ const FilePath &In_Path) noexcept
{
	if (m_pRenderComponent)
		m_pRenderComponent->SetAssetPath(In_Path);
}

void SeaLevel::SetCamera(_In_ GameObject *In_Cam) noexcept
{
	m_pCameraObj = In_Cam;
	if (m_pRenderComponent && m_pCameraObj)
	{
		m_pRenderComponent->SetCamera(m_pCameraObj);
	}
}

void SeaLevel::SetPlayer(_In_ GameObject *In_Player) noexcept
{
	m_pPlayer = In_Player;
}

void SeaLevel::LateUpdate()
{
	if (m_pPlayer)
	{
		auto pos = m_pPlayer->GetPos();
		pos.y = GetPos().y;
		SetPos(pos);
	}
}
