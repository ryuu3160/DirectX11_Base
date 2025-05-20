/*+===================================================================
	File: Main.cpp
	Summary: エントリーポイント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/05/20 Tue PM 12:11:45 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Main.hpp"


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}