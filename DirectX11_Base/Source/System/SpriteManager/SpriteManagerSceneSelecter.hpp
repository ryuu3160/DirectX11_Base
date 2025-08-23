/*+===================================================================
	File: SpriteManagerSceneSelecter.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/27 Sun PM 11:29:49 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SpriteManagerWindow.hpp"

/// <summary>
/// SpriteManagerSceneSelecterクラス
/// </summary>
class SpriteManagerSceneSelecter : public SpriteManagerWindow
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpriteManagerSceneSelecter();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteManagerSceneSelecter();

	void Draw(_In_ std::map<std::string, HoldFilePath> In_Scenes, _Inout_ int *Inout_Index) noexcept;

private:
	char m_NewSceneName[cx_MaxStringLength];
};
