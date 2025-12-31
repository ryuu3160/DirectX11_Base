/*+===================================================================
	File: SpriteManagerHierarchy.hpp
	Summary: スプライトマネージャーのヒエラルキービューウィンドウ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:37:04 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SpriteManagerWindow.hpp"

/// <summary>
/// SpriteManagerHierarchyクラス
/// </summary>
class SpriteManagerHierarchy : public SpriteManagerWindow
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpriteManagerHierarchy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteManagerHierarchy();

	void Draw(_In_ std::list<std::string> In_2DSpriteNames,_In_ std::list<std::string> In_3DSpriteNames, _Inout_ int *Inout_2DIndex, _Inout_ int *Inout_3DIndex) noexcept;

private:

};
