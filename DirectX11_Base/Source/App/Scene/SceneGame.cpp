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
#include "DirectX11/System/Geometory.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "App/GameObject/MainCamera.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "App/GameObject/Character/Player.hpp"
#include "App/GameObject/Character/Enemy.hpp"
#include "App/GameObject/Environment/SeaLevel.hpp"
#include "App/Scene/SceneResult.hpp"

// ===============================
//  定数
// ===============================
namespace
{
	const inline constexpr float TIME_LIMIT = 60.0f; // 制限時間（秒）
}

SceneGame::SceneGame()
	: SceneBase("Game"), m_FrameManager(FrameManager::GetInstance())
	, m_ChangeResultTIme(0.0f)
{
}

void SceneGame::Init()
{
	// カメラをメインシーンから取得
#ifdef _DEBUG
	CameraDCC *pCamera = Main::GetScene().GetObject<CameraDCC>("Camera");
#else
	MainCamera *pCamera = Main::GetScene().GetObject<MainCamera>("Camera");
#endif

	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	// BGMの再生
	SoundManager::GetInstance().Load("GameBGM", "Assets/Sound/BGM/BattleBGM.wav", true, true);
	SoundManager::GetInstance().Play("GameBGM");

	auto player = CreateObject<Player>("Player");
	player->SetCamera(pCamera);

	// カメラにプレイヤーを設定
	pCamera->SetTargetPlayer(player);

	// 敵の生成
	std::string name = "Enemy1";
	auto enemy1 = CreateObject<Enemy>(name,name);
	enemy1->SetCamera(pCamera);

	auto pos = player->GetPos();
	pos.z += 100.0f;
	enemy1->SetPos(pos);

	// ターゲットをプレイヤーに設定
	player->SetTarget(enemy1);

	// 海面オブジェクトの生成
	SeaLevel *pSeaLevel = CreateObject<SeaLevel>("SeaLevel",false);
	pSeaLevel->SetCamera(pCamera);
	pSeaLevel->SetPlayer(player);
	pSeaLevel->SetScale({ 100000.0f,1.0f,100000.0f });
	pSeaLevel->SetPatternScale({ 5000.0f,5000.0f });
	pSeaLevel->SetPos({ 0.0f,0.1f,0.0f });

	// スカイドームを作成
	SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	pSkyBox->SetCamera(pCamera);

	// 制限時間のタイマー設定
	m_FrameManager.AppendTimeCounter("GameTimer",true);
	m_FrameManager.StartTimeCounter("GameTimer");

	SpriteManager::GetInstance().CreateScene("Game");
	SpriteManager::GetInstance().ChangeScene(0);
}

void SceneGame::Uninit()
{
	m_FrameManager.StopTimeCounter("GameTimer");
	m_FrameManager.ResetTimeCounter("GameTimer");
}

void SceneGame::Update()
{
	// フェードイン
	if (FadeManager::GetInstance().IsFadeEnd("Fade") && FadeManager::GetInstance().GetFadeStatus("Fade") >= 1.0f && !m_ChangeScene)
	{
		FadeManager::GetInstance().StartFadeIn("Fade");
		return;
	}

	m_FrameManager.UpdateTimeCounter("GameTimer");

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
	float time = m_FrameManager.GetTimeCountSecond("GameTimer");
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
			cam->SetTargetPlayer(nullptr);
			auto pos = player->GetPos();
			auto playerFront = player->GetFront();
			auto playerUp = player->GetUp();
			pos = (pos - playerFront * (cx_ThirdPerson_Distance * 2.0f)) + playerUp * (cx_ThirdPerson_UpDistanceRate * 2.0f);
			cam->SetPos(pos);
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
