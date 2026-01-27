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

SeaLevel::SeaLevel(_In_ std::string_view In_Name, _In_ bool In_IsInstance)
	: GameObject(In_Name.data())
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr)
	, m_pPlayer(nullptr)
	, m_IsInstance(In_IsInstance)
	, m_PatternScale({ 1.0f,1.0f })
{
}

SeaLevel::~SeaLevel()
{
	m_pRenderComponent = nullptr;
}

void SeaLevel::Awake() noexcept
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
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_PatternScale"));
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetLayer(-1); // レイヤーの設定
	}
}

void SeaLevel::SetFilePath(_In_ const FilePath &In_Path) noexcept
{
	if (m_pRenderComponent)
		m_pRenderComponent->SetAssetPath(In_Path);
}

void SeaLevel::SetCamera(_In_ GameObject *In_Cam) noexcept
{
	m_pCameraObj = In_Cam;
}

void SeaLevel::SetPlayer(_In_ GameObject *In_Player) noexcept
{
	m_pPlayer = In_Player;
}

void SeaLevel::Update(_In_ float In_DeltaTime) noexcept
{
	if (m_pRenderComponent && !m_IsInstance)
	{
		struct PaternScaleParam
		{
			DirectX::XMFLOAT2 scale;
			DirectX::XMFLOAT2 dummy;
		};
		PaternScaleParam param;
		param.scale = m_PatternScale;
		param.dummy = { 0.0f,0.0f };
		PaternScaleParam params[] = { param };
		std::shared_ptr<ShaderParam> PSParam = std::make_shared<ShaderParam>("PatternScale", 0, params, std::size(params));
		reinterpret_cast<ModelRenderer *>(m_pRenderComponent)->SetWriteParamForPS(std::move(PSParam));
	}
}

void SeaLevel::LateUpdate(_In_ float In_DeltaTime) noexcept
{
	if (m_pPlayer && m_IsInstance)
	{
		auto pos = m_pPlayer->GetPosition();
		pos.y = GetPosition().y;
		SetPosition(pos);
	}
}
