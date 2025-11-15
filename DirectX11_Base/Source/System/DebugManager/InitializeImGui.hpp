/*+===================================================================
	File: InitializeImGui.hpp
	Summary: ImGuiの初期化クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:08:49 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/ImGui/imgui.h"
#include "System/ImGui/imgui_impl_dx11.h"
#include "System/ImGui/imgui_impl_win32.h"

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// InitializeImGuiクラス
/// </summary>
class InitializeImGui
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	InitializeImGui() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InitializeImGui() = default;

	static void InitImGui();

	static void UninitImGui();
};
