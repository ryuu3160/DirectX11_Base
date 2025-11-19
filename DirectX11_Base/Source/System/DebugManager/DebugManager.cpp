/*+===================================================================
	File: DebugManager.cpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:25 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	constexpr float cx_fTitleBarHeight = 80.0f;
	constexpr float cx_fToolBarHeight = 15.0f;
	static DebugWindow *c_NullWindow;
}

DebugManager::DebugManager()
	: m_ToolBarFlags(0)
{
	c_NullWindow = new DebugWindow("NullWindow");
	c_NullWindow->m_IsDummy = true;
}

DebugManager::~DebugManager()
{
	for (auto &itr : m_DebugWindows)
	{
		for (const auto &window : itr.second)
		{
			delete window;
		}
		itr.second.clear();
	}
	m_DebugWindows.clear();
}

void DebugManager::Init()
{
	m_ToolBarFlags |= ImGuiWindowFlags_MenuBar;
	m_ToolBarFlags |= ImGuiWindowFlags_NoCollapse;
	m_ToolBarFlags |= ImGuiWindowFlags_NoMove;
	m_ToolBarFlags |= ImGuiWindowFlags_NoResize;

	// データの読み込み
	std::fstream file("Assets\\Debug\\DebugManagerData.csv", std::ios::in);

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{

		}
		file.close();
	}
}

void DebugManager::Update() noexcept
{
}

void DebugManager::Draw() noexcept
{
	ImGuiViewport *vp = ImGui::GetMainViewport();
	ImVec2 pos = ImVec2(vp->WorkPos.x, vp->WorkPos.y - cx_fTitleBarHeight);
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(cx_nWINDOW_WIDTH, cx_fToolBarHeight), ImGuiCond_Always);
	if (ImGui::Begin("ToolBar",nullptr, m_ToolBarFlags))
	{
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		if (ImGui::BeginMenuBar())
		{
			for (const auto &itr : m_DebugWindows)
			{
				if (ImGui::BeginMenu(itr.first.c_str()))
				{
					for (const auto &window : itr.second)
					{
						if (!window)
							continue;
						bool isOpen = window->IsOpen();
						if (ImGui::MenuItem(window->GetName().c_str(), nullptr, isOpen))
						{
							window->SetIsOpen(!isOpen);
						}
					}
					ImGui::EndMenu();
				}
			}

			ImGui::EndMenuBar();
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();

	for (const auto &itr : m_DebugWindows)
	{
		for(const auto &window : itr.second)
		{
			if (!window)
				continue;

			if (window->IsOpen())
			{
				ImGui::Begin(window->GetName().c_str());
				window->Draw();
				ImGui::End();
			}
		}
	}
}

DebugWindow *DebugManager::CreateDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name)
{
	DebugWindow *NewWindow = new DebugWindow(In_Name);
	auto itr = m_DebugWindows.try_emplace(In_GroupName.data());
	itr.first->second.push_back(NewWindow);
	return NewWindow;
}

DebugWindow *DebugManager::GetDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name)
{
	auto itr = m_DebugWindows.find(In_GroupName.data());
	if (itr != m_DebugWindows.end())
	{
		for (const auto &window : itr->second)
		{
			if (window && window->GetName() == In_Name)
			{
				return window;
			}
		}
	}
	return c_NullWindow;
}
