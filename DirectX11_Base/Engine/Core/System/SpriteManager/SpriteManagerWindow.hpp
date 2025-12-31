/*+===================================================================
	File: SpriteManagerWindow.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:34:47 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

enum SpriteManagerWindowType
{
	None = 0, // ウィンドウタイプなし
	Hierarchy,
	Inspector,
	Loader,
	SceneSelecter, // シーンセレクター
};

// ==============================
//  定数
// ==============================
namespace
{
	const inline constexpr int cx_MaxStringLength = 256; // ファイルパスの最大長
}

/// <summary>
/// SpriteManagerWindowクラス
/// </summary>
class SpriteManagerWindow
{
public:
	SpriteManagerWindow(_In_ std::string_view In_WindowName);
	virtual ~SpriteManagerWindow();

	std::string GetWindowName() const noexcept { return m_strWindowName; }

	SpriteManagerWindowType GetWindowType() const noexcept { return m_WindowType; }

protected:
	SpriteManagerWindowType m_WindowType = None; // ウィンドウのタイプ

private:
	std::string m_strWindowName; // ウィンドウ名
};
