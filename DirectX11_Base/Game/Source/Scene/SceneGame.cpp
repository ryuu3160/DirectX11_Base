/*+===================================================================
	File: SceneGame.cpp
	Summary: ゲームシーン
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 03:57:10 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneGame.hpp"
#include "SceneResult.hpp"
#include "Game/Source/Object/Character/Player.hpp"
#include "Game/Source/Object/Character/Enemy.hpp"
#include "Game/Source/Object/Environment/SeaLevel.hpp"

// ===============================
//  定数
// ===============================
namespace
{
	const inline constexpr float TIME_LIMIT = 60.0f; // 制限時間(秒)
}

SceneGame::SceneGame()
	: SceneBase("Game"), m_Timer(nullptr)
	, m_ChangeResultTIme(0.0f)
{
	m_Timer = new Timer();
}

SceneGame::~SceneGame()
{
	delete m_Timer;
	m_Timer = nullptr;
}

void SceneGame::Init()
{
	// カメラをメインシーンから取得
#ifdef _DEBUG
	CameraDCC *pCamera = SceneManager::GetInstance().GetCurrentScene()->GetObject<CameraDCC>("EditorCamera");
#else
	MainCamera *pCamera = SceneManager::GetInstance().GetCurrentScene()->GetObject<MainCamera>("Camera");
#endif

	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	// BGMの再生
	SoundManager::GetInstance().Load("GameBGM", "Assets/Sound/BGM/BattleBGM.wav", true, true);
	SoundManager::GetInstance().Play("GameBGM");

	auto player = CreateObject<Player>("Player");
	player->SetCamera(pCamera);

	// 敵の生成
	std::string name = "Enemy1";
	auto enemy1 = CreateObject<Enemy>(name);
	enemy1->SetCamera(pCamera);

	auto pos = player->GetPosition();
	pos.z += 100.0f;
	enemy1->SetPosition(pos);

	// ターゲットをプレイヤーに設定
	player->SetTarget(enemy1);

	// 海面オブジェクトの生成
	SeaLevel *pSeaLevel = CreateObject<SeaLevel>("SeaLevel",nullptr,false);
	pSeaLevel->SetCamera(pCamera);
	pSeaLevel->SetPlayer(player);
	pSeaLevel->SetScale({ 100000.0f,1.0f,100000.0f });
	pSeaLevel->SetPatternScale({ 5000.0f,5000.0f });
	pSeaLevel->SetPosition({ 0.0f,0.1f,0.0f });

	// スカイドームを作成
	SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	pSkyBox->SetCamera(pCamera);

	// 制限時間のタイマー設定
	m_Timer->AppendTimeCounter("GameTimer",true);
	m_Timer->StartTimeCounter("GameTimer");

	SpriteManager::GetInstance().CreateScene("Game");
	SpriteManager::GetInstance().ChangeScene(0);
}

void SceneGame::Uninit()
{
	m_Timer->StopTimeCounter("GameTimer");
	m_Timer->ResetTimeCounter("GameTimer");
}

void SceneGame::Update(_In_ float In_DeltaTime)
{
	// フェードイン
	if (FadeManager::GetInstance().IsFadeEnd("Fade") && FadeManager::GetInstance().GetFadeStatus("Fade") >= 1.0f && !m_ChangeScene)
	{
		FadeManager::GetInstance().StartFadeIn("Fade");
		return;
	}

	m_Timer->UpdateTimeCounter("GameTimer");

	if(!GetObject<Enemy>("Enemy1"))
	{
		// 敵が破壊されたらリザルトへ
		if (!m_ChangeScene)
		{
			FadeManager::GetInstance().StartFadeOut("Fade");
			m_IsClear = true;
			m_ChangeScene = true;

			// クリア時のBGM
			SoundManager::GetInstance().Stop("GameBGM");
			SoundManager::GetInstance().Load("ClearSE", "Assets/Sound/SE/StageClear_Jingle01.mp3", true, false);
			SoundManager::GetInstance().Play("ClearSE");
		}
	}

	// 制限時間を超えた、又はプレイヤーが破壊されたらシーンチェンジ
	float time = m_Timer->GetTimeCountSecond("GameTimer");
	auto player = GetObject<Player>("Player");
	bool IsPlayerDead = player->IsDestroyed();
	if ((time >= TIME_LIMIT || IsPlayerDead) && !m_ChangeScene)
	{
		if (m_ChangeResultTIme <= 0.0f)
		{
			// 失敗時のBGM
			SoundManager::GetInstance().Stop("GameBGM");
			SoundManager::GetInstance().Load("FailedSE", "Assets/Sound/SE/gameover3.mp3", true, false);
			SoundManager::GetInstance().Play("FailedSE");
		}

		// プレイヤーが死んでいた場合とそうでない場合でリザルトへ行くまでの時間を変える
		if (IsPlayerDead && m_ChangeResultTIme <= 0.0f)
		{
#ifdef _DEBUG
			auto cam = GetObject<CameraDCC>("Camera");
#else
			auto cam = GetObject<MainCamera>("Camera");
#endif
			auto pos = player->GetPosition();
			auto playerFront = player->GetFront();
			auto playerUp = player->GetUp();
			pos = (pos - playerFront * (10.0f * 2.0f)) + playerUp * (1.0f * 2.0f);
			cam->SetPosition(pos);
			cam->SetQuat(player->GetQuat());
			m_ChangeResultTIme = 3.0f; // 3秒後にリザルトへ
		}

		// タイマーを進める
		m_ChangeResultTIme -= 1.0f / 60.0f;

		if (m_ChangeResultTIme <= 0.0f)
		{
			FadeManager::GetInstance().StartFadeOut("Fade");
			m_IsClear = (time < TIME_LIMIT && !IsPlayerDead);
			m_ChangeScene = true;
		}
	}

	// シーンチェンジ
	if (FadeManager::GetInstance().IsFadeEnd("Fade") && m_ChangeScene)
	{
		SceneManager::GetInstance().RemoveSubScene<SceneGame>();
		SceneManager::GetInstance().LoadSubSceneAsync<SceneResult>(m_IsClear);
	}
}

void SceneGame::Draw()
{
}

void SceneGame::ChangeScene()
{
}
