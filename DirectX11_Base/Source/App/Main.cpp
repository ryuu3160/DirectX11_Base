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
//  プロトタイプ宣言
// ==============================
void Update(_In_ float In_Tick);
void FixedUpdate(_In_ double In_FixedTick);
void Draw();

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
	auto &SceneM = SceneManager::GetInstance();
	auto pScene = SceneM.Init<SceneRoot>();
	SceneM.SceneObjectsInit();

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

void Main::GameLoop(_In_ FrameManager &In_Frame)
{
	// fps制御
	if (In_Frame.UpdateMain())
	{
		float DeltaTime = In_Frame.GetTick() * In_Frame.GetTimeScale();
		double FixedDeltaTime = In_Frame.GetFixedDeltaTime();
		In_Frame.AddAccumulatedTime(DeltaTime);

		// スパイラル回避
		In_Frame.SetAccumulatedTime(std::min(In_Frame.GetAccumulatedTime(), FixedDeltaTime * In_Frame.GetMaxStepCount()));
		int Steps = 0;

		// Inputの更新
		Input::Update();

		while (In_Frame.GetAccumulatedTime() >= FixedDeltaTime && Steps < In_Frame.GetMaxStepCount())
		{
			// 物理前処理（力の適用・入力を velocity 等に反映する等）
			//PrePhysics(FixedDeltaTime);

			// 固定刻みで物理更新（衝突検出・解決を含む）
			FixedUpdate(FixedDeltaTime);

			// 衝突イベントをキューに貯める場合はここでキューへ追加
			//EnqueueCollisionEvents();

			In_Frame.SubAccumulatedTime(FixedDeltaTime);
			++Steps;
		}

		DebugManager::GetInstance().DebugLogWarning("DeltaTime: {:.6f}", DeltaTime);
		Update(DeltaTime);	// 更新処理
		// シーン切り替えの更新
		SceneManager::GetInstance().UpdateSceneChange();
		// Inputの更新終了処理
		Input::EndUpdate();
		Draw();	// 描画処理
	}
}

void FixedUpdate(_In_ double In_FixedTick)
{
	SceneManager::GetInstance().RootFixedUpdate(In_FixedTick);
}

void Update(_In_ float In_Tick)
{
	auto &SceneM = SceneManager::GetInstance();
	SceneM.RootUpdate(In_Tick);
	SpriteManager::GetInstance().Update();
	DebugManager::GetInstance().Update();
}

void Draw()
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
