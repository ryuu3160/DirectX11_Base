/*+===================================================================
	File: SeaLevel.cpp
	Summary: 海面オブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/09/04 Thu PM 02:17:15 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SeaLevelObj.hpp"
#include "Game/Script/Environment/SeaLevel.hpp"

SeaLevelObj::SeaLevelObj(_In_ std::string_view In_Name, _In_ bool In_IsInstance)
	: GameObject(In_Name.data())
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr)
	, m_pPlayer(nullptr)
	, m_IsInstance(In_IsInstance)
	, m_PatternScale({ 1.0f,1.0f })
{
}

SeaLevelObj::~SeaLevelObj()
{
	m_pRenderComponent = nullptr;
}

void SeaLevelObj::Awake() noexcept
{
	SetPosition({ 0.0f,0.0f,0.0f });
	SetScale({ 1.0f,1.0f,1.0f });
	SetQuat({ 0.0f,0.0f,0.0f,0.0f });

	// レンダーコンポーネントの設定
	if(m_IsInstance)
	{
		// インスタンシング
		m_pRenderComponent = AddComponent<InstancedModelRenderer>();
		reinterpret_cast<InstancedModelRenderer *>(m_pRenderComponent)->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
		reinterpret_cast<InstancedModelRenderer *>(m_pRenderComponent)->SetVertexShader(ShaderManager::GetInstance().GetShader("IVS_InstancedObject"));
		reinterpret_cast<InstancedModelRenderer *>(m_pRenderComponent)->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
		reinterpret_cast<InstancedModelRenderer *>(m_pRenderComponent)->SetLayer(-1); // レイヤーの設定

		// インスタンシングの設定
		InstancedMesh::AlignInstanceData instanceData;
		instanceData.CountX = 200;
		instanceData.CountZ = 200;
		instanceData.CountY = 1;
		instanceData.StartPos = GetPosition();
		instanceData.Scale = GetScale();
		instanceData.Quaternion = GetQuat();
		instanceData.IsWrite = true;
		instanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };
		instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

		reinterpret_cast<InstancedModelRenderer *>(m_pRenderComponent)->SetAlignInstanceData(instanceData);
	}
	else
	{
		m_pRenderComponent = AddComponent<ModelRenderer>();
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetAssetPath("Engine/Assets/Model/Ground/Ocean.fbx");
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_PatternScale"));
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetLayer(-1); // レイヤーの設定
	}

	AddComponent<SeaLevel>();
}

void SeaLevelObj::SetFilePath(_In_ const FilePath &In_Path) noexcept
{
	if (m_pRenderComponent)
		m_pRenderComponent->SetAssetPath(In_Path);
}

void SeaLevelObj::SetCamera(_In_ GameObject *In_Cam) noexcept
{
	m_pCameraObj = In_Cam;
}

void SeaLevelObj::SetPlayer(_In_ GameObject *In_Player) noexcept
{
	m_pPlayer = In_Player;
}
