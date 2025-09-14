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
	SpriteManager::GetInstance().ChangeScene(1);

	auto button = SpriteManager::GetInstance().GetSprite("ResultButton");
	m_ButtonScaleX = button->GetScale().x;
	m_ButtonScaleY = button->GetScale().y;
	m_Time = 0.0f;
}

void SceneResult::Uninit()
{
}

void SceneResult::Update()
{
	// ボタンの拡縮
	auto button = SpriteManager::GetInstance().GetSprite("ResultButton");
	if (button && !m_IsChange)
	{
		float x = m_ButtonScaleX * (std::sinf(m_Time) * 0.5f + 1.0f);
		float y = m_ButtonScaleY * (std::sinf(m_Time) * 0.5f + 1.0f);
		button->SetScale({ x,y,1.0f });
		m_Time += 1.0f / 60.0f;
		if (m_Time >= 360.0f) m_Time = 0.0f;
	}

	// フェードイン
	if (FadeManager::GetInstance().IsFadeEnd("Fade") && FadeManager::GetInstance().GetFadeStatus("Fade") >= 1.0f && !m_IsChange)
	{
		FadeManager::GetInstance().StartFadeIn("Fade");
		return;
	}

	// スペースキーでタイトルへ(フェードアウト)
	if (Input::IsKeyTrigger(VK_SPACE) && !Input::IsKeyPress(VK_LSHIFT))
	{
		FadeManager::GetInstance().StartFadeOut("Fade");
		SpriteManager::GetInstance().GetSprite("ResultButton")->SetScale({ m_ButtonScaleX,m_ButtonScaleY,1.0f });
		m_IsChange = true;
	}

	if (FadeManager::GetInstance().IsFadeEnd("Fade") && m_IsChange)
	{
		m_SceneManager.RemoveSubScene<SceneResult>();
		m_SceneManager.LoadSubSceneAsync<SceneTitle>();
	}
}

void SceneResult::Draw()
{
}
