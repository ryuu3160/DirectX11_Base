/*+===================================================================
	File: RenderTargetManager.hpp
	Summary: RTV/DSV管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/12 Wed AM 04:21:32 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"
// ==============================
//	前方宣言
// ==============================
class RenderTarget;
class DepthStencil;

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// RenderTargetManagerクラス
/// </summary>
class RenderTargetManager : public Singleton<RenderTargetManager>
{
	friend class Singleton<RenderTargetManager>;
public:
	RenderTargetManager();
	~RenderTargetManager();

	/// <summary>
	/// 指定した幅と高さで既定のレンダーターゲットと深度ステンシルリソースを初期化します。
	/// </summary>
	/// <param name="[In_Width]">作成するリソースの幅</param>
	/// <param name="[In_Height]">作成するリソースの高さ</param>
	/// <param name="[In_UseStencil]">深度ステンシルリソースにステンシルバッファを使用するかどうかを示すブール値</param>
	/// <returns>作成されたRenderTargetとDepthStencilのペア。</returns>
	std::pair<RenderTarget*,DepthStencil*> InitializeDefaultResources(_In_ UINT In_Width, _In_ UINT In_Height, _In_ bool In_UseStencil = false);

	RenderTarget *GetDefaultRTV() const noexcept { return m_pDefaultRTV; }
	DepthStencil *GetDefaultDSV() const noexcept { return m_pDefaultDSV; }

	RenderTarget *CreateRenderTarget(_In_ const std::string &In_Name, _In_ DXGI_FORMAT In_Format, _In_ UINT In_Width, _In_ UINT In_Height);

	DepthStencil *CreateDepthStencil(_In_ const std::string &In_Name, _In_ UINT In_Width, _In_ UINT In_Height, _In_ bool In_UseStencil);

private:
	RenderTarget* m_pDefaultRTV;	// デフォルトのRTV
	DepthStencil* m_pDefaultDSV;	// デフォルトのDSV

	std::unordered_map<std::string, RenderTarget*> m_RenderTargets;	// RenderTargetのマップ
	std::unordered_map<std::string, DepthStencil*> m_DepthStencils;	// DepthStencilのマップ
};
