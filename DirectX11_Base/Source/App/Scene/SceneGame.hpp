/*+===================================================================
	File: SceneGame.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 03:57:01 初回作成
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
/// SceneGameクラス
/// </summary>
class SceneGame : public SceneBase
{
public:
	SceneGame() : SceneBase("Game") {}
	// シーンの初期化
	void Init() override;
	// シーンの解放
	void Uninit() override;
	// シーンの更新
	void Update() override;
	// シーンの描画
	void Draw() override;
private:

	void ChangeScene();
private:
	int m_Index = 0;
#ifdef _DEBUG
	DirectX::XMFLOAT3 m_CamPos{};
	DirectX::XMFLOAT3 m_CamLook{};
	DirectX::XMFLOAT3 m_CamUp{};
#endif

};
