/*+===================================================================
	File: DX11_Initialize.hpp
	Summary: DirectX11の初期化クラス
	Author: AT13C 01 青木雄一郎
	Date: 10/19/2024 Sat AM 12:32:59 初回作成
===================================================================+*/
#pragma once
// ==============================
//	include
// ==============================
#include <dxgidebug.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <DirectXMath.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// COM
#include <wrl/client.h>
#include <initguid.h>
using Microsoft::WRL::ComPtr;

// ==============================
//  マクロ
// ==============================
#define SAFE_RELEASE(ptr) { if(ptr){ptr->Release(); ptr = nullptr;}}

// ==============================
//	前方宣言
// ==============================
class RenderTarget;
class DepthStencil;

// ==============================
//	列挙型定義
// ==============================

/// <summary>
/// ブレンドモード
/// </summary>
enum BlendMode
{
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_ADDALPHA,
	BLEND_SUB,
	BLEND_SCREEN,
	BLEND_MAX
};

/// <summary>
/// サンプラーステート
/// </summary>
enum SamplerState
{
	SAMPLER_LINEAR,
	SAMPLER_POINT,
	SAMPLER_MAX
};

/// <summary>
/// 深度ステート
/// </summary>
enum DepthState
{
	DEPTH_ENABLE_WRITE_TEST,
	DEPTH_ENABLE_TEST,
	DEPTH_DISABLE,
	DEPTH_MAX
};

/// <summary>
/// DirectX初期化クラス
/// </summary>
class DX11_Initialize : public Singleton<DX11_Initialize>
{
	friend class Singleton;
public:
	/// <summary>
	/// DirectX初期化処理
	/// </summary>
	/// <returns>FAILEDマクロで判定</returns>
	HRESULT Init();
	/// <summary>
	/// DirectX終了処理
	/// </summary>
	void Uninit();
	/// <summary>
	/// 描画終了処理
	/// </summary>
	void Swap();

	/// <summary>
	/// ID3D11Deviceを取得する
	/// </summary>
	/// <returns>ID3D11Deviceへのポインター</returns>
	ID3D11Device* GetDevice();
	/// <summary>
	/// ID3D11DeviceContextを取得する
	/// </summary>
	/// <returns>ID3D11DeviceContextへのポインター</returns>
	ID3D11DeviceContext* GetDeviceContext();
	/// <summary>
	/// IDXGISwapChainを取得する
	/// </summary>
	/// <returns>IDXGISwapChainへのポインター</returns>
	IDXGISwapChain *GetSwapChain();

	/// <summary>
	/// 幅を取得する
	/// </summary>
	/// <returns>UINT型</returns>
	UINT GetWidth() const;
	/// <summary>
	/// 高さを取得する
	/// </summary>
	/// <returns>UINT型</returns>
	UINT GetHeight() const;

	/// <summary>
	/// WindowColorをセットする
	/// </summary>
	/// <param name="[In_fR]">レッド</param>
	/// <param name="[In_fG]">グリーン</param>
	/// <param name="[In_fB]">ブルー</param>
	/// <param name="[In_fA]">透過度</param>
	void SetWindowColor(float In_fR, float In_fG, float In_fB, float In_fA);

	/// <summary>
	/// WindowColorをセットする
	/// </summary>
	/// <param name="[In_vecRgba]">vector型の配列※要素数は4限定</param>
	void SetWindowColor(const std::vector<float>&In_vecRgba);
	/// <summary>
	/// WindowColorのレッドの値をセットする
	/// </summary>
	/// <param name="[In_fR]">レッド</param>
	void SetWindowColor_R(float In_fR);
	/// <summary>
	/// WindowColorのグリーンの値をセットする
	/// </summary>
	/// <param nameIn_fGg]">グリーン</param>
	void SetWindowColor_G(float In_fG);
	/// <summary>
	/// WindowColorのブルーの値をセットする
	/// </summary>
	/// <param name="[In_fB]">ブルー</param>
	void SetWindowColor_B(float In_fB);
	/// <summary>
	/// WindowColorの透過度の値をセットする
	/// </summary>
	/// <param name="[In_fA]">透過度</param>
	void SetWindowColor_A(float In_fA);

	/// <summary>
	/// fps値を変更
	/// </summary>
	/// <param name="[In_unNum]">fps値</param>
	void _SetRefreshRate_Numerator(UINT In_unNum);
	/// <summary>
	/// fpsの分母値を変更
	/// </summary>
	/// <param name="[In_unNum]">fpsの分母値</param>
	void _SetRefreshRate_Denominator(UINT In_unNum);
	/// <summary>
	/// マルチサンプル数を変更
	/// </summary>
	/// <param name="[In_unCount]">サンプル数</param>
	void _SetSampleDesc_Count(UINT In_unCount);
	/// <summary>
	/// フレームバッファの色情報(各8bit)を変更
	/// </summary>
	/// <param name="[In_dxgiFormat]">色情報</param>
	void _SetFormat(DXGI_FORMAT In_dxgiFormat);
	/// <summary>
	/// バッファの使用方法を変更
	/// </summary>
	/// <param name="[In_dxgiBufferUsage]">使用方法</param>
	void _SetBufferUsage(DXGI_USAGE In_dxgiBufferUsage);
	/// <summary>
	/// バックバッファ数の変更
	/// </summary>
	/// <param name="[In_unBufferCount]">カウント</param>
	void _SetBufferCount(UINT In_unBufferCount);
	/// <summary>
	/// フルスクリーンの可否を変更
	/// </summary>
	/// <param name="[In_bFullScreen]">true or false</param>
	void _SetFullScreen(bool In_bFullScreen);
	/// <summary>
	/// フラグの設定
	/// </summary>
	/// <param name="[In_unFlags]">フラグ</param>
	void _SetFlags(UINT In_unFlags);

	/// <summary>
	/// レンダーターゲットの設定
	/// </summary>
	/// <param name="[In_unNum]">設定数</param>
	/// <param name="[In_rtppViews]">RenderTargetクラスへのポインター</param>
	/// <param name="[In_dspView]">DepthStencilクラスへのポインター</param>
	void SetRenderTargets(UINT In_unNum, RenderTarget **In_rtppViews, DepthStencil *In_dspView);
	/// <summary>
	/// CullingModeの設定
	/// </summary>
	/// <param name="[In_cull]">モード</param>
	void SetCullingMode(D3D11_CULL_MODE In_cull);
	/// <summary>
	/// DepthTestの設定
	/// </summary>
	/// <param name="[In_State]">有効にするか否か</param>
	void SetDepthTest(DepthState In_State);
	/// <summary>
	/// BlendModeの設定
	/// </summary>
	/// <param name="[In_Blend]">モード</param>
	void SetBlendMode(BlendMode In_Blend);
	/// <summary>
	/// SamplerStateの設定
	/// </summary>
	/// <param name="[In_State]">SamplerState</param>
	void SetSamplerState(SamplerState In_State);

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DX11_Initialize();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~DX11_Initialize() override;

private:
	ComPtr<ID3D11Device> m_cpDevice;							//DirectXでアセットの作成を行う機能
	ComPtr<ID3D11DeviceContext> m_cpContext;					//DirectXで描画命令を発行する機能
	ComPtr<IDXGISwapChain> m_cpSwapChain;						//フレームバッファとディスプレイの橋渡し
	ComPtr<ID3D11RasterizerState> m_cpRasterizerState[3];		//ラスタライザステート
	ComPtr<ID3D11BlendState> m_cpBlendState[BLEND_MAX];			// ブレンドステート
	ComPtr<ID3D11SamplerState> m_cpSamplerState[SAMPLER_MAX];	//テクスチャのサンプリング方法を設定する
	ComPtr<ID3D11DepthStencilState> m_cpDepthStencilState[DEPTH_MAX];

	HRESULT m_hr;	// エラーチェック用

	//パラメーター
	UINT m_Width;						// フレームバッファの幅
	UINT m_Height;						// フレームバッファの高さ
	UINT m_RefreshRate_Numerator;		// fps(分子)
	UINT m_RefreshRate_Denominator;		// fps(分母)
	UINT m_SampleDesc_Count;			// マルチサンプリングの数
	UINT m_SampleDesc_Quality;			// マルチサンプリングの品質
	UINT m_BufferCount;					// バックバッファの数
	UINT m_Flags;
	DXGI_FORMAT m_Format;				// フレームバッファの色情報(各8bit)
	DXGI_USAGE m_BufferUsage;
	bool m_FullScreen;					// フルスクリーンにするかの判定

	//ドライバの種類
	D3D_DRIVER_TYPE m_DriverType;
	D3D_DRIVER_TYPE m_DriverTypes[3];	// 入れる値は使用者側で指定しない
	UINT m_nuDriverTypes;
	//機能レベル
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D_FEATURE_LEVEL m_FeatureLevels[7];	// 入れる値は使用者側で指定しない
	UINT m_nuFeatureLevels;
	UINT m_nuCreateDeviceFlags;

	//ウィンドウの色情報
	float m_WindowColor[4];	// 背景色
	float m_BlendFactor[4];
};