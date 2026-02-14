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
#include "Engine.hpp"
#include "Core/System/Component/ComponentRegistry.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

// ==============================
//  プロトタイプ宣言
// ==============================
void Update(_In_ float In_DeltaTime);
void FixedUpdate(_In_ double In_FixedDeltaTime);
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
	RegisterAllComponents(); // コンポーネントの登録
	auto &RenderM = RenderManager::GetInstance();
	auto &CollM = CollisionManager::GetInstance();
	auto &Sound = SoundManager::GetInstance();
	CameraManager::GetInstance();
	SceneManager::GetInstance();
	ParticlePoolManager::GetInstance();
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
		"IVS_Particle", // インスタンシング用パーティクル
		"PS_Particle",
	};
	ShaderManager::GetInstance().SetupShaders(shaders);

	// 初期リソース作成
	auto [RTV,DSV] = RenderTargetManager::GetInstance().InitializeDefaultResources(Instance.GetWidth(), Instance.GetHeight());

	if (!RTV || !DSV)
	{
		Error("Failed to create default resources.");
		return E_FAIL;
	}

	DX11_Core::GetInstance().SetRenderTargets(1, &RTV, DSV);

	// Input用のカスタムウィンドウプロシージャを登録
	Instance.AddCustomProc(Input::InputCustomProc);

	// 衝突空間の初期化
	CollM.InitOctreeSpace(DirectX::XMFLOAT3(-10000.0f, 10000.0f, -10000.0f), DirectX::XMFLOAT3(10000.0f, -10000.0f, 10000.0f), 6);

	// シーンの初期化
	auto &SceneM = SceneManager::GetInstance();
	auto pScene = SceneM.Init<SceneRoot>();
	SceneM.SceneObjectsInit();

	// メインのレンダーコンテキスト作成
	Camera *CameraCmp = nullptr;
#ifdef _DEBUG
	CameraCmp = pScene->GetObject<CameraObj>("EditorCamera")->GetComponent<Camera>();
#else
	CameraCmp = pScene->GetObject<CameraObj>("GameCamera")->GetComponent<Camera>();
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
		auto &CollM = CollisionManager::GetInstance();

		float DeltaTime = In_Frame.GetTick() * In_Frame.GetTimeScale();
		double FixedDeltaTime = In_Frame.GetFixedDeltaTime();
		In_Frame.AddAccumulatedTime(DeltaTime);

		// スパイラル回避
		In_Frame.SetAccumulatedTime(std::min(In_Frame.GetAccumulatedTime(), FixedDeltaTime * In_Frame.GetMaxStepCount()));
		int Steps = 0;

		// Inputの更新
		Input::Update();
		// InputSystemの更新
		InputSystem::GetInstance().Update(DeltaTime);

		while (In_Frame.GetAccumulatedTime() >= FixedDeltaTime && Steps < In_Frame.GetMaxStepCount())
		{
			// 固定刻みで物理更新（衝突検出・解決を含む）
			FixedUpdate(FixedDeltaTime);

			// 当たり判定処理
			CollM.CheckAllCollisions();

			// 当たり判定のコールバック呼び出し
			CollM.CallAllCollisionCallbacks();

			// フレーム時間の減算＆ステップ数加算
			In_Frame.SubAccumulatedTime(FixedDeltaTime);
			++Steps;
		}

		Update(DeltaTime);	// 更新処理
		// シーン切り替えの更新
		SceneManager::GetInstance().UpdateSceneChange();
		// Inputの更新終了処理
		Input::EndUpdate();
		Draw();	// 描画処理
	}
}

void FixedUpdate(_In_ double In_FixedDeltaTime)
{
	SceneManager::GetInstance().RootFixedUpdate(In_FixedDeltaTime);
}

void Update(_In_ float In_DeltaTime)
{
	auto &SceneM = SceneManager::GetInstance();
	SceneM.RootUpdate(In_DeltaTime);
	SpriteManager::GetInstance().Update();
	DebugManager::GetInstance().Update();
}

void Draw()
{
	DX11_Core &DX11 = DX11_Core::GetInstance();
	auto &SceneM = SceneManager::GetInstance();

	DX11.BeginDraw();

	SceneM.RootDraw();

	InitializeImGui::BeginImGuiFrame();
	// スプライトマネージャーの描画
	SpriteManager::GetInstance().DrawImGui();
	// デバッグマネージャーの描画
	DebugManager::GetInstance().Draw();
	InitializeImGui::EndImGuiFrame();

	// オブジェクトの破棄とコンポーネントの破棄、コンポーネントの順序変更を非同期で実行
	std::future<void> ChangeOrderFuture = std::async(std::launch::async, &SceneManager::ChangeOrderComponents, &SceneM);
	std::future<void> DestroyCmpFuture = std::async(std::launch::async, &SceneManager::DestroyObjectsComponents, &SceneM);
	std::future<void> DestroyObjFuture = std::async(std::launch::async, &SceneManager::DestroyObjects, &SceneM);

	DX11.Swap();

	ChangeOrderFuture.get(); // コンポーネントの順序変更が終わるまで待機
	DestroyCmpFuture.get(); // コンポーネントの破棄が終わるまで待機
	DestroyObjFuture.get(); // 破棄が終わるまで待機
}
