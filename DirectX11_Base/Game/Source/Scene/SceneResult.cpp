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
#include "SceneTitle.hpp"

SceneResult::SceneResult(_In_ const bool &In_IsClear)
	: SceneBase("Result"), m_IsClear(In_IsClear)
{
}

void SceneResult::Init()
{
	// カメラをメインシーンから取得
#ifdef _DEBUG
	CameraDCC *pCamera = SceneManager::GetInstance().GetCurrentScene()->GetObject<CameraDCC>("Camera");
#else
	MainCamera *pCamera = Main::GetScene().GetObject<MainCamera>("Camera");
#endif
	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	pCamera->SetPosition({ 0.0f,0.0f,-10.0f });
	pCamera->SetRotation({ 0.0f,0.0f,0.0f });

	SpriteManager::GetInstance().CreateScene("Result");
	SpriteManager::GetInstance().ChangeScene(1);

	// 背景画像
	if (m_IsClear)
	{
		auto sprite = SpriteManager::GetInstance().CreateSprite("ResultClear", "Assets/Texture/ResultClear.png", false, false, -1);
		sprite->SetPosition({ 0.0f,0.0f,0.0f });
		sprite->SetScale({ 20.0f,11.25f,1.0f });
		SpriteManager::GetInstance().DeleteSprite("ResultFailed");

		// クリア時のBGM
	}
	else
	{
		auto sprite = SpriteManager::GetInstance().CreateSprite("ResultFailed", "Assets/Texture/ResultFailed.png", false, false, -1);
		sprite->SetPosition({ 0.0f,0.0f,0.0f });
		sprite->SetScale({ 20.0f,11.25f,1.0f });
		SpriteManager::GetInstance().DeleteSprite("ResultClear");
	}


	auto button = SpriteManager::GetInstance().GetSprite("ResultButton");
	m_ButtonScaleX = button->GetScale().x;
	m_ButtonScaleY = button->GetScale().y;
	m_Time = 0.0f;
}

void SceneResult::Uninit()
{
}

void SceneResult::Update(_In_ float In_DeltaTime)
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
		SoundManager::GetInstance().Play("Click");
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
