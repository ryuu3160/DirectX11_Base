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

	auto player = CreateObject<Player>("Player");
	player->SetCamera(pCamera);

	// カメラにプレイヤーを設定
	pCamera->SetTargetPlayer(player);

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

	// Missileテスト
	auto missile = CreateObject<Missile>("MissileTest","MissileTest");
	auto MissileModel = missile->GetComponent<ModelRenderer>();
	MissileModel->SetCamera(pCamera);
	missile->SetPos({ -2.0f,1.0f,0.0f });
	missile->SetScale({ 0.007f,0.007f,0.007f });
	player->SetPos({ 0.0f,1.0f,0.0f });
	player->SetQuat({ 0.0f,0.0f,0.0f,1.0f });

	// 制限時間のタイマー設定
	m_FrameManager.AppendTimeCounter("GameTimer",true);

	SpriteManager::GetInstance().CreateScene("Game");
	SpriteManager::GetInstance().ChangeScene(1);
}

void SceneGame::Uninit()
{
	m_FrameManager.StopTimeCounter("GameTimer");
	m_FrameManager.ResetTimeCounter("GameTimer");
}

void SceneGame::Update()
{
	m_FrameManager.UpdateTimeCounter("GameTimer");

	if(m_FrameManager.GetTimeCountSecond("GameTimer") >= TIME_LIMIT)
	{
		SceneManager::GetInstance().RemoveSubScene<SceneGame>();
		SceneManager::GetInstance().LoadSubSceneAsync<SceneResult>();
	}
}

void SceneGame::Draw()
{
}
