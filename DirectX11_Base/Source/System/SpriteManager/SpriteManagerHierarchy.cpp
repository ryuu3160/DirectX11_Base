/*+===================================================================
	File: SpriteManagerHierarchy.cpp
	Summary: スプライトマネージャーのヒエラルキービューウィンドウ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:37:11 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManagerHierarchy.hpp"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"

SpriteManagerHierarchy::SpriteManagerHierarchy()
	: SpriteManagerWindow("Hierarchy##SpriteManager")
{
	m_WindowType = Hierarchy;
}

SpriteManagerHierarchy::~SpriteManagerHierarchy()
{
}

void SpriteManagerHierarchy::Draw(_In_ std::list<std::string> In_2DSpriteNames, _In_ std::list<std::string> In_3DSpriteNames, _Inout_ int *Inout_2DIndex, _Inout_ int *Inout_3DIndex) noexcept
{

	const char *items_2D[100];
	const char *items_3D[100];
	if (!In_2DSpriteNames.empty())
	{
		auto itr = In_2DSpriteNames.begin();
		for (int i = 0; i < In_2DSpriteNames.size() && i < 100; ++i,++itr)
			items_2D[i] = itr->c_str();
	}
	if (!In_3DSpriteNames.empty())
	{
		auto itr2 = In_3DSpriteNames.begin();
		for (int i = 0; i < In_3DSpriteNames.size() && i < 100; ++i,++itr2)
			items_3D[i] = itr2->c_str();
	}

	ImGui::ListBox("Hierarchy##2D", Inout_2DIndex, items_2D, static_cast<int>(In_2DSpriteNames.size()));
	ImGui::ListBox("Hierarchy##3D", Inout_3DIndex, items_3D, static_cast<int>(In_3DSpriteNames.size()));
}
