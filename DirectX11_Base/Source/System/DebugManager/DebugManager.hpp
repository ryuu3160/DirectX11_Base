/*+===================================================================
	File: DebugManager.hpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:16 初回作成
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
/// DebugManagerクラス
/// </summary>
class DebugManager : public Singleton<DebugManager>
{
	friend class Singleton<DebugManager>;
public:
	DebugManager();
	~DebugManager();

	// ------------------------------
	//  Getter
	// ------------------------------


	// ------------------------------
	//  Setter
	// ------------------------------

private:

};
