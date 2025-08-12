/*+===================================================================
	File: SpriteManagerWindow.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:34:53 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManagerWindow.hpp"

SpriteManagerWindow::SpriteManagerWindow(_In_ std::string_view In_WindowName)
{
	m_strWindowName = In_WindowName.data();
}

SpriteManagerWindow::~SpriteManagerWindow()
{
}
