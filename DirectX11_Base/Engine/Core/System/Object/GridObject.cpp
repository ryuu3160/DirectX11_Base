/*+===================================================================
	File: GridObject.cpp
	Summary: グリッドを表示するオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 07:17:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GridObject.hpp"
#include "Core/DirectX11/Renderer/LineRenderer.hpp"
#include "Core/System/Component/GridComponent.hpp"

GridObject::GridObject()
	: GridObject("GridObject")
{
}

GridObject::GridObject(_In_ std::string In_Name)
	: GameObject(In_Name)
	, m_pRenderComponent(nullptr)
	, m_pGridComponent(nullptr)
{
}

GridObject::~GridObject()
{
}

void GridObject::Init() noexcept
{
}

void GridObject::Awake() noexcept
{
	// グリッド描画コンポーネントの生成
	m_pRenderComponent = AddComponent<LineRenderer>();
	m_pRenderComponent->SetLayerGroup(LayerGroup_Grid);
	m_pGridComponent = AddComponent<GridComponent>();
}
