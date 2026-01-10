/*+===================================================================
	File: InitializeImGui.cpp
	Summary: ImGuiの初期化クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:08:42 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InitializeImGui.hpp"
#include "Core/DirectX11/DX11_Core.hpp"
#include "ryuu_lib/WindowGenerator/Window.hpp"
#include "Core/DirectX11/ResourceManager/RenderTargetManager.hpp"

void InitializeImGui::InitImGui()
{
	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto &io = ImGui::GetIO();
	auto &Instance = DX11_Core::GetInstance();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Window::GetInstance().GetHwnd());
	ImGui_ImplDX11_Init(Instance.GetDevice(), Instance.GetDeviceContext());

	// フォントの設定
	io.Fonts->AddFontFromFileTTF("Engine\\Assets\\Fonts\\NotoSansJP-Medium.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	// ドッキングの有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // ビューポートの有効化
}

void InitializeImGui::UninitImGui()
{
	// ImGui終了処理
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void InitializeImGui::BeginImGuiFrame() noexcept
{
	// Imguiの描画準備
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void InitializeImGui::EndImGuiFrame() noexcept
{
	// ImGuiの表示
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// マルチビューポートが有効な場合の処理
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// 現在のレンダーターゲットと深度ステンシルビューを取得
		auto &RTVManager = RenderTargetManager::GetInstance();
		auto rtv = RTVManager.GetDefaultRTV();
		auto dsv = RTVManager.GetDefaultDSV();

		// ImGuiのビューポートを更新
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		// レンダーターゲットを復元
		DX11_Core &Instance = DX11_Core::GetInstance();
		Instance.SetRenderTargets(1, &rtv, dsv);
	}
}
