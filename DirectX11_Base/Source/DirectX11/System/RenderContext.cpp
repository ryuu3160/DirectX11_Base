/*+===================================================================
	File: RenderContext.cpp
	Summary: レンダーコンテキストクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/12 Wed AM 07:47:19 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderContext.hpp"
#include "System/Component/Camera.hpp"
#include "DirectX11/Resource/Texture.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

RenderContext::RenderContext()
	: m_pCamera(nullptr), m_pRTV(nullptr), m_pDSV(nullptr), m_bIsMainContext(false)
{
}

RenderContext::~RenderContext()
{
	m_pCamera = nullptr;
	m_pRTV = nullptr;
	m_pDSV = nullptr;
}

void RenderContext::CreateMainContext(_In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept
{
	Create(In_Camera, In_RTV, In_DSV);
	m_bIsMainContext = true;
}

void RenderContext::Create(_In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept
{
	m_pCamera = In_Camera;
	m_pRTV = In_RTV;
	m_pDSV = In_DSV;
	ReCalculateMatrices();
}

void RenderContext::ReCalculateMatrices() noexcept
{
	m_ViewMatrix = m_pCamera->GetView(false);

	bool PrevIs3D = m_pCamera->Is3D();
	if (PrevIs3D)
	{
		m_ProjectionMatrix = m_pCamera->GetProj(false);
		m_pCamera->Set3D(false);
		m_2DProjMatrix = m_pCamera->GetProj(false);
	}
	else
	{
		m_2DProjMatrix = m_pCamera->GetProj(false);
		m_pCamera->Set3D(true);
		m_ProjectionMatrix = m_pCamera->GetProj(false);
	}

	m_pCamera->Set3D(PrevIs3D);
}

DirectX::XMFLOAT4X4 RenderContext::GetView(_In_ bool In_Transpose) const noexcept
{
	if (In_Transpose)
	{
		DirectX::XMFLOAT4X4 TransposedView;
		DirectX::XMStoreFloat4x4(&TransposedView, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix)));
		return TransposedView;
	}
	return m_ViewMatrix;
}

DirectX::XMFLOAT4X4 RenderContext::GetProj(_In_ bool In_Transpose) const noexcept
{
	if (In_Transpose)
	{
		DirectX::XMFLOAT4X4 TransposedProj;
		DirectX::XMStoreFloat4x4(&TransposedProj, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
		return TransposedProj;
	}
	return m_ProjectionMatrix;
}

DirectX::XMFLOAT4X4 RenderContext::Get2DProj(_In_ bool In_Transpose) const noexcept
{
	if (In_Transpose)
	{
		DirectX::XMFLOAT4X4 Transposed2DProj;
		DirectX::XMStoreFloat4x4(&Transposed2DProj, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_2DProjMatrix)));
		return Transposed2DProj;
	}
	return m_2DProjMatrix;
}
