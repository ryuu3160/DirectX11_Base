/*+===================================================================
	File: SceneTitle.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/17 Wed AM 02:24:55 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneTitle.hpp"

#include "DirectX11/System/RenderManager.hpp"
#include "DirectX11/ResourceManager/ShaderManager.hpp"
#include "DirectX11/Resource/Shaders/ShaderParam.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "DirectX11/Renderer/InstancedModelRenderer.hpp"
#include "DirectX11/Renderer/RTSpriteRenderer.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "System/Component/InputSystem.hpp"
#include "System/Component/Collider/SphereCollider.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

void SceneTitle::Init()
{
	// カメラ取得
	CameraDCC *pCamera = GetObject<CameraDCC>("EditorCamera");
	auto pCameraComp = pCamera->GetComponent<Camera>();

	auto &ShaderM = ShaderManager::GetInstance();

	// 波のオブジェクト作成
	auto WaveObj = CreateObject<GameObject>("WaveObject1");
	auto WaveRenderer = WaveObj->AddComponent<ModelRenderer>();
	WaveRenderer->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
	WaveRenderer->SetVertexShader(ShaderM.GetShader("VS_Object"));
	WaveRenderer->SetPixelShader(ShaderM.GetShader("PS_TexColor"));

	// 戦闘機のインスタンシングオブジェクト作成
	GameObject *pInstanced = CreateObject<GameObject>("F15EGroup");
	pInstanced->SetPosition({ 0.0f,0.1f,0.0f });
	pInstanced->SetScale({ 1.0f,1.0f,1.0f });
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Assets/Model/Character/F15E/F15E.fbx");
	InstancedComp->SetVertexShader(ShaderM.GetShader("IVS_InstancedObject"));
	InstancedComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	InstancedComp->SetScale(0.0005f);
	InstancedComp->IsUseMaterialShader(false);
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 2;
	instanceData.CountZ = 1;
	instanceData.CountY = 1;
	instanceData.StartPos = pInstanced->GetPosition();
	instanceData.Scale = pInstanced->GetScale();
	instanceData.Quaternion = pInstanced->GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 0.8f,0.0f,1.0f };
	instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

	InstancedComp->SetAlignInstanceData(instanceData);
}

void SceneTitle::Uninit()
{
}

void SceneTitle::Update(_In_ float In_Tick)
{
}

void SceneTitle::Draw()
{
}

void SceneTitle::ChangeScene()
{
}
