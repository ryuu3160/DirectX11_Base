/*+===================================================================
	File: MeshBuffer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT12X192 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:55:36 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DX11_Initialize.hpp"

class MeshBuffer
{
public:
	struct Description
	{
		const void *pVtx;
		UINT vtxSize;
		UINT vtxCount;
		bool isWrite;
		const void *pIdx;
		UINT idxSize;
		UINT idxCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
	};
public:
	MeshBuffer(const Description &desc);
	~MeshBuffer();

	void Draw(int count = 0);
	HRESULT Write(void *pVtx);

	Description GetDesc();

private:
	HRESULT CreateVertexBuffer(const void *pIdx, UINT size, UINT count, bool isWrite);
	HRESULT CreateIndexBuffer(const void *pVtx, UINT size, UINT count);

private:
	ID3D11Buffer *m_pVtxBuffer;
	ID3D11Buffer *m_pIdxBuffer;
	Description m_desc;
};
