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

// ==============================
//  前方宣言
// ==============================
class SceneBase;
class RenderTarget;
class DepthStencil;

namespace Main
{
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void InitializeDebugWindows() noexcept;
}
