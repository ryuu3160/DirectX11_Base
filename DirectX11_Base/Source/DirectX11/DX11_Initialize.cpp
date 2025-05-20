/*+===================================================================
	File: DX11_Initialize.hpp
	Summary: DirectX11の初期化クラス
	Author: ryuu3160
	Date: 10/19/2024 Sat AM 12:32:59 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DX11_Initialize.hpp"
#include "Texture/Texture.hpp"

DX11_Initialize::DX11_Initialize() : m_WindowColor{0.8f, 0.9f, 1.0f, 1.0f }, m_BlendFactor{0.0f, 0.0f, 0.0f, 0.0f}
								, m_pRT(nullptr), m_pDS(nullptr)
{
	//nullptrでの初期化
	m_cpDevice = nullptr;
	m_pDebug1 = nullptr;
	m_pDebug2 = nullptr;
	m_cpContext = nullptr;
	m_cpSwapChain = nullptr;
	m_cpRasterizerState[0] = nullptr;
	m_cpRasterizerState[1] = nullptr;
	m_cpRasterizerState[2] = nullptr;

	m_hr = E_FAIL;
	//windowのサイズを取得
	m_Width = Window::GetInstance().GetWidth();
	m_Height = Window::GetInstance().GetHeight();

	m_RefreshRate_Numerator = 60;
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

DX11_Initialize::~DX11_Initialize()
{
	if(m_pDebug1) m_pDebug1->Release();//デバッグ機能の解放
	if (m_pDebug2) m_pDebug2->Release();//デバッグ機能の解放
}

HRESULT DX11_Initialize::Init()
{
	//factoryの作成
	m_hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_cpFactory));
	if (FAILED(m_hr)) { return m_hr; }

	//デバイスとでデバイスコンテキストの作成
	for (UINT Index = 0; Index < m_nuDriverTypes; Index++)
	{
		m_hr = D3D11CreateDevice(nullptr, m_DriverType, nullptr, m_nuCreateDeviceFlags, m_FeatureLevels, m_nuFeatureLevels,
			D3D11_SDK_VERSION, &m_cpDevice, &m_FeatureLevel, &m_cpContext);

		if (SUCCEEDED(m_hr))
		{
			break;
		}
	}
	if (FAILED(m_hr)) { return m_hr; }

	///==========================
	/// スワップチェインの作成
	/// =========================
	DXGI_SWAP_CHAIN_DESC ini_sd;
	ZeroMemory(&ini_sd, sizeof(ini_sd));
	ini_sd.BufferDesc.Width = m_Width; // フレームバッファの幅
	ini_sd.BufferDesc.Height = m_Height; // フレームバッファの高さ
	ini_sd.BufferDesc.Format = m_Format; // フレームバッファの色情報(各8bit)
	//ini_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//ini_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	ini_sd.BufferDesc.RefreshRate.Numerator = m_RefreshRate_Numerator;
	ini_sd.BufferDesc.RefreshRate.Denominator = m_RefreshRate_Denominator;
	ini_sd.SampleDesc.Count = m_SampleDesc_Count; // マルチサンプリングの数
	//ini_sd.SampleDesc.Quality = m_SampleDesc_Quality; // マルチサンプリングの品質
	ini_sd.BufferUsage = m_BufferUsage; // バックバッファの使用方法
	ini_sd.BufferCount = m_BufferCount; // バックバッファの数
	ini_sd.OutputWindow = Window::GetInstance().GetHwnd(); // 関連付けるウインドウ
	ini_sd.Windowed = m_FullScreen ? FALSE : TRUE; // ウィンドウモードかフルスクリーンモードか
	//ini_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	ini_sd.Flags = m_Flags;

	//スワップチェイン作成
	m_hr = m_cpFactory->CreateSwapChain(m_cpDevice.Get(), &ini_sd, &m_cpSwapChain);
	if (FAILED(m_hr)) { return m_hr; }

	//レンダーターゲット設定
	m_pRT = new RenderTarget();
	if (FAILED(m_hr = m_pRT->CreateFromScreen()))
		return m_hr;

	//深度テクスチャ設定
	m_pDS = new DepthStencil();
	if (FAILED(m_hr = m_pDS->Create(m_pRT->GetWidth(), m_pRT->GetHeight(), false)))
		return m_hr;
	SetRenderTargets(1, &m_pRT, nullptr);

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
		m_hr = m_cpDevice->CreateRasterizerState(&rasterizer, &m_cpRasterizerState[i]);
		if (FAILED(m_hr)) return m_hr;
	}
	SetCullingMode(D3D11_CULL_BACK);

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
		BlendDesc.RenderTarget[0].SrcBlend = Blend[i][0];
		BlendDesc.RenderTarget[0].DestBlend = Blend[i][1];
		m_hr = m_cpDevice->CreateBlendState(&BlendDesc, &m_cpBlendState[i]);
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

#ifdef _DEBUG
	// デバッグ機能の設定
	m_hr = m_cpDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&m_pDebug1));
	if (FAILED(m_hr)) return E_FAIL;

	// デバッグ機能の設定(LiveObjects用)
	auto handle = GetModuleHandle("DXGIDebug.dll");
	if (handle)
	{
		auto fun = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(handle, "DXGIGetDebugInterface"));
		if (fun)
		{
			fun(__uuidof(IDXGIDebug), (void **)&m_pDebug2);
		}
	}
#endif // _DEBUG
	
	return m_hr;
}

void DX11_Initialize::Uninit()
{
	if (m_pDS)
	{
		delete m_pDS;
		m_pDS = nullptr;
	}
	if (m_pRT)
	{
		delete m_pRT;
		m_pRT = nullptr;
	}
	for (int i = 0; i < SAMPLER_MAX; i++)
		m_cpSamplerState[i] = nullptr;
	for (int i = 0; i < BLEND_MAX; i++)
		m_cpBlendState[i] = nullptr;
	if(m_cpContext)
		m_cpContext->ClearState();
	if(m_cpSwapChain)
		m_cpSwapChain->SetFullscreenState(false, NULL);
}

void DX11_Initialize::BeginDraw()
{
	// 描画開始時に画面をクリア
	m_pRT->Clear(m_WindowColor);
	m_pDS->Clear();
}
void DX11_Initialize::EndDraw()
{
	// 描画完了時に画面へ出力
	m_cpSwapChain->Present(0, 0);
}

ID3D11Device* DX11_Initialize::GetDevice()
{
	return m_cpDevice.Get();
}

ID3D11DeviceContext* DX11_Initialize::GetDeviceContext()
{
	return m_cpContext.Get();
}

IDXGISwapChain *DX11_Initialize::GetSwapChain()
{
	return m_cpSwapChain.Get();
}

RenderTarget *DX11_Initialize::GetDefaultRTV()
{
	return m_pRT;
}

DepthStencil *DX11_Initialize::GetDefaultDSV()
{
	return m_pDS;
}

UINT DX11_Initialize::GetWidth() const
{
	return m_Width;
}

UINT DX11_Initialize::GetHeight() const
{
	return m_Height;
}

void DX11_Initialize::SetWindowColor(float In_fR, float In_fG, float In_fB, float In_fA)
{
	m_WindowColor[0] = In_fR;
	m_WindowColor[1] = In_fG;
	m_WindowColor[2] = In_fB;
	m_WindowColor[3] = In_fA;
}

void DX11_Initialize::SetWindowColor(const std::vector<float>&In_vecRgba)
{
	if (In_vecRgba.size() != 4)
	{
		throw std::runtime_error("DX11 Init Error : ColorElement count error");
	}

	m_WindowColor[0] = In_vecRgba[0];
	m_WindowColor[1] = In_vecRgba[1];
	m_WindowColor[2] = In_vecRgba[2];
	m_WindowColor[3] = In_vecRgba[3];
}

void DX11_Initialize::SetWindowColor_R(float In_fR)
{
	m_WindowColor[0] = In_fR;
}

void DX11_Initialize::SetWindowColor_G(float In_fG)
{
	m_WindowColor[1] = In_fG;
}

void DX11_Initialize::SetWindowColor_B(float In_fB)
{
	m_WindowColor[2] = In_fB;
}

void DX11_Initialize::SetWindowColor_A(float In_fA)
{
	m_WindowColor[3] = In_fA;
}

void DX11_Initialize::_SetRefreshRate_Numerator(UINT In_unNum)
{
	m_RefreshRate_Numerator = In_unNum;
}

void DX11_Initialize::_SetRefreshRate_Denominator(UINT In_unNum)
{
	m_RefreshRate_Denominator = In_unNum;
}

void DX11_Initialize::_SetSampleDesc_Count(UINT In_unCount)
{
	m_SampleDesc_Count = In_unCount;
}

void DX11_Initialize::_SetFormat(DXGI_FORMAT In_dxgiFormat)
{
	m_Format = In_dxgiFormat;
}

void DX11_Initialize::_SetBufferUsage(DXGI_USAGE In_dxgiBufferUsage)
{
	m_BufferUsage = In_dxgiBufferUsage;
}

void DX11_Initialize::_SetBufferCount(UINT In_unBufferCount)
{
	m_BufferCount = In_unBufferCount;
}

void DX11_Initialize::_SetFullScreen(bool In_bFullScreen)
{
	m_FullScreen = In_bFullScreen;
}

void DX11_Initialize::_SetFlags(UINT In_unFlags)
{
	m_Flags = In_unFlags;
}

void DX11_Initialize::DebugOutput_LiveDeviceObjects()
{
	if (m_pDebug1)
	{
		m_pDebug1->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		OutputDebugStringA(">>>>>>>>> DebugOutput_LiveDeviceObjects\n>>>>>>>>> デバッグ出力完了\n");
	}
	else
	{
		OutputDebugStringA(">>>>>>>>> DebugOutput_LiveDeviceObjects\n>>>>>>>>> デバッグ出力失敗\n");
	}
}

void DX11_Initialize::DebugOutput_LiveObjects()
{
	if (m_pDebug2)
	{
		m_pDebug2->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		OutputDebugStringA(">>>>>>>>> DebugOutput_LiveObjects\n>>>>>>>>> デバッグ出力完了\n");
	}
	else
	{
		OutputDebugStringA(">>>>>>>>> DebugOutput_LiveObjects\n>>>>>>>>> デバッグ出力失敗\n");
	}
}

void DX11_Initialize::SetRenderTargets(UINT In_unNum, RenderTarget **In_rtppViews, DepthStencil *In_dspView)
{
	ID3D11RenderTargetView *rtvs[4];

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

void DX11_Initialize::SetCullingMode(D3D11_CULL_MODE In_cull)
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

void DX11_Initialize::SetDepthTest(bool In_Enable)
{
}

void DX11_Initialize::SetBlendMode(BlendMode In_Blend)
{
	if (In_Blend < 0 || In_Blend >= BLEND_MAX)
		return;
	FLOAT BlendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	m_cpContext->OMSetBlendState(m_cpBlendState[In_Blend].Get(), BlendFactor, 0xffffffff);
}

void DX11_Initialize::SetSamplerState(SamplerState In_State)
{
	if (In_State < 0 || In_State >= SAMPLER_MAX)
		return;
	auto ptr = m_cpSamplerState[In_State].Get();
	m_cpContext->PSSetSamplers(0, 1, &ptr);
}
