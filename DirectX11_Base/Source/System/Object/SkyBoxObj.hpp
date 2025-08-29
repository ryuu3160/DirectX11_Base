/*+===================================================================
	File: SkyBoxObj.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/30 Sat AM 01:13:20 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "GameObject.hpp"
#include "DirectX11/System/SkyBoxRenderer.hpp"

/// <summary>
/// SkyBoxObjクラス
/// </summary>
class SkyBoxObj : public GameObject
{
public:
	SkyBoxObj();
	~SkyBoxObj();

	void SetFilePath(_In_ const FilePath &In_Path) noexcept;
	void SetCamera(_In_ GameObject *In_Cam) noexcept;

	void LateUpdate() override final;

private:
	GameObject *m_pCameraObj; // カメラオブジェクト
	SkyBoxRenderer *m_pRenderComponent; // スカイボックス描画コンポーネント
};
