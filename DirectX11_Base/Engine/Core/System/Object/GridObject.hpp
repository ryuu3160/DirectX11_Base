/*+===================================================================
	File: GridObject.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 07:17:29 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "GameObject.hpp"
#include "Core/DirectX11/Renderer/LineRenderer.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// GridObjectクラス
/// </summary>
class GridObject : public GameObject
{
public:
	GridObject();
	GridObject(_In_ std::string In_Name);
	~GridObject();

	void Init() noexcept override;

	void Awake() noexcept override;

	void SetCamera(_In_ GameObject *In_Cam) noexcept;

	void Update(_In_ float In_DeltaTime) noexcept override final;

private:

	void CreateGrid() noexcept;

private:
	GameObject *m_pCameraObj; // カメラオブジェクト
	LineRenderer *m_pRenderComponent; // スカイボックス描画コンポーネント

	float m_GridSize;
	float m_GridMargin;
	float m_GridSizePrev;
	float m_GridMarginPrev;
	bool m_GridAxis;
	bool m_GridAxisPrev;
	DirectX::XMFLOAT4 m_GridColor;
	DirectX::XMFLOAT4 m_GridColorPrev;
	ItemGroup *m_pDebugGroup;
};
