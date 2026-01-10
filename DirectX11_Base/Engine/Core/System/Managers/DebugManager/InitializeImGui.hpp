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
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Core/Defines.hpp"

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

	static void BeginImGuiFrame() noexcept;

	static void EndImGuiFrame() noexcept;
};
