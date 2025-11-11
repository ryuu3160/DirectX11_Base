/*+===================================================================
	File: DX11_Core.hpp
	Summary: DirectX11のコア
	Author: AT13C 01 青木雄一郎
	Date: 10/19/2024 Sat AM 12:32:59 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DX11_Core.hpp"
#include "DirectX11/Resource/Texture.hpp"

DX11_Core::DX11_Core() : m_WindowColor{0.8f, 0.9f, 1.0f, 1.0f }, m_BlendFactor{0.0f, 0.0f, 0.0f, 0.0f}
{
	//nullptrでの初期化
	m_cpDevice = nullptr;
	m_cpContext = nullptr;
	m_cpSwapChain = nullptr;
	m_cpRasterizerState[0] = nullptr;
	m_cpRasterizerState[1] = nullptr;
	m_cpRasterizerState[2] = nullptr;

	m_hr = E_FAIL;
	//windowのサイズを取得
	m_Width = Window::GetInstance().GetWidth();
	m_Height = Window::GetInstance().GetHeight();

	m_RefreshRate_Numerator = 1000;
	m_RefreshRate_Denominator = 1;
	m_SampleDesc_Count = 1;
	m_SampleDesc_Quality = 0;
	m_BufferCount = 1;
	m_Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_FullScreen = false;

	//ドライバータイプの初期化
	m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	m_DriverTypes[0] = D3D_DRIVER_TYPE_HARDWARE;
	m_DriverTypes[1] = D3D_DRIVER_TYPE_WARP;
	m_DriverTypes[2] = D3D_DRIVER_TYPE_REFERENCE;
	m_nuDriverTypes = ARRAYSIZE(m_DriverTypes);

	//機能レベルの初期化
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_1;
	m_FeatureLevels[0] = D3D_FEATURE_LEVEL_11_1;
	m_FeatureLevels[1] = D3D_FEATURE_LEVEL_11_0;
	m_FeatureLevels[2] = D3D_FEATURE_LEVEL_10_1;
	m_FeatureLevels[3] = D3D_FEATURE_LEVEL_10_0;
	m_FeatureLevels[4] = D3D_FEATURE_LEVEL_9_3;
	m_FeatureLevels[5] = D3D_FEATURE_LEVEL_9_2;
	m_FeatureLevels[6] = D3D_FEATURE_LEVEL_9_1;
	m_nuFeatureLevels = ARRAYSIZE(m_FeatureLevels);
	m_nuCreateDeviceFlags = 0;

#ifdef _DEBUG
	m_nuCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
};

DX11_Core::~DX11_Core()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	for (int i = 0; i < 3; ++i)
	{
		m_cpRasterizerState[i] = nullptr;
	}
	for (int i = 0; i < BLEND_MAX; ++i)
	{
		m_cpBlendState[i] = nullptr;
	}
	for (int i = 0; i < SAMPLER_MAX; ++i)
	{
		m_cpSamplerState[i] = nullptr;
	}
	for (int i = 0; i < DEPTH_MAX; ++i)
	{
		m_cpDepthStencilState[i] = nullptr;
	}

	if (m_cpContext)
		m_cpContext->ClearState();
	if (m_cpSwapChain)
		m_cpSwapChain->SetFullscreenState(false, NULL);

	m_cpDevice = nullptr;
	m_cpContext = nullptr;
	m_cpSwapChain = nullptr;
}

HRESULT DX11_Core::Init()
{
	m_hr = E_FAIL;

	///==========================
	/// スワップチェインの作成
	/// =========================
	DXGI_SWAP_CHAIN_DESC ini_sd;
	ZeroMemory(&ini_sd, sizeof(ini_sd));
	ini_sd.BufferDesc.Width = m_Width; // フレームバッファの幅
	ini_sd.BufferDesc.Height = m_Height; // フレームバッファの高さ
	ini_sd.BufferDesc.Format = m_Format; // フレームバッファの色情報(各8bit)
	ini_sd.SampleDesc.Count = m_SampleDesc_Count; // マルチサンプリングの数
	ini_sd.BufferDesc.RefreshRate.Numerator = m_RefreshRate_Numerator;
	ini_sd.BufferDesc.RefreshRate.Denominator = m_RefreshRate_Denominator;
	ini_sd.BufferUsage = m_BufferUsage; // バックバッファの使用方法
	ini_sd.BufferCount = m_BufferCount; // バックバッファの数
	ini_sd.OutputWindow = Window::GetInstance().GetHwnd(); // 関連付けるウインドウ
	ini_sd.Windowed = m_FullScreen ? FALSE : TRUE; // ウィンドウモードかフルスクリーンモードか
	ini_sd.Flags = m_Flags;

	for (UINT driverTypeIndex = 0; driverTypeIndex < m_nuDriverTypes; ++driverTypeIndex)
	{
		m_DriverType = m_DriverTypes[driverTypeIndex];
		m_hr = D3D11CreateDeviceAndSwapChain(
			NULL,								// ディスプレイデバイスのアダプタ（NULLの場合最初に見つかったアダプタ）
			m_DriverType,						// デバイスドライバのタイプ
			NULL,								// ソフトウェアラスタライザを使用する場合に指定する
			m_nuCreateDeviceFlags,				// デバイスフラグ
			m_FeatureLevels,					// 機能レベル
			m_nuFeatureLevels,					// 機能レベル数
			D3D11_SDK_VERSION,
			&ini_sd,							// スワップチェインの設定
			m_cpSwapChain.GetAddressOf(),		// IDXGIDwapChainインタフェース
			m_cpDevice.GetAddressOf(),			// ID3D11Deviceインタフェース
			&m_FeatureLevel,					// サポートされている機能レベル
			m_cpContext.GetAddressOf());		// デバイスコンテキスト
		if (SUCCEEDED(m_hr))
		{
			break;
		}
	}
	if (FAILED(m_hr))
	{
		return m_hr;
	}

	// カリング設定
	D3D11_RASTERIZER_DESC rasterizer = {};
	D3D11_CULL_MODE cull[] = {
		D3D11_CULL_NONE,
		D3D11_CULL_FRONT,
		D3D11_CULL_BACK,
	};
	rasterizer.FillMode = D3D11_FILL_SOLID;
	rasterizer.FrontCounterClockwise = false;
	for (int i = 0; i < 3; i++)
	{
		rasterizer.CullMode = cull[i];
		m_hr = m_cpDevice->CreateRasterizerState(&rasterizer, m_cpRasterizerState[i].GetAddressOf());
		if (FAILED(m_hr)) return m_hr;
	}
	SetCullingMode(D3D11_CULL_BACK);

	// 深度テスト
	// https://tositeru.github.io/ImasaraDX11/part/ZBuffer-and-depth-stencil
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
	dsDesc.BackFace = dsDesc.FrontFace;
	bool enablePattern[DEPTH_MAX] = { true, true, false };
	D3D11_DEPTH_WRITE_MASK maskPattern[DEPTH_MAX] = {
		D3D11_DEPTH_WRITE_MASK_ALL,
		D3D11_DEPTH_WRITE_MASK_ZERO,
		D3D11_DEPTH_WRITE_MASK_ZERO,
	};
	D3D11_COMPARISON_FUNC DepthFunc[DEPTH_MAX] = {
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_ALWAYS,
	};
	for (int i = 0; i < DEPTH_MAX; ++i)
	{
		dsDesc.DepthEnable = dsDesc.StencilEnable = enablePattern[i];
		dsDesc.DepthWriteMask = maskPattern[i];
		dsDesc.DepthFunc = dsDesc.FrontFace.StencilFunc = DepthFunc[i];
		m_hr = m_cpDevice->CreateDepthStencilState(&dsDesc, m_cpDepthStencilState[i].GetAddressOf());
		if (FAILED(m_hr)) { return m_hr; }
	}
	SetDepthTest(DEPTH_ENABLE_WRITE_TEST);

	//ブレンドステートの設定
	// https://pgming-ctrl.com/directx11/blend/
	D3D11_BLEND_DESC BlendDesc = {};
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3D11_BLEND Blend[BLEND_MAX][2] = {
		{D3D11_BLEND_ONE, D3D11_BLEND_ZERO},
		{D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA},
		{D3D11_BLEND_ONE, D3D11_BLEND_ONE},
		{D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE},
		{D3D11_BLEND_ZERO, D3D11_BLEND_INV_SRC_COLOR},
		{D3D11_BLEND_INV_DEST_COLOR, D3D11_BLEND_ONE},
	};
	for (int i = 0; i < BLEND_MAX; i++)
	{
		auto BlendState = m_cpBlendState[i].GetAddressOf();
		BlendDesc.RenderTarget[0].SrcBlend = Blend[i][0];
		BlendDesc.RenderTarget[0].DestBlend = Blend[i][1];
		m_hr = m_cpDevice->CreateBlendState(&BlendDesc, BlendState);
		if (FAILED(m_hr)) return m_hr;
	}
	SetBlendMode(BLEND_ALPHA);

	//サンプラーステートの設定
	D3D11_SAMPLER_DESC SampDesc = {};
	D3D11_FILTER Filter[] = {
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_FILTER_MIN_MAG_MIP_POINT,
	};
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Ｕ(横)方向の繰り返し設定
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Ｖ(縦)方向の繰り返し設定
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Ｗ(奥)方向の繰り返し設定(2Dでも設定が必要
	for (int i = 0; i < SAMPLER_MAX; i++)
	{
		SampDesc.Filter = Filter[i]; // 拡大・縮小を行ったとき、どう補間するか
		m_hr = m_cpDevice->CreateSamplerState(&SampDesc, m_cpSamplerState[i].GetAddressOf());
		if (FAILED(m_hr)) return m_hr;
	}
	SetSamplerState(SAMPLER_LINEAR);

	return m_hr;
}

void DX11_Core::Swap()
{
	// 描画完了時に画面へ出力
	m_cpSwapChain->Present(0, 0);
}

DepthState DX11_Core::GetNowDepthState() const noexcept
{
	ID3D11DepthStencilState *pCurrentState = nullptr;
	UINT stencilRef = 0;
	m_cpContext->OMGetDepthStencilState(&pCurrentState, &stencilRef);
	for (int i = 0; i < DEPTH_MAX; ++i)
	{
		if (pCurrentState == m_cpDepthStencilState[i].Get())
		{
			if (pCurrentState) pCurrentState->Release();
			return static_cast<DepthState>(i);
		}
	}
	if (pCurrentState) pCurrentState->Release();
	return DEPTH_ERROR;
}

void DX11_Core::SetWindowColor(_In_ const float &In_fR, _In_ const float &In_fG, _In_ const float &In_fB, _In_ const float &In_fA) noexcept
{
	m_WindowColor[0] = In_fR;
	m_WindowColor[1] = In_fG;
	m_WindowColor[2] = In_fB;
	m_WindowColor[3] = In_fA;
}

void DX11_Core::SetWindowColor(_In_ const DirectX::XMFLOAT4 &In_arrRgba) noexcept
{
	m_WindowColor[0] = In_arrRgba.x;
	m_WindowColor[1] = In_arrRgba.y;
	m_WindowColor[2] = In_arrRgba.z;
	m_WindowColor[3] = In_arrRgba.w;
}

void DX11_Core::SetWindowColor_R(_In_ const float &In_fR) noexcept
{
	m_WindowColor[0] = In_fR;
}

void DX11_Core::SetWindowColor_G(_In_ const float &In_fG) noexcept
{
	m_WindowColor[1] = In_fG;
}

void DX11_Core::SetWindowColor_B(_In_ const float &In_fB) noexcept
{
	m_WindowColor[2] = In_fB;
}

void DX11_Core::SetWindowColor_A(_In_ const float &In_fA) noexcept
{
	m_WindowColor[3] = In_fA;
}

void DX11_Core::_SetRefreshRate_Numerator(_In_ const UINT &In_unNum) noexcept
{
	m_RefreshRate_Numerator = In_unNum;
}

void DX11_Core::_SetRefreshRate_Denominator(_In_ const UINT &In_unNum) noexcept
{
	m_RefreshRate_Denominator = In_unNum;
}

void DX11_Core::_SetSampleDesc_Count(_In_ const UINT &In_unCount) noexcept
{
	m_SampleDesc_Count = In_unCount;
}

void DX11_Core::_SetFormat(_In_ const DXGI_FORMAT &In_dxgiFormat) noexcept
{
	m_Format = In_dxgiFormat;
}

void DX11_Core::_SetBufferUsage(_In_ const DXGI_USAGE &In_dxgiBufferUsage) noexcept
{
	m_BufferUsage = In_dxgiBufferUsage;
}

void DX11_Core::_SetBufferCount(_In_ const UINT &In_unBufferCount) noexcept
{
	m_BufferCount = In_unBufferCount;
}

void DX11_Core::_SetFullScreen(_In_ const bool &In_bFullScreen) noexcept
{
	m_FullScreen = In_bFullScreen;
}

void DX11_Core::_SetFlags(_In_ const UINT &In_unFlags) noexcept
{
	m_Flags = In_unFlags;
}

void DX11_Core::SetRenderTargets(UINT In_unNum, RenderTarget **In_rtppViews, DepthStencil *In_dspView) noexcept
{
	static ID3D11RenderTargetView *rtvs[4];

	if (In_unNum > 4)
		In_unNum = 4;
	for (UINT i = 0; i < In_unNum; i++)
		rtvs[i] = In_rtppViews[i]->GetView();
	m_cpContext->OMSetRenderTargets(In_unNum, rtvs, In_dspView ? In_dspView->GetView() : nullptr);

	//ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f; // 表示位置X
	vp.TopLeftY = 0.0f; // 表示位置Y
	vp.Width = static_cast<float>(In_rtppViews[0]->GetWidth()); // 表示する幅
	vp.Height = static_cast<float>(In_rtppViews[0]->GetHeight()); // 表示する高さ
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_cpContext->RSSetViewports(1, &vp);
}

void DX11_Core::SetCullingMode(D3D11_CULL_MODE In_cull) noexcept
{
	switch (In_cull)
	{
	case D3D11_CULL_NONE:
		m_cpContext->RSSetState(m_cpRasterizerState[0].Get());
		break;
	case D3D11_CULL_FRONT:
		m_cpContext->RSSetState(m_cpRasterizerState[1].Get());
		break;
	case D3D11_CULL_BACK:
		m_cpContext->RSSetState(m_cpRasterizerState[2].Get());
		break;
	}
}

void DX11_Core::SetDepthTest(DepthState In_State) noexcept
{
	m_cpContext->OMSetDepthStencilState(m_cpDepthStencilState[In_State].Get(), 0);
}

void DX11_Core::SetBlendMode(BlendMode In_Blend) noexcept
{
	if (In_Blend < 0 || In_Blend >= BLEND_MAX)
		return;
	FLOAT BlendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	m_cpContext->OMSetBlendState(m_cpBlendState[In_Blend].Get(), BlendFactor, 0xffffffff);
}

void DX11_Core::SetSamplerState(SamplerState In_State) noexcept
{
	if (In_State < 0 || In_State >= SAMPLER_MAX)
		return;
	auto ptr = m_cpSamplerState[In_State].Get();
	m_cpContext->VSSetSamplers(0, 1, &ptr);
	m_cpContext->PSSetSamplers(0, 1, &ptr);
	m_cpContext->HSSetSamplers(0, 1, &ptr);
	m_cpContext->DSSetSamplers(0, 1, &ptr);
}

void DX11_Core::Change2DMode()
{
	// 2D描画の設定
	auto pRTV = RenderTargetManager::GetInstance().GetDefaultRTV();
	SetDepthTest(DEPTH_DISABLE); // 深度テスト無効
	SetRenderTargets(1, &pRTV, nullptr);
}

void DX11_Core::Change3DMode()
{
	// 3D描画の設定
	auto &RTVManager = RenderTargetManager::GetInstance();
	auto pRTV = RTVManager.GetDefaultRTV();
	auto pDSV = RTVManager.GetDefaultDSV();
	SetRenderTargets(1, &pRTV, pDSV);
	SetDepthTest(DEPTH_ENABLE_WRITE_TEST); // 深度テスト有効
}
