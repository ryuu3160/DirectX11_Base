/*+===================================================================
	File: Transform.hpp
	Summary: Transformクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/23 Tue PM 06:37:41 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/System/Component/Component.hpp"

/// <summary>
/// Transformクラス
/// </summary>
class Transform : public Component
{
public:
	Transform();
	~Transform();

	virtual void Init() noexcept override;

	void SaveLoad(_In_ DataAccessor *In_Data) final;

	/// <summary>
	/// <para>指定された角度で回転を適用します</para>
	/// <para>連続した回転を行う場合はこちらを使用してください</para>
	/// </summary>
	/// <param name="[In_Angle]">回転角度を表す3次元ベクトル(Degree)</param>
	void Rotate(_In_ float In_PitchDeg, _In_ float In_YawDeg, _In_ float In_RollDeg) noexcept;

	// ------------------------------
	//  Getter
	// ------------------------------
	DirectX::XMFLOAT3 GetRotation(_In_ bool In_IsDegree = false) noexcept;

	inline DirectX::XMFLOAT3 GetPosition() const noexcept { return m_Pos; }
	inline DirectX::XMFLOAT4 GetQuat() const noexcept { return m_Quat; }
	inline DirectX::XMFLOAT3 GetScale() const noexcept { return m_Scale; }

	DirectX::XMFLOAT3 GetFront(_In_ const bool &Is_Normalize = true) const noexcept;
	DirectX::XMFLOAT3 GetRight() const noexcept;
	DirectX::XMFLOAT3 GetUp() const noexcept;

	// ------------------------------
	//  Setter
	// ------------------------------
	void SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;

	/// <summary>
	/// 回転を設定します
	/// </summary>
	/// <param name="[In_Rotation]">設定する回転値(Degree)</param>
	void SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept;
	void SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept;
	void SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept;

protected:
#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif

private:

	/// <summary>
	/// <para>クォータニオンからオイラー角を同期して取得します</para>
	/// <para>インスペクター用</para>
	/// </summary>
	/// <param name="[In_Flag]">同期するオイラー角の成分を指定するフラグ</param>
	/// <returns>クォータニオンから計算されたオイラー角 (ピッチ、ヨー、ロール) を表す XMFLOAT3 構造体。</returns>
	DirectX::XMFLOAT3 SyncEulerFromQuat() noexcept;

	/// SyncEulerFromQuatのヘルパー関数
	DirectX::XMFLOAT3 ChoiceBestEuler(_In_ const std::vector<DirectX::XMFLOAT3> In_Condidates) const noexcept;

	// SyncEulerFromQuatのヘルパー関数
	std::vector<DirectX::XMFLOAT3> GetPracticalEulerCandidates() const noexcept;
	// SyncEulerFromQuatのヘルパー関数
	std::vector<DirectX::XMFLOAT3> GenerateGimbalLockCandidates(_In_ float In_Pitch, _In_ float In_Yaw, _In_ float In_Roll, _In_ float In_StepDeg = 15.0f) const noexcept;

private:
	DirectX::XMFLOAT3	m_Pos;		// 座標
	DirectX::XMFLOAT3	m_Scale;	// 拡縮
	DirectX::XMFLOAT4	m_Quat;		// 回転(クォータニオン)

	// Euler角用
	DirectX::XMFLOAT3	m_Euler;		// 回転(オイラー角)
	DirectX::XMFLOAT3	m_AccumEuler;	// オイラー角の累積
	bool m_IsSyncEuler;					// オイラー角とクォータニオンの同期が必要かどうか
};
