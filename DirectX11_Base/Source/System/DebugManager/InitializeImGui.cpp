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
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\NotoSansJP-Medium.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
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
