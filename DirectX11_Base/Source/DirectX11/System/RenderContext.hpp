/*+===================================================================
	File: RenderContext.hpp
	Summary: RenderContextクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/12 Wed AM 07:43:24 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	前方宣言
// ==============================
class Camera;
class RenderTarget;
class DepthStencil;

/// <summary>
/// RenderContext構造体
/// </summary>
class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Create(_In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept;

	/// <summary>
	/// 行列を再計算します
	/// </summary>
	void ReCalculateMatrices() noexcept;

	DirectX::XMFLOAT4X4 GetView(_In_ bool In_Transpose = true) const noexcept;
	DirectX::XMFLOAT4X4 GetProj(_In_ bool In_Transpose = true) const noexcept;
	DirectX::XMFLOAT4X4 Get2DProj(_In_ bool In_Transpose = true) const noexcept;
	inline Camera *GetCamera() const noexcept { return m_pCamera; }
	inline RenderTarget *GetRTV() noexcept { return m_pRTV; }
	inline DepthStencil *GetDSV() noexcept { return m_pDSV; }

	inline bool IsMainContext() const noexcept { return m_bIsMainContext; }

private:
	DirectX::XMFLOAT4X4 m_ViewMatrix{};
	DirectX::XMFLOAT4X4 m_ProjectionMatrix{};
	DirectX::XMFLOAT4X4 m_2DProjMatrix{};
	Camera *m_pCamera;
	RenderTarget *m_pRTV;
	DepthStencil *m_pDSV;

	bool m_bIsMainContext;
	static bool m_bIsExistMainContext;
};
