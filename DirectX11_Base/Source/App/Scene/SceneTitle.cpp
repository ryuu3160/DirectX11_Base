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
	CameraBaseObj *pCamera = GetObject<CameraBaseObj>("EditorCamera");
	auto pCameraComp = pCamera->GetComponent<Camera>();

	// 波のオブジェクト作成
	auto WaveObj = CreateObject<GameObject>("WaveObject1");
	auto WaveRenderer = WaveObj->AddComponent<ModelRenderer>();
	WaveRenderer->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
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
