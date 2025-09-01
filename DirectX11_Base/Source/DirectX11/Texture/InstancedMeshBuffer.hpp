/*+===================================================================
	File: InstancedMeshBuffer.hpp
	Summary: インスタンシング用のメッシュバッファクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 01:42:49 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/DX11_Initialize.hpp"

/// <summary>
/// InstancedMeshBufferクラス
/// </summary>
class InstancedMeshBuffer
{
public:
	
	/// <summary>
	/// 頂点およびインデックスバッファの情報とインスタンシング用の情報を格納する構造体
	/// </summary>
	struct InstancingDesc
	{
		const void *pVtx;
		UINT vtxSize;
		UINT vtxCount;
		bool isWrite;
		const void *pIdx;
		UINT idxSize;
		UINT idxCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
		// 以下、インスタンシング用の情報
		const void *pInstance;      // インスタンスデータ（ポインタ）
		UINT instanceSize;          // インスタンス1個分のサイズ
		UINT instanceCount;         // インスタンスの個数
		bool isInstanceWrite;       // インスタンスバッファの書き込み可否
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	InstancedMeshBuffer(_In_ const InstancingDesc &In_Desc) noexcept;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InstancedMeshBuffer();

	void Draw(_In_ int In_Count = 0) noexcept;

	/// <summary>
	/// 頂点データを書き込みます。
	/// </summary>
	/// <param name="pVtx">書き込む頂点データへのポインタ。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT Write(_In_ void *In_pVtx) noexcept;

	/// <summary>
	/// 頂点及びインデックスバッファの情報を取得します。
	/// </summary>
	/// <returns>頂点及びインデックスバッファの情報を格納したDescription構造体。</returns>
	inline InstancingDesc GetDesc() const noexcept { return m_Desc; }
private:

	/// <summary>
	/// 頂点バッファを作成します。
	/// </summary>
	/// <param name="[In_pVtx]">頂点データへのポインタ。</param>
	/// <param name="[In_Size]">バッファのサイズ（バイト単位）。</param>
	/// <param name="[In_Count]">頂点の数。</param>
	/// <param name="[In_IsWrite]">バッファが書き込み可能かどうかを示すフラグ。</param>
	/// <returns>操作の成否を示すHRESULT値。</returns>
	HRESULT CreateVertexBuffer(_In_ const void *In_pVtx, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept;
	/// <summary>
	/// インデックスバッファを作成します。
	/// </summary>
	/// <param name="[In_pIdx]">インデックスデータへのポインタ。</param>
	/// <param name="[In_Size]">インデックスバッファのサイズ（バイト単位）。</param>
	/// <param name="[In_Count]">インデックスの数。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT CreateIndexBuffer(_In_ const void *In_pIdx, _In_ const UINT &In_Size, _In_ const UINT &In_Count) noexcept;

	/// <summary>
	/// 指定されたインスタンス情報を使用して新しいインスタンスを作成します。
	/// </summary>
	/// <param name="[In_pInstance]">インスタンスデータへのポインタ。</param>
	/// <param name="[In_Size]">インスタンスのサイズ（バイト単位）。</param>
	/// <param name="[In_Count]">インスタンスの数。</param>
	/// <param name="[In_IsWrite]">書き込み可能かどうかを示すフラグ。</param>
	/// <returns>操作の成功または失敗を示す HRESULT コード。</returns>
	HRESULT CreateInstance(_In_ const void *In_pInstance, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept;

	/// <summary>
	/// インスタンスデータ用のバッファを作成します。
	/// </summary>
	/// <param name="[In_pInstance]">インスタンスデータへのポインタ。</param>
	/// <param name="[In_Size]">各インスタンスのサイズ（バイト単位）。</param>
	/// <param name="[In_Count]">インスタンスの数。</param>
	/// <param name="[In_IsWrite]">バッファが書き込み可能かどうかを示すフラグ。</param>
	/// <returns>操作の成功または失敗を示す HRESULT コード。</returns>
	HRESULT CreateInstanceBuffer(_In_ const void *In_pInstance, _In_ const UINT &In_Size, _In_ const UINT &In_Count, _In_ const bool &In_IsWrite) noexcept;

	/// <summary>
	/// 指定されたサイズとカウントに基づいて、インスタンスのシェーダーリソースビュー (SRV) を作成します。
	/// </summary>
	/// <param name="[In_Count]">作成するインスタンスの数。</param>
	/// <returns>操作の成功または失敗を示す HRESULT コード。</returns>
	HRESULT CreateInstanceSRV(_In_ const UINT &In_Count) noexcept;

private:
	ComPtr<ID3D11Buffer> m_pVtxBuffer;
	ComPtr<ID3D11Buffer> m_pIdxBuffer;
	ComPtr<ID3D11Buffer> m_pInstanceBuffer;
	ComPtr<ID3D11ShaderResourceView> m_pInstanceSRV;
	InstancingDesc m_Desc;
};
