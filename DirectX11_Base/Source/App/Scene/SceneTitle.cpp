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

// ==============================
//  定数
// ==============================
namespace
{
	const inline constexpr float cx_ButtonScaleX = 6.0f; // ボタンの初期Xスケール
	const inline constexpr float cx_ButtonScaleY = 1.28f; // ボタンの初期Yスケール
}

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

	// SE追加
	SoundManager::GetInstance().Load("Click", "Assets/Sound/SE/ClickButton.mp3", true, false);

	SpriteManager::GetInstance().CreateScene("Title");
	SpriteManager::GetInstance().ChangeScene(3);

	m_Time = 0.0f;

	// フェードイン
	if (FadeManager::GetInstance().IsFadeEnd("Fade") && FadeManager::GetInstance().GetFadeStatus("Fade") >= 1.0f)
	{
		FadeManager::GetInstance().StartFadeIn("Fade");
	}

	// BGM再生
	SoundManager::GetInstance().Play("TitleBGM");
}

void SceneTitle::Uninit()
{
	SoundManager::GetInstance().Stop("TitleBGM");
}

void SceneTitle::Update()
{
	// ボタンの拡縮
	auto button = SpriteManager::GetInstance().GetSprite("TitleButton");
	if (button && !m_IsChange)
	{
		float x = cx_ButtonScaleX * (std::sinf(m_Time) * 0.5f + 1.0f);
		float y = cx_ButtonScaleY * (std::sinf(m_Time) * 0.5f + 1.0f);
		button->SetScale({ x,y,1.0f });
		m_Time += 1.0f / 60.0f;
		if (m_Time >= 360.0f) m_Time = 0.0f;
	}

	if (Input::IsKeyTrigger(VK_SPACE) && !Input::IsKeyPress(VK_LSHIFT))
	{
		SoundManager::GetInstance().Play("Click");
		FadeManager::GetInstance().StartFadeOut("Fade");
		SpriteManager::GetInstance().GetSprite("TitleButton")->SetScale({ cx_ButtonScaleX,cx_ButtonScaleY,1.0f });
		m_IsChange = true;
	}

	if(FadeManager::GetInstance().IsFadeEnd("Fade") && m_IsChange)
	{
		m_SceneManager.RemoveSubScene<SceneTitle>();
		m_SceneManager.LoadSubScene<SceneGame>();
	}
}

void SceneTitle::Draw()
{
}
