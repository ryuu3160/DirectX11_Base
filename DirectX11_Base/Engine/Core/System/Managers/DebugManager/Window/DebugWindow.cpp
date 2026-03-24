/*+===================================================================
	File: DebugWindow.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:34:14 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugWindow.hpp"

DebugWindow::DebugWindow(_In_ std::string_view In_Name, _In_ ImGuiWindowFlags In_Flags)
	: DebugWindowBase(In_Name, WindowType::Normal, In_Flags)
	, m_IsShowCloseButton(false)
{
}

DebugWindow::~DebugWindow()
{
}

void DebugWindow::Draw() noexcept
{
	if(!m_IsOpen)
		return;

	std::string windowName = GetViewWindowName();

	// ImGui::Beginは返り値に関わらず必ずImGui::Endを呼ぶ必要がある
	bool IsContentVisible = ImGui::Begin(windowName.c_str(), m_IsShowCloseButton ? &m_IsOpen : NULL, m_WindowFlags);

	// コンテンツが見えている場合のみ描画
	if(IsContentVisible)
	{
		DrawItems();
		ChangeItems();
	}

	// 必ずEndを呼ぶ
	ImGui::End();
}
