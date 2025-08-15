/*+===================================================================
	File: Texture.hpp
	Summary: Texture関連クラス
	Author: AT13C 01 青木雄一郎
	Date: 10/19/2024 Sat AM 01:20:06 初回作成
===================================================================+*/
#pragma once

// ===============================
//	include
// ===============================
#include "DirectX11/DX11_Initialize.hpp"

// ===============================
//	前方宣言
// ===============================
struct aiTexture;

/// <summary>
/// テクスチャ
/// </summary>
class Texture
{
public:
	Texture();
	virtual ~Texture();
	/// <summary>
	/// 指定されたファイルパスでオブジェクトを作成します。
	/// </summary>
	/// <param name="[In_FileName]">作成に使用するファイルパス。</param>
	/// <returns>操作が成功した場合はS_OK、失敗した場合はエラーコードを返します。</returns>
	HRESULT Create(_In_ const FilePath &In_FileName) noexcept;

	/// <summary>
	/// 埋め込みテクスチャの場合のオブジェクト作成関数
	/// </summary>
	/// <param name="[In_aiTex]">aiTextureオブジェクトへのポインタ</param>
	/// <returns>操作が成功した場合はS_OK、失敗した場合はエラーコードを返します。</returns>
	HRESULT Create(_In_ const aiTexture *In_aiTex) noexcept;

	/// <summary>
	/// 指定されたフォーマット、幅、高さ、およびデータでオブジェクトを作成します。
	/// </summary>
	/// <param name="[In_Format]">作成するオブジェクトのDXGIフォーマット。</param>
	/// <param name="[In_Width]">オブジェクトの幅（ピクセル単位）。</param>
	/// <param name="[In_Height]">オブジェクトの高さ（ピクセル単位）。</param>
	/// <param name="[In_pData]">初期化に使用するデータへのポインタ。nullptrの場合はデータなし。</param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	HRESULT Create(_In_ DXGI_FORMAT In_Format, _In_ const UINT In_Width, _In_ const UINT In_Height, _In_ const void *In_pData = nullptr) noexcept;

	inline const UINT &GetWidth() const noexcept { return m_width; }
	inline const UINT &GetHeight() const noexcept { return m_height; }
	inline ID3D11ShaderResourceView *GetResource() const noexcept { return m_pSRV.Get(); }

protected:
	/// <summary>
	/// 指定されたフォーマット、幅、高さから D3D11 テクスチャ2Dの記述子を作成します。
	/// </summary>
	/// <param name="[In_Format]">テクスチャの DXGI フォーマット。</param>
	/// <param name="[In_Width]">テクスチャの幅（ピクセル単位）。</param>
	/// <param name="[In_Height]">テクスチャの高さ（ピクセル単位）。</param>
	/// <returns>指定されたパラメータに基づく D3D11_TEXTURE2D_DESC 構造体。</returns>
	D3D11_TEXTURE2D_DESC MakeTexDesc(_In_ DXGI_FORMAT In_Format, _In_ const UINT &In_Width, _In_ const UINT &In_Height) noexcept;
	/// <summary>
	/// D3D11_TEXTURE2D_DESC とデータを使用してテクスチャリソースを作成します。
	/// </summary>
	/// <param name="[In_Desc]"></param>
	/// <param name="[In_pData]"></param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	virtual HRESULT CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData) noexcept;

protected:
	UINT m_width;	///< 横幅
	UINT m_height;	///< 縦幅
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	ComPtr<ID3D11Texture2D> m_pTex;
};

/// <summary>
/// レンダーターゲット
/// </summary>
class RenderTarget : public Texture
{
public:
	RenderTarget();
	~RenderTarget();
	/// <summary>
	/// レンダーターゲットをクリアします。
	/// </summary>
	void Clear();
	/// <summary>
	/// 指定された色でレンダーターゲットをクリアします。
	/// </summary>
	/// <param name="[In_Color]">色データ</param>
	void Clear(_In_ const float *In_Color) noexcept;

	/// <summary>
	/// 指定されたフォーマット、幅、高さでレンダーターゲットを作成します。
	/// </summary>
	/// <param name="[In_Format]"></param>
	/// <param name="[In_Width]"></param>
	/// <param name="[In_Height]"></param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	HRESULT Create(_In_ DXGI_FORMAT In_Format, _In_ const UINT &In_Width, _In_ const UINT &In_Height) noexcept;
	/// <summary>
	/// スクリーンからレンダーターゲットを作成します。
	/// </summary>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	HRESULT CreateFromScreen() noexcept;
	/// <summary>
	/// レンダーターゲットビューを取得します。
	/// </summary>
	/// <returns>ID3D11RenderTargetViewのポインタ。</returns>
	inline ID3D11RenderTargetView *GetView() const noexcept { return m_pRTV.Get(); }

protected:
	/// <summary>
	/// D3D11_TEXTURE2D_DESC とデータを使用してレンダーターゲットリソースを作成します。
	/// </summary>
	/// <param name="[In_Desc]"></param>
	/// <param name="[In_pData]"></param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	virtual HRESULT CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData = nullptr) noexcept override;

private:
	ComPtr<ID3D11RenderTargetView> m_pRTV;
};

/// <summary>
/// 深度テクスチャ
/// </summary>
class DepthStencil : public Texture
{
public:
	DepthStencil();
	~DepthStencil();
	/// <summary>
	/// 深度ステンシルビューをクリアします。
	/// </summary>
	void Clear();

	/// <summary>
	/// 指定された幅、高さ、およびステンシル使用フラグで深度ステンシルビューを作成します。
	/// </summary>
	/// <param name="[In_Width]"></param>
	/// <param name="[In_Height]"></param>
	/// <param name="[In_UseStencil]"></param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	HRESULT Create(_In_ const UINT &In_Width, _In_ const UINT &In_Height, _In_ const bool &In_UseStencil) noexcept;
	/// <summary>
	/// 深度ステンシルビューを取得します。
	/// </summary>
	/// <returns></returns>
	inline ID3D11DepthStencilView *GetView() const noexcept { return m_pDSV.Get(); }

protected:
	/// <summary>
	/// D3D11_TEXTURE2D_DESC とデータを使用して深度ステンシルリソースを作成します。
	/// </summary>
	/// <param name="[In_Desc]"></param>
	/// <param name="[In_pData]"></param>
	/// <returns>操作が成功したかどうかを示すHRESULT値。</returns>
	virtual HRESULT CreateResource(_In_ D3D11_TEXTURE2D_DESC &In_Desc, const void *In_pData = nullptr) noexcept override;

private:
	ComPtr<ID3D11DepthStencilView> m_pDSV;
};
