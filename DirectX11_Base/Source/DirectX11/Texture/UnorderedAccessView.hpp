/*+===================================================================
	File: UnorderedAccessView.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:47:15 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../DX11_Initialize.hpp"

class UnorderedAccessView
{
public:
	UnorderedAccessView();
	~UnorderedAccessView();
	HRESULT Create(UINT stride, UINT num, void *pData = nullptr);

	void Copy();

	ID3D11UnorderedAccessView *GetUAV();
	ID3D11ShaderResourceView *GetSRV();

private:
	ID3D11Buffer *m_pBuffer;
	ID3D11UnorderedAccessView *m_pUAV;
	ID3D11ShaderResourceView *m_pSRV;
};
