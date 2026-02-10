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
#include "Game/Object/Character/PlayerObj.hpp"
#include "Game/Object/Character/EnemyObj.hpp"
#include "Game/Object/Environment/SeaLevelObj.hpp"
#include "Game/Script/Character/Player.hpp"

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
	CameraObj *pCamera = SceneManager::GetInstance().GetCurrentScene()->GetObject<CameraObj>("EditorCamera");
#else
	CameraObj *pCamera = SceneManager::GetInstance().GetCurrentScene()->GetObject<CameraObj>("GameCamera");
#endif

	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	// BGMの再生
	SoundManager::GetInstance().Load("GameBGM", "Assets/Sound/BGM/BattleBGM.wav", true, true);
	SoundManager::GetInstance().Play("GameBGM");

	auto player = CreateObject<PlayerObj>("Player");

	// 敵の生成
	std::string name = "Enemy1";
	//auto enemy1 = CreateObject<EnemyObj>(name);

	auto pos = player->GetPosition();
	pos.z += 100.0f;
	//enemy1->SetPosition(pos);

	// 海面オブジェクトの生成
	SeaLevelObj *pSeaLevel = CreateObject<SeaLevelObj>("SeaLevel",nullptr,false);
	pSeaLevel->SetCamera(pCamera);
	pSeaLevel->SetPlayer(player);
	pSeaLevel->SetScale({ 100000.0f,1.0f,100000.0f });
	pSeaLevel->SetPatternScale({ 5000.0f,5000.0f });
	pSeaLevel->SetPosition({ 0.0f,0.1f,0.0f });

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

	if(!GetObject<EnemyObj>("Enemy1"))
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
	auto player = GetObject<PlayerObj>("Player");
	auto PlayerCmp = player->GetComponent<Player>();
	bool IsPlayerDead = PlayerCmp->IsDestroyed();
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
			auto cam = GetObject<CameraObj>("GameCamera");
#else
			auto cam = GetObject<CameraObj>("GameCamera");
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
