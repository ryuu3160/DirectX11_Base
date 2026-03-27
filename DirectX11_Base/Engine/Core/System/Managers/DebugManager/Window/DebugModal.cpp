/*+===================================================================
	File: DebugModal.cpp
	Summary: モーダルダイアログ
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu PM 04:25:25 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugModal.hpp"

DebugModal::DebugModal(_In_ std::string_view In_Name, _In_ ImGuiWindowFlags In_Flags)
	: DebugWindowBase(In_Name, WindowType::Modal, In_Flags)
	, m_ShouldOpen(false), m_ShouldClose(false)
	, m_ShowDefaultButtons(true), m_IsShowCloseButton(true)
	, m_OkCallback(nullptr)
	, m_CancelCallback(nullptr)
	, m_CloseCallback(nullptr)
{
	m_IsOpen = false; // モーダルは初期状態で閉じておく
}

DebugModal::~DebugModal()
{
}

void DebugModal::Draw() noexcept
{
	// OpenPopupを呼ぶ必要がある
	if(m_ShouldOpen)
	{
		std::string WindowName = GetViewWindowName();
		ImGui::OpenPopup(WindowName.c_str());
		m_ShouldOpen = false;
		m_IsOpen = true;
		m_ShouldClose = false;
		// 開いた時のコールバックを呼ぶ
		if(m_OpenCallback)
			m_OpenCallback();
	}

	if(!m_IsOpen)
		return;

	std::string windowName = GetViewWindowName();

	// BeginPopupModalがtrueの時だけ処理
	if(ImGui::BeginPopupModal(windowName.c_str(), m_IsShowCloseButton ? &m_IsOpen : NULL, m_WindowFlags))
	{
		DrawItems();

		// デフォルトボタンを表示
		if(m_ShowDefaultButtons)
		{
			DrawDefaultButtons();
		}

		ChangeItems();

		if(m_ShouldClose)
		{
			ImGui::CloseCurrentPopup();
			m_IsOpen = false;
		}

		// trueの時だけEndPopupを呼ぶ
		ImGui::EndPopup();
	}

	// モーダルが閉じられた場合
	if(!m_IsOpen)
	{
		if(m_CloseCallback)
			m_CloseCallback();
		m_ShouldClose = false;
	}
}

void DebugModal::Close() noexcept
{
	m_ShouldClose = true;
}

void DebugModal::DrawDefaultButtons() noexcept
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
