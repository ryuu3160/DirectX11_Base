/*+===================================================================
	File: Main.cpp
	Summary: ゲームのメイン処理
	Author: AT13C192 01 青木雄一郎
	Date: 2025/5/22 Thu PM 06:14:25 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Main.hpp"
#include "DirectX11/DX11_Initialize.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "DirectX11/System/Sprite.hpp"
#include "System/Input/Input.hpp"
#include "System/Scene/SceneRoot.hpp"

// ==============================
//  グローバル変数
// ==============================
namespace
{
	std::shared_ptr<SceneRoot> g_pScene;
}

HRESULT Main::Init()
{
	HRESULT hr;

	// DirectXの初期化
	hr = DX11_Initialize::GetInstance().Init();
	if (FAILED(hr))
		return hr;

	// 各種機能の初期化
	Geometory::GetInstance().Init();
	Sprite::GetInstance().Init();
	Input::Init();

	// シーンの初期化
	SceneBase::Initialize();
	g_pScene = std::make_shared<SceneRoot>();
	g_pScene->Init();

	DX11_Initialize::GetInstance().SetRenderTargets(1,)

	return hr;
}

void Main::Uninit()
{
	
	// 各種機能の終了処理
	Input::Uninit();
	Sprite::GetInstance().Uninit();
	Geometory::GetInstance().Uninit();
	DX11_Initialize::GetInstance().Uninit();
}

void Main::Update()
{
	Input::Update();

}

void Main::Draw()
{


	DX11_Initialize::GetInstance().Swap();
}
