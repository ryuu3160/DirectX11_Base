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
#include "Engine.hpp"

/// <summary>
/// SeaLevelクラス
/// </summary>
class SeaLevelObj : public GameObject
{
public:
	SeaLevelObj(_In_ std::string_view In_Name, _In_ bool In_IsInstance);
	~SeaLevelObj();

	void Awake() noexcept override;

	void SetFilePath(_In_ const FilePath &In_Path) noexcept;
	void SetCamera(_In_ GameObject *In_Cam) noexcept;
	void SetPlayer(_In_ GameObject *In_Player) noexcept;
	void SetPatternScale(_In_ const DirectX::XMFLOAT2 &In_Scale) noexcept { m_PatternScale = In_Scale; }

private:
	GameObject *m_pPlayer;
	GameObject *m_pCameraObj; // カメラオブジェクト
	RenderComponent *m_pRenderComponent;
	DirectX::XMFLOAT2 m_PatternScale; // パターンスケール
	bool m_IsInstance; // インスタンシングを使用するかどうか
};
