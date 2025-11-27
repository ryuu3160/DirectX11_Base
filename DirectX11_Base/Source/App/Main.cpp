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
#include "DirectX11/System/RenderManager.hpp"
#include "System/DebugManager/InitializeImGui.hpp"
#include "System/DebugManager/DebugManager.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/ResourceManager/ShaderManager.hpp"
#include "System/Input/Input.hpp"
#include "System/Scene/SceneRoot.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/ImGui/imgui_impl_win32.h"

// ==============================
//  グローバル変数
// ==============================

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT Main::Init()
{
	HRESULT hr;
	Window &Instance = Window::GetInstance();

	// DirectXの初期化
	hr = DX11_Core::GetInstance().Init();
	if (FAILED(hr))
		return hr;

	// 各種機能の初期化
	auto &RenderM = RenderManager::GetInstance();
	auto &Sound = SoundManager::GetInstance();
	Geometory::GetInstance().Init();
	SceneManager::GetInstance();
	FadeManager::GetInstance();
	Input::Init();

	// デバッグ関連の初期化
	InitializeImGui::InitImGui();
	auto &SpriteM = SpriteManager::GetInstance();
	auto &DebugM = DebugManager::GetInstance();
	// DebugManagerの初期化
	DebugM.Init();
	// 通常のデバッグウィンドウを追加
	InitializeDebugWindows();

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
	auto pScene = SceneManager::GetInstance().Init<SceneRoot>();

	// 初期リソース作成
	auto [RTV,DSV] = RenderTargetManager::GetInstance().InitializeDefaultResources(Instance.GetWidth(), Instance.GetHeight());

	if (!RTV || !DSV)
	{
		Error("Failed to create default resources.");
		return E_FAIL;
	}

	DX11_Core::GetInstance().SetRenderTargets(1, &RTV, DSV);

	// メインのレンダーコンテキスト作成
	Camera *CameraCmp = nullptr;
#ifdef _DEBUG
	CameraCmp = pScene->GetObject<CameraDCC>("EditorCamera")->GetComponent<Camera>();
#else

#endif
	if(CameraCmp == nullptr)
	{
		Error("MainCamera is not found.");
		return E_FAIL;
	}
	RenderM.CreateMainRenderContext(CameraCmp, RTV, DSV);

	// SpriteManagerの初期化
	SpriteM.Init();

#ifdef _DEBUG
	// ImGui専用のウィンドウプロシージャを登録
	Instance.AddCustomProc(ImGui_ImplWin32_WndProcHandler);
#endif

	return hr;
}

void Main::Uninit()
{
	// 各種機能の終了処理
	Input::Uninit();
}

void Main::Update(_In_ float In_Tick)
{
	auto &SceneM = SceneManager::GetInstance();
	Input::Update();
	SceneM.RootUpdate(In_Tick);
	SpriteManager::GetInstance().Update();
	DebugManager::GetInstance().Update();
}

void Main::FixedUpdate(_In_ double In_FixedTick)
{
}

void Main::ChangeScene()
{
	// シーン切り替えの更新
	SceneManager::GetInstance().UpdateSceneChange();

	// Inputの更新終了処理
	Input::EndUpdate();
}

void Main::Draw()
{
	auto &SceneM = SceneManager::GetInstance();
	DX11_Core &DX11 = DX11_Core::GetInstance();
	auto &RTVManager = RenderTargetManager::GetInstance();
	auto rtv = RTVManager.GetDefaultRTV();
	auto dsv = RTVManager.GetDefaultDSV();
	float color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

	DX11.GetDeviceContext()->ClearRenderTargetView(rtv->GetView(), color);
	DX11.GetDeviceContext()->ClearDepthStencilView(dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	SceneM.RootDraw();

	InitializeImGui::BeginImGuiFrame();
	// スプライトマネージャーの描画
	SpriteManager::GetInstance().DrawImGui();
	// デバッグマネージャーの描画
	DebugManager::GetInstance().Draw();
	InitializeImGui::EndImGuiFrame();

	// オブジェクトの破棄は非同期で行う
	std::future<void> fut = std::async(std::launch::async, &SceneManager::DestroyObjects, &SceneM);

	DX11.Swap();

	fut.get(); // 破棄が終わるまで待機
}

void Main::InitializeDebugWindows() noexcept
{
	auto &DebugM = DebugManager::GetInstance();
	// フレームレート表示ウィンドウ
	auto log = DebugM.CreateDebugWindow("System", "Log");
	DebugM.CreateDebugWindow("System", "Hierarchy");
	DebugM.CreateDebugWindow("System", "Inspector");
	DebugM.AddToolBarMenu("System", "Reset ImGui Layout", []()
		{
			ImGui::LoadIniSettingsFromDisk("Assets/Debug/imgui_layout.ini");
		});

	DebugM.AddToolBarMenu("Camera", "Editor", []()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraDCC>("EditorCamera");
				if(!CamObj)
					return;
				auto camera = CamObj->GetComponent<Camera>();
				auto context = RenderManager::GetInstance().GetRenderContext("Main");
				if (camera && context)
				{
					context->GetCamera()->SetActive(false);
					context->SwapCamera(camera);
					CamObj->SetActive(true);
				}
			}
		});
	DebugM.AddToolBarMenu("Camera", "Game", []()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraBaseObj>("GameCamera");
				if (!CamObj)
					return;
				auto camera = CamObj->GetComponent<Camera>();
				auto context = RenderManager::GetInstance().GetRenderContext("Main");
				if (camera && context)
				{
					context->GetCamera()->GetGameObject()->SetActive(false);
					context->SwapCamera(camera);
					CamObj->SetActive(true);
				}
			}
		});

	log->CreateItem<ItemText>("LogText", true, ImGuiInputTextFlags_ReadOnly,true, true);
}
