/*+===================================================================
	File: SceneRoot.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:10 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SceneBase.hpp"

/// <summary>
/// SceneRootクラス
/// </summary>
class SceneRoot : public SceneBase
{
public:
	SceneRoot() : SceneBase("Root") {}
	// シーンの初期化
	void Init() override;
	// シーンの解放
	void Uninit() override;
	// シーンの更新
	void Update(_In_ float In_DeltaTime) override;
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
