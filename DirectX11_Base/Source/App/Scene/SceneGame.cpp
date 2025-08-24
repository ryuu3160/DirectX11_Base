/*+===================================================================
	File: SceneGame.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 03:57:10 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneGame.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "App/GameObject/Character/Player.hpp"

void SceneGame::Init()
{
	// オブジェクトの作成
	CameraDCC *pCamera = Main::GetScene().GetObject<CameraDCC>("Camera");
	auto pCameraComp = pCamera->GetComponent<Camera>();

	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(pCameraComp);
	SpriteManager::GetInstance().SetCameraObject(pCamera);

	auto player = CreateObject<Player>("Player");
	auto playerModel = player->GetComponent<ModelRenderer>();
	playerModel->SetCamera(pCamera);
}

void SceneGame::Uninit()
{
}

void SceneGame::Update()
{
}

void SceneGame::Draw()
{
}
