/*+===================================================================
	File: MeshBuffer.cpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:55:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MeshBuffer.hpp"

MeshBuffer::MeshBuffer(_In_ const Description &In_Desc) noexcept
	: m_pVtxBuffer(NULL), m_pIdxBuffer(NULL), m_Desc{}
{
	HRESULT hr = E_FAIL;
	hr = CreateVertexBuffer(In_Desc.pVtx, In_Desc.vtxSize, In_Desc.vtxCount, In_Desc.isWrite);
	if (In_Desc.pIdx)
	{
		hr = CreateIndexBuffer(In_Desc.pIdx, In_Desc.idxSize, In_Desc.idxCount);
	}
	m_Desc = In_Desc;

	rsize_t vtxMemSize = In_Desc.vtxSize * In_Desc.vtxCount;
	void *pVtx = new char[vtxMemSize];
	memcpy_s(pVtx, vtxMemSize, In_Desc.pVtx, vtxMemSize);
	m_Desc.pVtx = pVtx;
	rsize_t idxMemSize = In_Desc.idxSize * In_Desc.idxCount;
	void *pIdx = new char[idxMemSize];
	memcpy_s(pIdx, idxMemSize, In_Desc.pIdx, idxMemSize);
	m_Desc.pIdx = pIdx;
}
MeshBuffer::~MeshBuffer()
{
	delete[] m_Desc.pIdx;
	delete[] m_Desc.pVtx;

	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
}

HRESULT MeshBuffer::CreateVertexBuffer(_In_ const void *In_pVtx, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept
{
	//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = In_Size * In_Count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (In_IsWrite)
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	//--- バッファの初期値を設定
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = In_pVtx;

	//--- 頂点バッファの作成
	HRESULT hr;
	ID3D11Device *pDevice = DX11_Initialize::GetInstance().GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, m_pVtxBuffer.GetAddressOf());

	return hr;
}

HRESULT MeshBuffer::CreateIndexBuffer(_In_ const void *In_pIdx, _In_ const UINT &In_Size, _In_ const UINT &In_Count) noexcept
{
	// インデックスサイズの確認
	switch (In_Size)
	{
	default:
		return E_FAIL;
	case 2:
	case 4:
		break;
	}

	// バッファの情報を設定
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = In_Size * In_Count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// バッファの初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = In_pIdx;

	// インデックスバッファ生成
	ID3D11Device *pDevice = DX11_Initialize::GetInstance().GetDevice();
	HRESULT hr;
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, m_pIdxBuffer.GetAddressOf());

	return hr;
}

void MeshBuffer::Draw(_In_ int In_Count) noexcept
{
	ID3D11DeviceContext *pContext = DX11_Initialize::GetInstance().GetDeviceContext();
	UINT stride = m_Desc.vtxSize;
	UINT offset = 0;

	// トポロジの設定
	ID3D11HullShader *hullShader;
	pContext->HSGetShader(&hullShader, nullptr, nullptr);
	if (hullShader)
		hullShader->Release();
	else
		pContext->IASetPrimitiveTopology(m_Desc.topology);

	// 頂点バッファ設定
	pContext->IASetVertexBuffers(0, 1, m_pVtxBuffer.GetAddressOf(), &stride, &offset);

	// 描画
	if (m_Desc.idxCount > 0)
	{
		DXGI_FORMAT format{};
		switch (m_Desc.idxSize)
		{
		case 4: format = DXGI_FORMAT_R32_UINT; break;
		case 2: format = DXGI_FORMAT_R16_UINT; break;
		}
		pContext->IASetIndexBuffer(m_pIdxBuffer.Get(), format, 0);
		pContext->DrawIndexed(In_Count ? In_Count : m_Desc.idxCount, 0, 0);
	}
	else
	{
		// 頂点バッファのみで描画
		pContext->Draw(In_Count ? In_Count : m_Desc.vtxCount, 0);
	}

}

HRESULT MeshBuffer::Write(void *pVtx)
{
	if (!m_Desc.isWrite) { return E_FAIL; }
	DX11_Initialize &Instance = DX11_Initialize::GetInstance();
	HRESULT hr;
	ID3D11Device *pDevice = Instance.GetDevice();
	ID3D11DeviceContext *pContext = Instance.GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー
	hr = pContext->Map(m_pVtxBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = m_Desc.vtxCount * m_Desc.vtxSize;
		memcpy_s(mapResource.pData, size, pVtx, size);
		pContext->Unmap(m_pVtxBuffer.Get(), 0);
	}
	return hr;
}
