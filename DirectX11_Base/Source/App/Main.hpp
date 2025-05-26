/*+===================================================================
	File: Main.hpp
	Summary: ゲームのメイン処理
	Author: AT13C192 01 青木雄一郎
	Date: 2025/5/22 Thu PM 06:14:17 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

namespace Main
{
	HRESULT Init(HWND hWnd, int width, int height);
	void Uninit();
	void Update();
	void Draw();
}
