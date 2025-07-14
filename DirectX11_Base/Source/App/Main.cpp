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
#include "DirectX11/System/SpriteManager.hpp"
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
	Window &Instance = Window::GetInstance();

	// DirectXの初期化
	hr = DX11_Initialize::GetInstance().Init();
	if (FAILED(hr))
		return hr;

	// 各種機能の初期化
	Geometory::GetInstance().Init();
	SpriteManager::GetInstance().Init();
	Input::Init();

	// シーンの初期化
	SceneBase::Initialize();
	g_pScene = std::make_shared<SceneRoot>();
	g_pScene->Init();

	// 初期リソース作成
	auto rtv = g_pScene->CreateObject<RenderTarget>("RTV");
	rtv->CreateFromScreen();
	auto dsp = g_pScene->CreateObject<DepthStencil>("DSV");
	hr = dsp->Create(Instance.GetWidth(), Instance.GetHeight(),false);

	DX11_Initialize::GetInstance().SetRenderTargets(1, &rtv, dsp);

	return hr;
}

void Main::Uninit()
{
	g_pScene->Uninit();
	g_pScene.reset();
	
	// 各種機能の終了処理
	Input::Uninit();
	Geometory::GetInstance().Uninit();
	DX11_Initialize::GetInstance().Uninit();
}

void Main::Update()
{
	Input::Update();
	g_pScene->RootUpdate();
}

void Main::Draw()
{
	DX11_Initialize &DX11 = DX11_Initialize::GetInstance();
	auto rtv = g_pScene->GetObject<RenderTarget>("RTV");
	auto dsv = g_pScene->GetObject<DepthStencil>("DSV");
	float color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

	DX11.GetDeviceContext()->ClearRenderTargetView(rtv->GetView(), color);
	DX11.GetDeviceContext()->ClearDepthStencilView(dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	g_pScene->RootDraw();

	DX11.Swap();
}

void Main::Change2D_Draw() noexcept
{
	// 2D描画の設定
	DX11_Initialize &Instance = DX11_Initialize::GetInstance();
	auto pRTV = Main::GetScene().GetObject<RenderTarget>("RTV");
	Instance.SetRenderTargets(1, &pRTV, nullptr);
}

void Main::Change3D_Draw() noexcept
{
	// 3D描画の設定
	DX11_Initialize &Instance = DX11_Initialize::GetInstance();
	auto pRTV = Main::GetScene().GetObject<RenderTarget>("RTV");
	auto pDSV = Main::GetScene().GetObject<DepthStencil>("DSV");
	Instance.SetRenderTargets(1, &pRTV, pDSV);
}

SceneRoot &Main::GetScene() noexcept
{
	return *g_pScene;
}
