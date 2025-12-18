/*+===================================================================
	File: GridObject.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 07:17:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GridObject.hpp"

GridObject::GridObject()
	: GridObject("GridObject")
{
}

GridObject::GridObject(_In_ std::string In_Name)
	: GameObject(In_Name)
	, m_pRenderComponent(nullptr)
	, m_pCameraObj(nullptr), m_pDebugGroup(nullptr)
	, m_GridSize(20.0f), m_GridMargin(1.0f), m_GridColor{ 0.5f, 0.5f, 0.5f, 1.0f }
	, m_GridAxis(true)
	, m_GridSizePrev(20.0f), m_GridMarginPrev(1.0f), m_GridColorPrev{ 0.5f, 0.5f, 0.5f, 1.0f }
	, m_GridAxisPrev(true)
{
	// グリッド描画コンポーネントの生成
	m_pRenderComponent = AddComponent<LineRenderer>();
	m_pRenderComponent->SetLayerGroup(LayerGroup_Grid);

	// グリッドの作成
	CreateGrid();
}

GridObject::~GridObject()
{
}

void GridObject::Init() noexcept
{
	auto &DebugMenu = DebugManager::GetInstance().GetDebugWindowRef("System", "DebugMenu");
	m_pDebugGroup = DebugMenu.CreateItem<ItemGroup>("Grid##GridObject");
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridSize##GridObject", DebugItem::Float, true);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridMargin##GridObject", DebugItem::Float, true);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridColor##GridObject", DebugItem::Color, true);
	m_pDebugGroup->CreateGroupItem<ItemBind>("Active##GridObject", DebugItem::Bool, &m_IsActive);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridAxis##GridObject", DebugItem::Bool, true);
}

void GridObject::SetCamera(_In_ GameObject *In_Cam) noexcept
{
}

void GridObject::Update(_In_ float In_Tick) noexcept
{
	m_GridSize = (*m_pDebugGroup)["GridSize##GridObject"].GetFloat();
	m_GridMargin = (*m_pDebugGroup)["GridMargin##GridObject"].GetFloat();
	m_GridColor = (*m_pDebugGroup)["GridColor##GridObject"].GetColor();
	m_GridAxis = (*m_pDebugGroup)["GridAxis##GridObject"].GetBool();

	if(m_GridAxis != m_GridAxisPrev || m_GridSize != m_GridSizePrev || m_GridMargin != m_GridMarginPrev || m_GridColor != m_GridColorPrev)
	{
		CreateGrid();
		m_GridAxisPrev = m_GridAxis;
		m_GridSizePrev = m_GridSize;
		m_GridMarginPrev = m_GridMargin;
		m_GridColorPrev = m_GridColor;
	}
}

void GridObject::CreateGrid() noexcept
{
	// グリッドの削除
	//m_pRenderComponent->ClearLines();
	// グリッドの作成
	float d = m_GridMargin;
	float s = m_GridSize;
	for(; s >= m_GridMargin && m_GridMargin > 0.0f; d += m_GridMargin, s -= m_GridMargin)
	{
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(d, 0.0f, -m_GridSize), DirectX::XMFLOAT3(d, 0.0f, m_GridSize));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-d, 0.0f, -m_GridSize), DirectX::XMFLOAT3(-d, 0.0f, m_GridSize));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, d), DirectX::XMFLOAT3(m_GridSize, 0.0f, d));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, -d), DirectX::XMFLOAT3(m_GridSize, 0.0f, -d));
	}
	// 軸描画
	if(m_GridAxis)
	{
		m_pRenderComponent->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(m_GridSize, 0.0f, 0.0f));
		m_pRenderComponent->SetColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, -m_GridSize, 0.0f), DirectX::XMFLOAT3(0.0f, m_GridSize, 0.0f));
		m_pRenderComponent->SetColor(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -m_GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, m_GridSize));
	}
	else
	{
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(m_GridSize, 0.0f, 0.0f));
		m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -m_GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, m_GridSize));
	}
}
