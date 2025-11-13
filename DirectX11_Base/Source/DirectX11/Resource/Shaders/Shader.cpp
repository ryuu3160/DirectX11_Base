/*+===================================================================
	File: Shader.cpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:39:29 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Shader.hpp"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

//----------
// 基本クラス
Shader::Shader(Kind kind)
	: m_kind(kind)
{
}
Shader::~Shader()
{
	// バッファの解放
	for (auto buf : m_pBuffers)
	{
		if (buf) { buf->Release(); }
	}
	m_pBuffers.clear();
	m_pTextures.clear();
}
HRESULT Shader::Load(_In_ const FilePath &In_FileName) noexcept
{
	HRESULT hr = E_FAIL;

	// ファイルを読み込む
	std::fstream file;
	file.open(In_FileName.data(), std::ios::in | std::ios::binary);
	if (!file.is_open()) { return hr; }

	// ファイルのサイズを調べる
	int fileSize = 0;
	file.seekg(0, std::ios::end);
	fileSize = (int)file.tellg();

	// メモリに読み込み
	file.seekg(0, std::ios::beg);
	char *pData = new char[fileSize];
	file.read(pData, fileSize);
	file.close();

	// シェーダー作成
	hr = Make(pData, fileSize);

	// 終了処理
	if (pData) { delete[] pData; }
	return hr;
}
HRESULT Shader::Compile(_In_ const char *pCode)
{
	static const char *pTargetList[] =
	{
		"vs_5_0",
		"gs_5_0",
		"hs_5_0",
		"ds_5_0",
		"ps_5_0"
	};

	HRESULT hr;
	ID3DBlob *pBlob;
	ID3DBlob *error;
	UINT compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	hr = D3DCompile(pCode, strlen(pCode), nullptr, nullptr, nullptr,
		"main", pTargetList[m_kind], compileFlag, 0, &pBlob, &error);
	if (FAILED(hr)) { return hr; }

	// シェーダ作成
	hr = Make(pBlob->GetBufferPointer(), (UINT)pBlob->GetBufferSize());
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(error);
	return hr;
}

void Shader::WriteBuffer(_In_ const UINT &In_Slot, _In_ void *In_pData) noexcept
{
	if (In_Slot < m_pBuffers.size())
		DX11_Core::GetInstance().GetDeviceContext()->UpdateSubresource(m_pBuffers[In_Slot], 0, nullptr, In_pData, 0, 0);
}
void Shader::SetTexture(_In_ const UINT &In_Slot, _In_opt_ Texture *In_Tex) noexcept
{
	if (In_Slot >= m_pTextures.size()) { return; }
	ID3D11ShaderResourceView *pTex = In_Tex ? In_Tex->GetResource() : nullptr;
	m_pTextures[In_Slot] = pTex;
	DX11_Core &Instance = DX11_Core::GetInstance();
	switch (m_kind)
	{
	case Vertex:	Instance.GetDeviceContext()->VSSetShaderResources(In_Slot, 1, &pTex); break;
	case Pixel:		Instance.GetDeviceContext()->PSSetShaderResources(In_Slot, 1, &pTex); break;
	case Hull:		Instance.GetDeviceContext()->HSSetShaderResources(In_Slot, 1, &pTex); break;
	case Domain:	Instance.GetDeviceContext()->DSSetShaderResources(In_Slot, 1, &pTex); break;
	}
}

HRESULT Shader::Make(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	HRESULT hr;
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();

	// 解析用のリフレクション作成
	ID3D11ShaderReflection *pReflection;
	hr = D3DReflect(In_pData, In_Size, IID_PPV_ARGS(&pReflection));
	if (FAILED(hr)) { return hr; }

	// 定数バッファ作成
	D3D11_SHADER_DESC shaderDesc;
	pReflection->GetDesc(&shaderDesc);
	m_pBuffers.resize(shaderDesc.ConstantBuffers, nullptr);
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		// シェーダーの定数バッファの情報を取得
		D3D11_SHADER_BUFFER_DESC shaderBufDesc;
		ID3D11ShaderReflectionConstantBuffer *cbuf = pReflection->GetConstantBufferByIndex(i);
		cbuf->GetDesc(&shaderBufDesc);

		// 作成するバッファの情報
		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.ByteWidth = shaderBufDesc.Size;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		// バッファの作成
		hr = pDevice->CreateBuffer(&bufDesc, nullptr, &m_pBuffers[i]);
		if (FAILED(hr)) { return hr; }
	}
	// テクスチャ領域作成
	m_pTextures.resize(shaderDesc.BoundResources, nullptr);

	return MakeShader(In_pData, In_Size);
}

//----------
// 頂点シェーダ
VertexShader::VertexShader()
	: Shader(Shader::Vertex)
	, m_pVS(nullptr)
	, m_pInputLayout(nullptr)
{
}

VertexShader::~VertexShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pVS = nullptr;
	m_pInputLayout = nullptr;
}

void VertexShader::Bind(void)
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->VSSetShader(m_pVS.Get(), NULL, 0);
	pContext->IASetInputLayout(m_pInputLayout.Get());
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->VSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->VSSetShaderResources(i, 1, &m_pTextures[i]);
}

HRESULT VertexShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	HRESULT hr;
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();

	// シェーダー作成
	hr = pDevice->CreateVertexShader(In_pData, In_Size, NULL, m_pVS.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	/*
	シェーダ作成時にシェーダリフレクションを通してインプットレイアウトを取得
	セマンティクスの配置などから識別子を作成
	識別子が登録済→再利用、なければ新規作成
	https://blog.techlab-xe.net/dxc-shader-reflection/
	*/

	ID3D11ShaderReflection *pReflection;
	D3D11_SHADER_DESC shaderDesc;
	D3D11_INPUT_ELEMENT_DESC *pInputDesc;
	D3D11_SIGNATURE_PARAMETER_DESC sigDesc;

	DXGI_FORMAT formats[][4] =
	{
		{
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
		}, {
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT_R32G32B32_SINT,
			DXGI_FORMAT_R32G32B32A32_SINT,
		}, {
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		},
	};

	hr = D3DReflect(In_pData, In_Size, IID_PPV_ARGS(&pReflection));
	if (FAILED(hr)) { return hr; }

	pReflection->GetDesc(&shaderDesc);
	pInputDesc = new D3D11_INPUT_ELEMENT_DESC[shaderDesc.InputParameters];
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
	{
		pReflection->GetInputParameterDesc(i, &sigDesc);
		pInputDesc[i].SemanticName = sigDesc.SemanticName;
		pInputDesc[i].SemanticIndex = sigDesc.SemanticIndex;

		// http://marupeke296.com/TIPS_No17_Bit.html
		// 立っているビットの数を数える(要素数を求められる)
		BYTE elementCount = sigDesc.Mask;
		elementCount = (elementCount & 0x05) + ((elementCount >> 1) & 0x05);
		elementCount = (elementCount & 0x03) + ((elementCount >> 2) & 0x03);

		switch (sigDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			pInputDesc[i].Format = formats[0][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			pInputDesc[i].Format = formats[1][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			pInputDesc[i].Format = formats[2][elementCount - 1];
			break;
		}
		pInputDesc[i].InputSlot = 0;
		pInputDesc[i].AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		pInputDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pInputDesc[i].InstanceDataStepRate = 0;
	}

	hr = pDevice->CreateInputLayout(
		pInputDesc, shaderDesc.InputParameters,
		In_pData, In_Size, m_pInputLayout.GetAddressOf()
	);

	delete[] pInputDesc;
	pInputDesc = nullptr;
	return hr;
}

//----------
// インスタンシング頂点シェーダ
InstancedVertexShader::InstancedVertexShader()
	: Shader(Shader::InstancedVertex)
	, m_pVS(nullptr)
	, m_pInputLayout(nullptr)
	, m_pInstanceSRV(nullptr)
{
}

InstancedVertexShader::~InstancedVertexShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pVS = nullptr;
	m_pInstanceSRV = nullptr;
}

void InstancedVertexShader::Bind(void)
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->VSSetShader(m_pVS.Get(), NULL, 0);
	pContext->IASetInputLayout(m_pInputLayout.Get());
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->VSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	// インスタンス用SRVをセット
	if (m_pInstanceSRV)
	{
		pContext->VSSetShaderResources(0, 1, &m_pInstanceSRV);
	}
}

HRESULT InstancedVertexShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	HRESULT hr;
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();

	// シェーダー作成
	hr = pDevice->CreateVertexShader(In_pData, In_Size, NULL, m_pVS.GetAddressOf());
	if (FAILED(hr)) { return hr; }

	/*
	シェーダ作成時にシェーダリフレクションを通してインプットレイアウトを取得
	セマンティクスの配置などから識別子を作成
	識別子が登録済→再利用、なければ新規作成
	https://blog.techlab-xe.net/dxc-shader-reflection/
	*/

	ID3D11ShaderReflection *pReflection;
	D3D11_SHADER_DESC shaderDesc;
	D3D11_INPUT_ELEMENT_DESC *pInputDesc;
	D3D11_SIGNATURE_PARAMETER_DESC sigDesc;

	DXGI_FORMAT formats[][4] =
	{
		{
			DXGI_FORMAT_R32_UINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32_UINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
		}, {
			DXGI_FORMAT_R32_SINT,
			DXGI_FORMAT_R32G32_SINT,
			DXGI_FORMAT_R32G32B32_SINT,
			DXGI_FORMAT_R32G32B32A32_SINT,
		}, {
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R32G32B32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
		},
	};

	hr = D3DReflect(In_pData, In_Size, IID_PPV_ARGS(&pReflection));
	if (FAILED(hr)) { return hr; }

	pReflection->GetDesc(&shaderDesc);
	pInputDesc = new D3D11_INPUT_ELEMENT_DESC[shaderDesc.InputParameters];
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
	{
		pReflection->GetInputParameterDesc(i, &sigDesc);
		pInputDesc[i].SemanticName = sigDesc.SemanticName;
		pInputDesc[i].SemanticIndex = sigDesc.SemanticIndex;

		// http://marupeke296.com/TIPS_No17_Bit.html
		// 立っているビットの数を数える(要素数を求められる)
		BYTE elementCount = sigDesc.Mask;
		elementCount = (elementCount & 0x05) + ((elementCount >> 1) & 0x05);
		elementCount = (elementCount & 0x03) + ((elementCount >> 2) & 0x03);

		switch (sigDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			pInputDesc[i].Format = formats[0][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			pInputDesc[i].Format = formats[1][elementCount - 1];
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			pInputDesc[i].Format = formats[2][elementCount - 1];
			break;
		}
		pInputDesc[i].InputSlot = 0;
		pInputDesc[i].AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		pInputDesc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pInputDesc[i].InstanceDataStepRate = 0;
	}

	hr = pDevice->CreateInputLayout(
		pInputDesc, shaderDesc.InputParameters,
		In_pData, In_Size, m_pInputLayout.GetAddressOf()
	);

	delete[] pInputDesc;
	return hr;
}

//----------
// ピクセルシェーダ
PixelShader::PixelShader()
	: Shader(Shader::Pixel)
	, m_pPS(nullptr)
{
}
PixelShader::~PixelShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pPS = nullptr;
}
void PixelShader::Bind(void)
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->PSSetShader(m_pPS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->PSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->PSSetShaderResources(i, 1, &m_pTextures[i]);
}
HRESULT PixelShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	return DX11_Core::GetInstance().GetDevice()->CreatePixelShader(In_pData, In_Size, NULL, m_pPS.GetAddressOf());
}

//----------
// ハルシェーダ
HullShader::HullShader()
	: Shader(Shader::Hull)
	, m_pHS(nullptr)
{
}
HullShader::~HullShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pHS = nullptr;
}
void HullShader::Bind(void)
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->HSSetShader(m_pHS.Get(), nullptr, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->HSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->HSSetShaderResources(i, 1, &m_pTextures[i]);
}
void HullShader::Unbind(void)
{
	DX11_Core::GetInstance().GetDeviceContext()->HSSetShader(nullptr, nullptr, 0);
}
HRESULT HullShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	return DX11_Core::GetInstance().GetDevice()->CreateHullShader(In_pData, In_Size, NULL, m_pHS.GetAddressOf());
}

//----------
// ドメインシェーダ
DomainShader::DomainShader()
	: Shader(Shader::Domain)
	, m_pDS(nullptr)
{
}
DomainShader::~DomainShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pDS = nullptr;
}
void DomainShader::Bind(void)
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->DSSetShader(m_pDS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->DSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->DSSetShaderResources(i, 1, &m_pTextures[i]);
}
void DomainShader::Unbind(void)
{
	DX11_Core::GetInstance().GetDeviceContext()->DSSetShader(nullptr, nullptr, 0);
}
HRESULT DomainShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	return DX11_Core::GetInstance().GetDevice()->CreateDomainShader(In_pData, In_Size, NULL, m_pDS.GetAddressOf());
}

//----------
// ジオメトリシェーダー
GeometryShader::GeometryShader()
	: Shader(Shader::Geometry)
	, m_pGS(nullptr)
{
}
GeometryShader::~GeometryShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pGS = nullptr;
}
void GeometryShader::Bind()
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->GSSetShader(m_pGS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->GSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->GSSetShaderResources(i, 1, &m_pTextures[i]);
}
void GeometryShader::Unbind()
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	ID3D11ShaderResourceView *pSRV = nullptr;
	pContext->GSSetShaderResources(0, 1, &pSRV);
	pContext->GSSetShader(nullptr, nullptr, 0);
}
HRESULT GeometryShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	return DX11_Core::GetInstance().GetDevice()->CreateGeometryShader(In_pData, In_Size, NULL, m_pGS.GetAddressOf());
}

//----------
// コンピュートシェーダー
ComputeShader::ComputeShader()
	: Shader(Shader::Compute)
	, m_pCS(nullptr)
{
}
ComputeShader::~ComputeShader()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pCS = nullptr;
}
void ComputeShader::Bind()
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	pContext->CSSetShader(m_pCS.Get(), nullptr, 0);
	for (int i = 0; i < m_pBuffers.size(); ++i)
		pContext->CSSetConstantBuffers(i, 1, &m_pBuffers[i]);
	for (int i = 0; i < m_pTextures.size(); ++i)
		pContext->CSSetShaderResources(i, 1, &m_pTextures[i]);
}
void ComputeShader::Unbind()
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	ID3D11UnorderedAccessView *pUAV = nullptr;
	ID3D11ShaderResourceView *pSRV = nullptr;

	pContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);
	pContext->CSSetShaderResources(0, 1, &pSRV);
	pContext->CSSetShader(nullptr, nullptr, 0);
}
void ComputeShader::Dispatch(_In_ UINT In_Num, _In_ UnorderedAccessView **ppUAV, _In_ const UINT &In_Thread) noexcept
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	ID3D11UnorderedAccessView *ptr[4];
	if (In_Num > 4) In_Num = 4;
	for (unsigned int i = 0; i < In_Num; ++i)
		ptr[i] = ppUAV[i]->GetUAV();
	pContext->CSSetUnorderedAccessViews(0, In_Num, ptr, nullptr);
	pContext->Dispatch(In_Thread, 1, 1);
}
HRESULT ComputeShader::MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept
{
	return DX11_Core::GetInstance().GetDevice()->CreateComputeShader(In_pData, In_Size, NULL, m_pCS.GetAddressOf());
}
