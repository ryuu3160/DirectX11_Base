/*+===================================================================
	File: EntryPoint.cpp
	Summary: エントリーポイント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/05/20 Tue PM 12:11:45 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "EntryPoint.hpp"
#include "App/Main.hpp"
#include "System/DebugManager/InitializeImGui.hpp"
#include "System/DebugManager/DebugManager.hpp"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ------------------------------
	// ウィンドウの初期化
	// ------------------------------
	Window &window = Window::GetInstance();													// インスタンス生成
	window.SetClassName(lpClassName);														// ウィンドウクラス名の設定
	window.Create(TITLE_NAME, cx_nWINDOW_WIDTH, cx_nWINDOW_HEIGHT, hInstance, nCmdShow);	// ウィンドウの生成
	window.SetWindowPosCenter();															// ウィンドウを中央に表示

	// ------------------------------
	// FrameManagerの初期化
	// ------------------------------
	FrameManager &frame = FrameManager::GetInstance();	// インスタンス生成
	frame.Init(60);										// フレームレートを60fpsに設定して初期化
#ifdef _DEBUG
	frame._SetHwnd(window.GetHwnd());					// ウィンドウハンドルの設定
	frame._SetWindowTitle(window.GetTitleName());		// ウィンドウタイトルの設定
#endif

	// ------------------------------
	// DirectXなどの各種機能の初期化
	// ------------------------------
	if (FAILED(Main::Init()))
	{
		Main::Uninit();
		// Singletonオブジェクトの解放
		SingletonController::Release();
		// ImGuiの終了処理
		InitializeImGui::UninitImGui();
		return 0;
	}

	// ゲームループ
	while (window.IsLoop())
	{
		// ウィンドウのメッセージループ処理
		if (window.MessageLoop())
		{
			// fps制御
			if (frame.UpdateMain())
			{
				float DeltaTime = frame.GetTick() * frame.GetTimeScale();
				double FixedDeltaTime = frame.GetFixedDeltaTime();
				frame.AddAccumulatedTime(DeltaTime);

				// スパイラル回避
				frame.SetAccumulatedTime(std::min(frame.GetAccumulatedTime(), FixedDeltaTime * frame.GetMaxStepCount()));
				int Steps = 0;
				while (frame.GetAccumulatedTime() >= FixedDeltaTime && Steps < frame.GetMaxStepCount())
				{
					// 物理前処理（力の適用・入力を velocity 等に反映する等）
					//Main::PrePhysics(FixedDeltaTime);

					// 固定刻みで物理更新（衝突検出・解決を含む）
					Main::FixedUpdate(FixedDeltaTime);

					// 衝突イベントをキューに貯める場合はここでキューへ追加
					//Main::EnqueueCollisionEvents();

					frame.SubAccumulatedTime(FixedDeltaTime);
					++Steps;
				}
				Main::Update(DeltaTime);	// 更新処理
				Main::ChangeScene();			// シーン切り替え処理
				Main::Draw();	// 描画処理
			}
		}
	}

	Main::Uninit();	// 各種機能の解放

	// Singletonオブジェクトの解放
	SingletonController::Release();

	// ImGuiの終了処理
	InitializeImGui::UninitImGui();

	return 0;
}