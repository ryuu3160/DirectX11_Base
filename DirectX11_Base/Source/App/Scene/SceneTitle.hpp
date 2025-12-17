/*+===================================================================
	File: SceneTitle.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/17 Wed AM 02:24:50 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Scene/SceneBase.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// SceneTitleクラス
/// </summary>
class SceneTitle : public SceneBase
{
public:
	SceneTitle() : SceneBase("Title") {}
	// シーンの初期化
	void Init() override;
	// シーンの解放
	void Uninit() override;
	// シーンの更新
	void Update(_In_ float In_Tick) override;
	// シーンの描画
	void Draw() override;
private:

	void ChangeScene();
private:
	int m_Index = 0;
	Random *m_Random;
};
