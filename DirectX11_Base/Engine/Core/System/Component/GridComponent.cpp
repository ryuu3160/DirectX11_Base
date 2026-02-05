/*+===================================================================
	File: GridComponent.cpp
	Summary: Gridを設定、描画するコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2026/2/4 Wed PM 07:34:35 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GridComponent.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/DirectX11/Renderer/LineRenderer.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

GridComponent::GridComponent()
	: Component("GridComponent")
	, m_pRenderComponent(nullptr)
	, m_pDebugGroup(nullptr)
	, m_GridSize(20.0f), m_GridMargin(1.0f), m_GridColor{ 0.5f, 0.5f, 0.5f, 1.0f }
	, m_GridAxis(true)
{
}

GridComponent::~GridComponent()
{
	m_pRenderComponent = nullptr;
	m_pDebugGroup = nullptr;
}

void GridComponent::Init() noexcept
{
	auto &DebugMenu = DebugManager::GetInstance().GetDebugWindowRef("System", "DebugMenu");
	m_pDebugGroup = DebugMenu.CreateItem<ItemGroup>("Grid##GridObject");
	m_pDebugGroup->CreateGroupItem<ItemSetNextItemWidth>("##GridObject", 100.0f);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridSize##GridObject", DebugItem::Float, true);
	m_pDebugGroup->CreateGroupItem<ItemSameLine>("1##GridObject");
	m_pDebugGroup->CreateGroupItem<ItemSetNextItemWidth>("##GridObject", 100.0f);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridMargin##GridObject", DebugItem::Float, true);
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridColor##GridObject", DebugItem::Color, true);
	m_pDebugGroup->CreateGroupItem<ItemBind>("Active##GridObject", DebugItem::Bool, &m_IsActive);
	m_pDebugGroup->CreateGroupItem<ItemSameLine>("2##GridObject");
	m_pDebugGroup->CreateGroupItem<ItemValue>("GridAxis##GridObject", DebugItem::Bool, true);

	m_pRenderComponent = m_pGameObject->GetComponent<LineRenderer>();
	// グリッドの作成
	CreateGrid();
}

void GridComponent::Awake() noexcept
{
}

void GridComponent::Update(_In_ float In_DeltaTime) noexcept
{
	auto GridSize = (*m_pDebugGroup)["GridSize##GridObject"].GetFloat();
	auto GridMargin = (*m_pDebugGroup)["GridMargin##GridObject"].GetFloat();
	auto GridColor = (*m_pDebugGroup)["GridColor##GridObject"].GetColor();
	auto GridAxis = (*m_pDebugGroup)["GridAxis##GridObject"].GetBool();

	if(m_GridAxis != GridAxis || m_GridSize != GridSize || m_GridMargin != GridMargin || m_GridColor != GridColor)
	{
		m_GridAxis = GridAxis;
		m_GridSize = GridSize;
		m_GridMargin = GridMargin;
		m_GridColor = GridColor;
		// グリッドの削除
		m_pRenderComponent->ClearLines();
		// グリッドの再作成
		CreateGrid();
	}
}

void GridComponent::CreateGrid() noexcept
{
	// グリッドの作成
	m_pRenderComponent->SetColor(m_GridColor);

	auto AddGrid = std::async(std::launch::async, [this]()
		{
			float d = m_GridMargin;
			float s = m_GridSize;
			for(; s >= m_GridMargin && m_GridMargin > 0.0f; d += m_GridMargin, s -= m_GridMargin)
			{
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(d, 0.0f, -m_GridSize), DirectX::XMFLOAT3(d, 0.0f, m_GridSize), m_GridColor, m_GridColor);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-d, 0.0f, -m_GridSize), DirectX::XMFLOAT3(-d, 0.0f, m_GridSize), m_GridColor, m_GridColor);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, d), DirectX::XMFLOAT3(m_GridSize, 0.0f, d), m_GridColor, m_GridColor);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, -d), DirectX::XMFLOAT3(m_GridSize, 0.0f, -d), m_GridColor, m_GridColor);
			}
		});

	// 軸描画
	auto AddAxis = std::async(std::launch::async, [this]()
		{
			if(m_GridAxis)
			{
				DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
				DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
				DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(m_GridSize, 0.0f, 0.0f), red, red);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, -m_GridSize, 0.0f), DirectX::XMFLOAT3(0.0f, m_GridSize, 0.0f), green, green);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -m_GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, m_GridSize), blue, blue);
			}
			else
			{
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(-m_GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(m_GridSize, 0.0f, 0.0f), m_GridColor, m_GridColor);
				m_pRenderComponent->AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -m_GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, m_GridSize), m_GridColor, m_GridColor);
			}
		});

	AddGrid.wait();
	AddAxis.wait();
}
