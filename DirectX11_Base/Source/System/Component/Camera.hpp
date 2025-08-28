/*+===================================================================
	File: Camera.hpp
	Summary: カメラのコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/03 Thu PM 02:11:00 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Component.hpp"

// ==============================
//  定数
// ==============================
namespace
{
	const inline constexpr DirectX::XMFLOAT3 cx_ThirdPerson_TargetOffset = { 0.0f,2.0f,0.0f }; // 3人称視点カメラの注視点オフセット
	const inline constexpr DirectX::XMFLOAT3 cx_ThirdPerson_Offset = { 0.0f,3.0f,0.0f }; // 3人称視点カメラのオフセット
	const inline constexpr float cx_ThirdPerson_Distance = 7.5f; // 3人称視点カメラの注視点からカメラまでの距離
}

/// <summary>
/// Cameraクラス
/// </summary>
class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	void ReadWrite(_In_ DataAccessor *In_Data) final;
#if _DEBUG
	void Draw() const noexcept;
#endif

	DirectX::XMFLOAT4X4 GetView(_In_ bool In_Transpose = true) const noexcept;
	DirectX::XMFLOAT4X4 GetProj(_In_ bool In_Transpose = true) const noexcept;

	/// <summary>
	/// カメラが3Dかどうかを取得
	/// </summary>
	/// <returns>3Dかどうかのフラグ</returns>
	inline const bool &Is3D() const noexcept { return m_bIs3D; }
	
	/// <summary>
	/// カメラの視野角を取得
	/// </summary>
	/// <returns>視野角</returns>
	inline float GetFovy() const noexcept {return DirectX::XMConvertToRadians(m_fFovy);}
	/// <summary>
	/// カメラの幅を取得
	/// </summary>
	/// <returns>幅</returns>
	inline const float &GetWidth() const noexcept { return m_fWidth; }
	/// <summary>
	/// 最低描画距離を取得
	/// </summary>
	/// <returns>距離</returns>
	inline const float &GetNear() const noexcept { return m_fNear; }
	/// <summary>
	/// 最大描画距離を取得
	/// </summary>
	/// <returns>距離</returns>
	inline const float &GetFar() const noexcept { return m_fFar; }
	/// <summary>
	/// カメラのアスペクト比を取得
	/// </summary>
	/// <returns>アスペクト比</returns>
	inline const float &GetAspect() const noexcept { return m_fAspect; }
	/// <summary>
	/// カメラの焦点距離を取得
	/// </summary>
	/// <returns>焦点距離</returns>
	inline const float &GetFocus() const noexcept { return m_fFocus; }

	/// <summary>
	/// 見ている点を取得
	/// </summary>
	/// <returns>焦点座標</returns>
	DirectX::XMFLOAT3 GetLook() const noexcept;

	/// <summary>
	/// フォーカスを設定
	/// </summary>
	/// <param name="focus">焦点距離</param>
	inline void SetFocus(_In_ const float &In_fFocus) noexcept { m_fFocus = In_fFocus; }

	inline void Set3D(_In_ const bool &In_bIs3D) noexcept { m_bIs3D = In_bIs3D; }

private:
	bool m_bIs3D;
	float m_fFovy;
	float m_fWidth;
	float m_fAspect, m_fNear, m_fFar;
	float m_fFocus;
#ifdef _DEBUG
	bool m_bIsShow;
#endif
};
