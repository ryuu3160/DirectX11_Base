/*+===================================================================
	File: SceneTitle.hpp
	Summary: タイトルシーン
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/11 Thu AM 02:24:51 初回作成
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
	void Update() override;
	// シーンの描画
	void Draw() override;
private:

	void ChangeScene();
private:
	int m_Index = 0;
	bool m_IsChange = false;
#ifdef _DEBUG
	DirectX::XMFLOAT3 m_CamPos{};
	DirectX::XMFLOAT3 m_CamLook{};
	DirectX::XMFLOAT3 m_CamUp{};
#endif

};
