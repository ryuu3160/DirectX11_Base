/*+===================================================================
	File: DebugPopup.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu PM 04:29:34 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugPopup.hpp"

DebugPopup::DebugPopup(_In_ std::string_view In_Name, _In_ ImGuiWindowFlags In_Flags)
	: DebugWindowBase(In_Name, WindowType::Popup, In_Flags)
	, m_ShouldOpen(false), m_ShowDefaultButtons(true)
	, m_OkCallback(nullptr), m_CancelCallback(nullptr), m_OpenCallback(nullptr), m_CloseCallback(nullptr)
{
}

DebugPopup::~DebugPopup()
{
}

void DebugPopup::Draw() noexcept
{
	if(m_ShouldOpen)
	{
		std::string windowName = GetViewWindowName();
		ImGui::OpenPopup(windowName.c_str());
		m_ShouldOpen = false;
		m_IsOpen = true;
		// 開いた時のコールバックを呼ぶ
		if(m_OpenCallback)
			m_OpenCallback();
	}

	std::string windowName = GetViewWindowName();

	// BeginPopupがtrueの時だけ処理
	if(ImGui::BeginPopup(windowName.c_str(), m_WindowFlags))
	{
		DrawItems();

		// デフォルトボタンを表示
		if(m_ShowDefaultButtons)
		{
			DrawDefaultButtons();
		}

		ChangeItems();

		// trueの時だけEndPopupを呼ぶ
		ImGui::EndPopup();
	}
}

void DebugPopup::Close() noexcept
{
	ImGui::CloseCurrentPopup();

	m_IsOpen = false;

	if(m_CloseCallback)
		m_CloseCallback();
}

void DebugPopup::DrawDefaultButtons() noexcept
{
	ImGui::Separator();
	ImGui::Spacing();

	float buttonWidth = 120.0f;
	float spacing = ImGui::GetStyle().ItemSpacing.x;
	float totalWidth = buttonWidth * 2 + spacing;
	float availWidth = ImGui::GetContentRegionAvail().x;
	float offset = (availWidth - totalWidth) * 0.5f;

	if(offset > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

	if(ImGui::Button("OK", ImVec2(buttonWidth, 0)))
	{
		if(m_OkCallback)
			m_OkCallback();
		Close();
	}

	ImGui::SameLine();

	if(ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
	{
		if(m_CancelCallback)
			m_CancelCallback();
		Close();
	}
}
