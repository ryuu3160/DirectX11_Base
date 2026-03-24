/*+===================================================================
	File: DebugWindow.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:34:04 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DebugWindowBase.hpp"

/// <summary>
/// DebugWindowクラス
/// </summary>
class DebugWindow : public DebugWindowBase
{
	friend class DebugManager;
public:
	DebugWindow(_In_ std::string_view In_Name, _In_ ImGuiWindowFlags In_Flags = 0);
	~DebugWindow();

	/// <summary>
	/// 閉じるボタンの表示を設定
	/// </summary>
	/// <param name="[In_IsShow]">閉じるボタンを表示するかどうか</param>
	void SetShowCloseButton(_In_ bool In_IsShow) noexcept { m_IsShowCloseButton = In_IsShow; }

	void Draw() noexcept override;

private:
	bool m_IsShowCloseButton;
};
