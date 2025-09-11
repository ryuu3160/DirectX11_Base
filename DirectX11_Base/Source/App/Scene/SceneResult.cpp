/*+===================================================================
	File: SceneResult.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/12 Fri AM 07:12:24 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneResult.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "App/GameObject/MainCamera.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "App/GameObject/Character/Player.hpp"
#include "App/GameObject/Environment/SeaLevel.hpp"
#include "App/Scene/SceneTitle.hpp"


void SceneResult::Init()
{
	// カメラをメインシーンから取得
#ifdef _DEBUG
	CameraDCC *pCamera = Main::GetScene().GetObject<CameraDCC>("Camera");
#else
	MainCamera *pCamera = Main::GetScene().GetObject<MainCamera>("Camera");
#endif
	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	pCamera->SetPos({ 0.0f,0.0f,-10.0f });
	pCamera->SetRotation({ 0.0f,0.0f,0.0f });

	SpriteManager::GetInstance().CreateScene("Result");
}

void SceneResult::Uninit()
{
}

void SceneResult::Update()
{
	if (Input::IsKeyTrigger(VK_SPACE) && !Input::IsKeyPress(VK_LSHIFT))
	{
		m_SceneManager.RemoveSubScene<SceneResult>();
		m_SceneManager.LoadSubSceneAsync<SceneTitle>();
	}
}

void SceneResult::Draw()
{
}
