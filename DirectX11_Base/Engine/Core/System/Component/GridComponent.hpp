/*+===================================================================
	File: GridComponent.hpp
	Summary: Gridを設定、描画するコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2026/2/4 Wed PM 07:34:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Component/Component.hpp"

// ==============================
//	前方宣言
// ==============================
class LineRenderer;
class ItemGroup;

// ---ComponentDescription---
// Gridを設定、描画するコンポーネント
// --------------------------
class GridComponent : public Component
{
public:
	GridComponent();
	~GridComponent();

	void Init() noexcept override;

	void Awake() noexcept override;

	void Update(_In_ float In_DeltaTime) noexcept override final;

private:

	void CreateGrid() noexcept;

private:
	LineRenderer *m_pRenderComponent; // スカイボックス描画コンポーネント

	float m_GridSize;
	float m_GridMargin;
	bool m_GridAxis;
	DirectX::XMFLOAT4 m_GridColor;
	ItemGroup *m_pDebugGroup;
};
