/*+===================================================================
	File: SeaLevel.hpp
	Summary: 海面オブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/09/04 Thu PM 02:17:09 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/GameObject.hpp"
#include "DirectX11/System/InstancedModelRenderer.hpp"

/// <summary>
/// SeaLevelクラス
/// </summary>
class SeaLevel : public GameObject
{
public:
	SeaLevel();
	~SeaLevel();

	void SetFilePath(_In_ const FilePath &In_Path) noexcept;
	void SetCamera(_In_ GameObject *In_Cam) noexcept;
	void SetPlayer(_In_ GameObject *In_Player) noexcept;

	void LateUpdate() override;

private:
	GameObject *m_pPlayer;
	GameObject *m_pCameraObj; // カメラオブジェクト
	InstancedModelRenderer *m_pRenderComponent;
};
