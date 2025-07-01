/*+===================================================================
	File: UnorderedAccessView.cpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:47:22 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "UnorderedAccessView.hpp"

UnorderedAccessView::UnorderedAccessView()
    : m_pBuffer(nullptr)
    , m_pUAV(nullptr)
    , m_pSRV(nullptr)
{
}
UnorderedAccessView::~UnorderedAccessView()
{
	// ComPtrは自動的に解放されるため、明示的な解放は不要
    m_pSRV = nullptr;
	m_pUAV = nullptr;
	m_pBuffer = nullptr;
}
HRESULT UnorderedAccessView::Create(UINT stride, UINT num, void *pData)
{
    // https://wizframework.github.io/BaseCross64/15_01.html
    DX11_Initialize &Instance = DX11_Initialize::GetInstance();
    ID3D11Device *pDevice = Instance.GetDevice();
    ID3D11DeviceContext *pContext = Instance.GetDeviceContext();
    HRESULT hr = E_FAIL;

    // バッファ作成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = stride * num;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // ComputeShaderで利用するための構造化バッファ
        desc.StructureByteStride = stride;
        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = pData;
        hr = pDevice->CreateBuffer(&desc, pData ? &data : nullptr, m_pBuffer.GetAddressOf());
        if (FAILED(hr)) { return hr; }
    }

    // アクセスビュー作成
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.NumElements = num;
        hr = pDevice->CreateUnorderedAccessView(m_pBuffer.Get(), &desc, m_pUAV.GetAddressOf());
        if (FAILED(hr)) { return hr; }
    }

    // シェーダーリソースビュー作成
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = num;
        hr = pDevice->CreateShaderResourceView(m_pBuffer.Get(), &desc, m_pSRV.GetAddressOf());
        if (FAILED(hr)) { return hr; }
    }

    return hr;
}

void UnorderedAccessView::Copy()
{
	DX11_Initialize &Instance = DX11_Initialize::GetInstance();
    D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
    D3D11_BUFFER_DESC desc = {};
    D3D11_BUFFER_DESC base;
    m_pBuffer->GetDesc(&base);
    desc.ByteWidth = base.ByteWidth;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.StructureByteStride = base.StructureByteStride;
    ID3D11Buffer *pLocal;
    Instance.GetDevice()->CreateBuffer(&desc, nullptr, &pLocal);
    Instance.GetDeviceContext()->CopyResource(pLocal, m_pBuffer.Get());
    if (SUCCEEDED(Instance.GetDeviceContext()->Map(pLocal, 0, D3D11_MAP_READ, 0, &MappedResource)))
    {
        float check[100];
        memcpy(check, MappedResource.pData, sizeof(check));
        int a = 0;
    }
}

ID3D11UnorderedAccessView *UnorderedAccessView::GetUAV()
{
    return m_pUAV.Get();
}
ID3D11ShaderResourceView *UnorderedAccessView::GetSRV()
{
    return m_pSRV.Get();
}