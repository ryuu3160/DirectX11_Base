/*+===================================================================
	File: InstancedMeshBuffer.cpp
	Summary: インスタンシング用のメッシュバッファクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 01:42:56 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InstancedMeshBuffer.hpp"

InstancedMeshBuffer::InstancedMeshBuffer(_In_ const InstancingDesc &In_Desc) noexcept
	: m_pVtxBuffer(nullptr), m_pIdxBuffer(nullptr), m_Desc{}
{
	HRESULT hr = E_FAIL;
	// 頂点バッファとインデックスバッファの作成
	hr = CreateVertexBuffer(In_Desc.pVtx, In_Desc.vtxSize, In_Desc.vtxCount, In_Desc.isWrite);
	if (In_Desc.pIdx)
	{
		hr = CreateIndexBuffer(In_Desc.pIdx, In_Desc.idxSize, In_Desc.idxCount);
	}
	// インスタンスバッファとインスタンシング用のSRVの作成
	hr = CreateInstance(In_Desc.pInstance, In_Desc.instanceSize, In_Desc.instanceCount, In_Desc.isInstanceWrite);

	m_Desc = In_Desc;

	// 頂点とインデックスの情報をコピー
	rsize_t vtxMemSize = In_Desc.vtxSize * In_Desc.vtxCount;
	void *pVtx = new char[vtxMemSize];
	memcpy_s(pVtx, vtxMemSize, In_Desc.pVtx, vtxMemSize);
	m_Desc.pVtx = pVtx;
	rsize_t idxMemSize = In_Desc.idxSize * In_Desc.idxCount;
	void *pIdx = new char[idxMemSize];
	memcpy_s(pIdx, idxMemSize, In_Desc.pIdx, idxMemSize);
	m_Desc.pIdx = pIdx;
}

InstancedMeshBuffer::~InstancedMeshBuffer()
{
	delete[] m_Desc.pIdx;
	delete[] m_Desc.pVtx;

	// ComPtrは自動的に解放されるため、明示的な解放は不要
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
	m_pInstanceSRV = nullptr;
	m_pInstanceBuffer = nullptr;
}

void InstancedMeshBuffer::Draw(_In_ int In_Count) noexcept
{
	ID3D11DeviceContext *pContext = DX11_Core::GetInstance().GetDeviceContext();
	UINT stride = m_Desc.vtxSize;
	UINT offset = 0;

	// ハルシェーダーが設定されている場合は、プリミティブトポロジを設定しない
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
		pContext->DrawIndexedInstanced(In_Count ? In_Count : m_Desc.idxCount, m_Desc.instanceCount, 0, 0, 0);
	}
	else
	{
		// 頂点バッファのみで描画
		pContext->DrawInstanced(In_Count ? In_Count : m_Desc.vtxCount, m_Desc.instanceCount, 0, 0);
	}
}

HRESULT InstancedMeshBuffer::Write(_In_ void *In_pVtx) noexcept
{
	return E_NOTIMPL;
}

HRESULT InstancedMeshBuffer::CreateVertexBuffer(_In_ const void *In_pVtx, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept
{
	//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = In_Size * In_Count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// UsageとCPUAccessFlagsの設定
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
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, m_pVtxBuffer.GetAddressOf());

	return hr;
}

HRESULT InstancedMeshBuffer::CreateIndexBuffer(_In_ const void *In_pIdx, _In_ const UINT &In_Size, _In_ const UINT &In_Count) noexcept
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
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();
	HRESULT hr;
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, m_pIdxBuffer.GetAddressOf());

	return hr;
}

HRESULT InstancedMeshBuffer::CreateInstance(_In_ const void *In_pInstance, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept
{
	HRESULT hr = E_FAIL;

	// インスタンスバッファの作成
	hr = CreateInstanceBuffer(In_pInstance, In_Size, In_Count, In_IsWrite);

	if (FAILED(hr))
		return hr;

	// インスタンス用のSRVの作成
	hr = CreateInstanceSRV(In_Count);

	return hr;
}

HRESULT InstancedMeshBuffer::CreateInstanceBuffer(_In_ const void *In_pInstance, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept
{
	//インスタンスデータ用バッファの設定
	D3D11_BUFFER_DESC bd_instance;
	bd_instance.Usage = D3D11_USAGE_DYNAMIC;
	bd_instance.ByteWidth = In_Size * In_Count;
	bd_instance.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd_instance.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_instance.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd_instance.StructureByteStride = In_Size;

	//--- バッファの初期値を設定
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = In_pInstance;

	// インスタンスデータ用バッファの作成
	HRESULT hr;
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();
	hr = pDevice->CreateBuffer(&bd_instance, &subResource, &m_pInstanceBuffer);

	return hr;
}

HRESULT InstancedMeshBuffer::CreateInstanceSRV(_In_ const UINT &In_Count) noexcept
{
	HRESULT hr;
	if(!m_pInstanceBuffer)
		return E_FAIL;

	//インスタンス用のリソースビューを作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.BufferEx.NumElements = In_Count;

	// インスタンス用のリソースビューを作成
	ID3D11Device *pDevice = DX11_Core::GetInstance().GetDevice();
	hr = pDevice->CreateShaderResourceView(m_pInstanceBuffer.Get(), &srvDesc, &m_pInstanceSRV);

	return hr;
}
