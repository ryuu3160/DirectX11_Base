/*+===================================================================
	File: SceneResult.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/12 Fri AM 07:12:17 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// SceneResultクラス
/// </summary>
class SceneResult : public SceneBase
{
public:
	SceneResult(_In_ const bool &In_IsClear);
	// シーンの初期化
	void Init() override;
	// シーンの解放
	void Uninit() override;
	// シーンの更新
	void Update(_In_ float In_DeltaTime) override;
	// シーンの描画
	void Draw() override;
private:

private:
	int m_Index = 0;
	float m_ButtonScaleX = 1.0f;
	float m_ButtonScaleY = 1.0f;
	float m_Time = 0.0f;
	bool m_IsChange = false;
	bool m_IsClear;
#ifdef _DEBUG
	DirectX::XMFLOAT3 m_CamPos{};
	DirectX::XMFLOAT3 m_CamLook{};
	DirectX::XMFLOAT3 m_CamUp{};
#endif

};
