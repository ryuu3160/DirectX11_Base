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

/// <summary>
/// Cameraクラス
/// </summary>
class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void Init() noexcept override;

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
	/// Z軸を固定しているかどうかを取得
	/// </summary>
	/// <returns>Z軸が固定されているかのフラグ</returns>
	inline const bool &IsLockZ() const noexcept { return m_bIsLockZ; }

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

	/// <summary>
	/// 垂直視野角（fovy）を設定します。
	/// </summary>
	/// <param name="In_fFovy">設定する垂直視野角を表す const float 型の参照。</param>
	inline void SetFovy(_In_ const float &In_fFovy) noexcept { m_fFovy = In_fFovy; }

	inline void Set3D(_In_ const bool &In_bIs3D) noexcept { m_bIs3D = In_bIs3D; }

	/// <summary>
	/// Z軸を固定するかどうか
	/// </summary>
	/// <param name="[In_bIsLock]">固定するかどうかの真偽値</param>
	inline void SetIsLockZ(_In_ const bool &In_bIsLock) noexcept { m_bIsLockZ = In_bIsLock; }

private:
	bool m_bIs3D;
	bool m_bIsLockZ;
	float m_fFovy;
	float m_fWidth;
	float m_fAspect, m_fNear, m_fFar;
	float m_fFocus;
#ifdef _DEBUG
	bool m_bIsShow;
#endif
};
