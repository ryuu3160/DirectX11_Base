/*+===================================================================
	File: CustomWindowProc.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/7 Sat PM 03:10:35 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	定数定義
// ==============================
namespace
{
}

namespace EngineCustomWindowProc
{
	/// <summary>
	/// Altキーの単独押しを無効化するプロシージャ
	/// </summary>
	LRESULT DisableSingleAltKey(_In_ HWND In_hWnd, _In_ UINT In_unMessage, _In_ WPARAM In_wParam, _In_ LPARAM In_lParam);
}
