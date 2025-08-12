/*+===================================================================
	File: SpriteManagerLoader.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:54:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SpriteManagerWindow.hpp"

/// <summary>
/// SpriteManagerLoaderクラス
/// </summary>
class SpriteManagerLoader : public SpriteManagerWindow
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpriteManagerLoader();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteManagerLoader();

	void Draw() noexcept;

private:
	char m_cSpriteName[cx_MaxStringLength]; // スプライト名
	char m_cFilePath[cx_MaxStringLength]; // ファイルパス
	int m_nLayer = 0; // レイヤー番号
	float m_fScale = 1.0f; // スケール
	bool m_bIs3D = false; // 3Dスプライトかどうか
	bool m_bIsBillBoard = false; // ビルボードスプライトかどうか
};
