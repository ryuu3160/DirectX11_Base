/*+===================================================================
	File: SceneTitle.cpp
	Summary: タイトルシーン
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/11 Thu AM 02:24:57 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneTitle.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "App/GameObject/MainCamera.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "App/GameObject/Character/Player.hpp"
#include "App/GameObject/Environment/SeaLevel.hpp"
#include "App/Scene/SceneGame.hpp"

void SceneTitle::Init()
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

	SpriteManager::GetInstance().CreateScene("Title");
	SpriteManager::GetInstance().ChangeScene(3);
}

void SceneTitle::Uninit()
{
}

void SceneTitle::Update()
{
	if (Input::IsKeyTrigger(VK_SPACE) && !Input::IsKeyPress(VK_LSHIFT))
	{
		FadeManager::GetInstance().StartFadeOut("Fade");
		m_IsChange = true;
	}

	if(m_IsChange && FadeManager::GetInstance().IsFadeEnd("Fade"))
	{
		m_SceneManager.RemoveSubScene<SceneTitle>();
		m_SceneManager.LoadSubScene<SceneGame>();
	}
}

void SceneTitle::Draw()
{
}
