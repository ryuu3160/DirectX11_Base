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
#include "DirectX11/System/Geometory.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "System/Input/Input.hpp"
#include "System/Scene/SceneRoot.hpp"
#include "System/ImGui/imgui_impl_win32.h"

// ==============================
//  グローバル変数
// ==============================
namespace
{
	std::shared_ptr<SceneRoot> g_pScene;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT Main::Init()
{
	HRESULT hr;
	Window &Instance = Window::GetInstance();

	// DirectXの初期化
	hr = DX11_Initialize::GetInstance().Init();
	if (FAILED(hr))
		return hr;

	// 各種機能の初期化
	auto &Sound = SoundManager::GetInstance();
	Geometory::GetInstance().Init();
	SpriteManager::GetInstance().Init();
	Input::Init();

	// よく使うシェーダーの読み込み
	std::vector<std::string> shaders = {
		"VS_Object",
		"PS_TexColor",	// テクスチャ貼っただけ
		"PS_PatternScale", // パターンスケール
		"IVS_InstancedObject", // インスタンシング用
		"IVS_Bumpmap", // インスタンシング用バンプマップ
		"VS_Bumpmap",
		"PS_POM",
		"VS_Sprite",
		"PS_Sprite",
	};
	ShaderManager::GetInstance().SetupShaders(shaders);

	// Input用のカスタムウィンドウプロシージャを登録
	Instance.AddCustomProc(Input::InputCustomProc);

	// シーンの初期化
	SceneBase::Initialize();
	g_pScene = SceneManager::GetInstance().Init<SceneRoot>();

	// 初期リソース作成
	auto rtv = g_pScene->CreateObject<RenderTarget>("RTV");
	rtv->CreateFromScreen();
	auto dsp = g_pScene->CreateObject<DepthStencil>("DSV");
	hr = dsp->Create(Instance.GetWidth(), Instance.GetHeight(),false);

	DX11_Initialize::GetInstance().SetRenderTargets(1, &rtv, dsp);

#ifdef _DEBUG
	// ImGui専用のウィンドウプロシージャを登録
	Instance.AddCustomProc(ImGui_ImplWin32_WndProcHandler);
#endif

	return hr;
}

void Main::Uninit()
{
	g_pScene = nullptr;
	
	// 各種機能の終了処理
	Input::Uninit();
}

void Main::Update()
{
	auto &SceneM = SceneManager::GetInstance();
	Input::Update();
	SceneM.RootUpdate();
	SpriteManager::GetInstance().Update();

	// シーン切り替えの更新
	SceneM.UpdateSceneChange();

	// Inputの更新終了処理
	Input::EndUpdate();
}

void Main::Draw()
{
	DX11_Initialize &DX11 = DX11_Initialize::GetInstance();
	auto rtv = g_pScene->GetObject<RenderTarget>("RTV");
	auto dsv = g_pScene->GetObject<DepthStencil>("DSV");
	float color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

	DX11.GetDeviceContext()->ClearRenderTargetView(rtv->GetView(), color);
	DX11.GetDeviceContext()->ClearDepthStencilView(dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	SceneManager::GetInstance().RootDraw();

	// スプライトマネージャーの3D描画
	SpriteManager::GetInstance().Draw3D();

	Change2D_Draw(); // 2D描画の設定
	SpriteManager::GetInstance().Draw2D();
	Change3D_Draw(); // 3D描画の設定

	SpriteManager::GetInstance().DrawImGui();

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

SceneBase &Main::GetScene() noexcept
{
	return *g_pScene;
}

RenderTarget *Main::GetRenderTarget() noexcept
{
	return g_pScene->GetObject<RenderTarget>("RTV");
}

DepthStencil *Main::GetDepthStencil() noexcept
{
	return g_pScene->GetObject<DepthStencil>("DSV");
}
