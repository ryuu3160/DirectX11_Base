/*+===================================================================
	File: RenderTargetManager.cpp
	Summary: RTV/DSV管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/12 Wed AM 04:21:39 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderTargetManager.hpp"
#include "Engine/Core/DirectX11/Resource/Texture.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

RenderTargetManager::RenderTargetManager()
	: m_pDefaultRTV(nullptr), m_pDefaultDSV(nullptr)
{
}

RenderTargetManager::~RenderTargetManager()
{
	if (m_pDefaultRTV)
	{
		delete m_pDefaultRTV;
		m_pDefaultRTV = nullptr;
	}
	if (m_pDefaultDSV)
	{
		delete m_pDefaultDSV;
		m_pDefaultDSV = nullptr;
	}

	for (auto &pair : m_RenderTargets)
	{
		if (pair.second)
		{
			delete pair.second;
			pair.second = nullptr;
		}
	}
	for (auto &pair : m_DepthStencils)
	{
		if (pair.second)
		{
			delete pair.second;
			pair.second = nullptr;
		}
	}
	m_RenderTargets.clear();
	m_DepthStencils.clear();
}

std::pair<RenderTarget *, DepthStencil *> RenderTargetManager::InitializeDefaultResources(_In_ UINT In_Width, _In_ UINT In_Height, _In_ bool In_UseStencil)
{
	if(m_pDefaultDSV && m_pDefaultRTV)
		return { m_pDefaultRTV, m_pDefaultDSV };

	m_pDefaultRTV = new RenderTarget();
	m_pDefaultRTV->CreateFromScreen();

	m_pDefaultDSV = new DepthStencil();
	m_pDefaultDSV->Create(In_Width, In_Height, false);

	return { m_pDefaultRTV, m_pDefaultDSV };
}

RenderTarget *RenderTargetManager::CreateRenderTarget(_In_ const std::string &In_Name, _In_ DXGI_FORMAT In_Format, _In_ UINT In_Width, _In_ UINT In_Height)
{
	auto RTV = new RenderTarget();

	if (FAILED(RTV->Create(In_Format, In_Width, In_Height)))
	{
		return nullptr;
	}

	auto Result = m_RenderTargets.try_emplace(In_Name, RTV);

	return Result.first->second;
}

DepthStencil *RenderTargetManager::CreateDepthStencil(_In_ const std::string &In_Name, _In_ UINT In_Width, _In_ UINT In_Height, _In_ bool In_UseStencil)
{
	auto DSV = new DepthStencil();

	if (FAILED(DSV->Create(In_Width, In_Height, In_UseStencil)))
	{
		return nullptr;
	}

	auto Result = m_DepthStencils.try_emplace(In_Name, DSV);

	return Result.first->second;
}
