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
#include "DirectX11/DX11_Core.hpp"

class UnorderedAccessView
{
public:
	UnorderedAccessView();
	~UnorderedAccessView();
	HRESULT Create(_In_ const UINT &In_Stride, _In_ const  UINT &In_Num, _In_ void *In_pData = nullptr) noexcept;

	void Copy();

	inline ID3D11UnorderedAccessView *GetUAV() const noexcept { return m_pUAV.Get(); }
	inline ID3D11ShaderResourceView *GetSRV() const noexcept { return m_pSRV.Get(); }

private:
	ComPtr<ID3D11Buffer> m_pBuffer;
	ComPtr<ID3D11UnorderedAccessView> m_pUAV;
	ComPtr< ID3D11ShaderResourceView> m_pSRV;
};
