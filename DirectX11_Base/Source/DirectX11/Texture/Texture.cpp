/*+===================================================================
	File: Texture.hpp
	Summary: Texture関連クラス
	Author: AT13C 01 青木雄一郎
	Date: 10/19/2024 Sat AM 01:20:06 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Texture.hpp"
#include "DirectX11/DirectXTex/TextureLoad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

/// <summary>
/// テクスチャ
/// </summary>
Texture::Texture()
	: m_width(0), m_height(0)
	, m_pTex(nullptr)
	, m_pSRV(nullptr)
{
}
Texture::~Texture()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pSRV = nullptr;
	m_pTex = nullptr;
}
HRESULT Texture::Create(_In_ const FilePath &In_FileName) noexcept
{
	HRESULT hr = S_OK;

	// 文字変換
	wchar_t wPath[MAX_PATH];
	size_t wLen = 0;
	MultiByteToWideChar(0, 0, In_FileName.data(), -1, wPath, MAX_PATH);

	// ファイル別読み込み
	DirectX::TexMetadata mdata;
	DirectX::ScratchImage image;
	if (strstr(In_FileName.data(), ".tga"))
		hr = DirectX::LoadFromTGAFile(wPath, &mdata, image);
	else
		hr = DirectX::LoadFromWICFile(wPath, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &mdata, image);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// シェーダリソース生成
	hr = CreateShaderResourceView(DX11_Initialize::GetInstance().GetDevice(), image.GetImages(), image.GetImageCount(), mdata, m_pSRV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		m_width = (UINT)mdata.width;
		m_height = (UINT)mdata.height;
	}
	return hr;
}
HRESULT Texture::Create(_In_ const aiTexture *In_aiTex) noexcept
{
	HRESULT hr = S_OK;

	DirectX::TexMetadata mdata;
	DirectX::ScratchImage image;
	// 埋め込みテクスチャの読み込み
	hr = DirectX::LoadFromWICMemory(In_aiTex->pcData, In_aiTex->mWidth, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &mdata, image);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// シェーダリソース生成
	hr = CreateShaderResourceView(DX11_Initialize::GetInstance().GetDevice(), image.GetImages(), image.GetImageCount(), mdata, m_pSRV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		m_width = (UINT)mdata.width;
		m_height = (UINT)mdata.height;
	}
	return hr;
}
HRESULT Texture::Create(_In_ DXGI_FORMAT In_Format, _In_ const UINT In_Width, _In_ const UINT In_Height, _In_ const void *In_pData) noexcept
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(In_Format, In_Width, In_Height);
	return CreateResource(desc, In_pData);
}

D3D11_TEXTURE2D_DESC Texture::MakeTexDesc(_In_ DXGI_FORMAT In_Format, _In_ const UINT &In_Width, _In_ const UINT &In_Height) noexcept
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = In_Format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = In_Width;
	desc.Height = In_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	return desc;
}
HRESULT Texture::CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData) noexcept
{
	HRESULT hr = E_FAIL;

	auto Device = DX11_Initialize::GetInstance().GetDevice();

	// テクスチャ作成
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = In_pData;
	data.SysMemPitch = In_Desc.Width * 4;
	hr = Device->CreateTexture2D(&In_Desc, In_pData ? &data : nullptr, m_pTex.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	switch (In_Desc.Format)
	{
	default:						srvDesc.Format = In_Desc.Format;			break;
	case DXGI_FORMAT_R32_TYPELESS: 	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;	break;
	}
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// 生成
	hr = Device->CreateShaderResourceView(m_pTex.Get(), &srvDesc, m_pSRV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		m_width = In_Desc.Width;
		m_height = In_Desc.Height;
	}
	return hr;
}

/// <summary>
/// レンダーターゲット
/// </summary>
RenderTarget::RenderTarget()
	: m_pRTV(nullptr)
{
}
RenderTarget::~RenderTarget()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pRTV = nullptr;
}
void RenderTarget::Clear()
{
	static float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Clear(color);
}
void RenderTarget::Clear(_In_ const float *In_Color) noexcept
{
	DX11_Initialize::GetInstance().GetDeviceContext()->ClearRenderTargetView(m_pRTV.Get(), In_Color);
}
HRESULT RenderTarget::Create(_In_ DXGI_FORMAT In_Format, _In_ const UINT &In_Width, _In_ const UINT &In_Height) noexcept
{
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(In_Format, In_Width, In_Height);
	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	return CreateResource(desc);
}
HRESULT RenderTarget::CreateFromScreen() noexcept
{
	HRESULT hr;
	DX11_Initialize& Instance = DX11_Initialize::GetInstance();

	// バックバッファのポインタを取得
	ID3D11Texture2D *pBackBuffer = NULL;
	hr = Instance.GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(m_pTex.GetAddressOf()));
	if (FAILED(hr)) { return hr; }

	// バックバッファへのポインタを指定してレンダーターゲットビューを作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.Texture2D.MipSlice = 0;
	hr = Instance.GetDevice()->CreateRenderTargetView(m_pTex.Get(), &rtvDesc, m_pRTV.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTex->GetDesc(&desc);
		m_width = desc.Width;
		m_height = desc.Height;
	}
	return hr;
}
HRESULT RenderTarget::CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData) noexcept
{
	// テクスチャリソース作成
	HRESULT hr = Texture::CreateResource(In_Desc, nullptr);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = In_Desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	// 生成
	return DX11_Initialize::GetInstance().GetDevice()->CreateRenderTargetView(m_pTex.Get(), &rtvDesc, m_pRTV.GetAddressOf());
}

/// <summary>
/// 深度テクスチャ
/// </summary>
DepthStencil::DepthStencil()
	: m_pDSV(nullptr)
{
}
DepthStencil::~DepthStencil()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pDSV = nullptr;
}
void DepthStencil::Clear()
{
	DX11_Initialize::GetInstance().GetDeviceContext()->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
HRESULT DepthStencil::Create(_In_ const UINT &In_Width, _In_ const UINT &In_Height, _In_ const bool &In_UseStencil) noexcept
{
	// https://docs.microsoft.com/ja-jp/windows/win32/direct3d11/d3d10-graphics-programming-guide-depth-stencil#compositing
	D3D11_TEXTURE2D_DESC desc = MakeTexDesc(In_UseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS, In_Width, In_Height);
	desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	return CreateResource(desc);
}
HRESULT DepthStencil::CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData) noexcept
{
	// ステンシル使用判定
	bool useStencil = (In_Desc.Format == DXGI_FORMAT_R24G8_TYPELESS);

	// リソース生成
	In_Desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = Texture::CreateResource(In_Desc, nullptr);
	if (FAILED(hr)) { return hr; }

	// 設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = useStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	// 生成
	return DX11_Initialize::GetInstance().GetDevice()->CreateDepthStencilView(m_pTex.Get(), &dsvDesc, m_pDSV.GetAddressOf());
}