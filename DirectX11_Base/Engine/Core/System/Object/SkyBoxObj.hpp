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
#include "Core/DirectX11/Renderer/SkyBoxRenderer.hpp"

/// <summary>
/// SkyBoxObjクラス
/// </summary>
class SkyBoxObj : public GameObject
{
public:
	SkyBoxObj();
	SkyBoxObj(_In_ std::string In_Name);
	~SkyBoxObj();

	void Awake() noexcept override;

	void SetFilePath(_In_ const FilePath &In_Path) noexcept;
	void SetCamera(_In_ GameObject *In_Cam) noexcept;

	void LateUpdate(_In_ float In_DeltaTime) noexcept override final;

private:
	GameObject *m_pCameraObj; // カメラオブジェクト
	SkyBoxRenderer *m_pRenderComponent; // スカイボックス描画コンポーネント
};
