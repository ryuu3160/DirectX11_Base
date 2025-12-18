/*+===================================================================
	File: MeshBuffer.hpp
	Summary: メッシュバッファクラス 佐々木先生のものを参考にして作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:55:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/DX11_Core.hpp"

class MeshBuffer
{
public:
	/// <summary>
	/// 頂点およびインデックスバッファの情報を格納する構造体
	/// </summary>
	struct Description
	{
		const void *pVtx = nullptr;
		UINT vtxSize;
		UINT vtxCount;
		bool isWrite;
		const void *pIdx = nullptr;
		UINT idxSize;
		UINT idxCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
public:
	MeshBuffer(_In_ const Description &In_Desc) noexcept;
	~MeshBuffer();

	void Draw(_In_ int In_Count = 0) noexcept;

	/// <summary>
	/// 頂点データを書き込みます
	/// </summary>
	/// <param name="pVtx">書き込む頂点データへのポインタ</param>
	/// <returns>操作の成功または失敗を示すHRESULT値</returns>
	HRESULT Write(_In_ void *In_pVtx) noexcept;

	/// <summary>
	/// 頂点及びインデックスバッファの情報を取得します
	/// </summary>
	/// <returns>頂点及びインデックスバッファの情報を格納したDescription構造体</returns>
	inline Description GetDesc() const noexcept { return m_Desc; }

	/// <summary>
	/// 指定された記述子に基づいてバッファを再作成します
	/// </summary>
	/// <param name="In_Desc">バッファの再作成に使用される記述子</param>
	/// <returns>操作の成功または失敗を示すHRESULT値</returns>
	HRESULT RemakeBuffer(_In_opt_ void *In_pVtx, _In_ const int In_VtxCount, _In_opt_ void *In_pIdx = nullptr, _In_ const int In_IdxCount = 0) noexcept;

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

private:
	ComPtr<ID3D11Buffer> m_pVtxBuffer;
	ComPtr<ID3D11Buffer> m_pIdxBuffer;
	Description m_Desc;
};
