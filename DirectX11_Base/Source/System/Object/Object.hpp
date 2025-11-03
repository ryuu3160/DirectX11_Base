/*+===================================================================
	File: Object.hpp
	Summary: オブジェクトの基底クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/10/13 Mon PM 11:45:55 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	前方宣言
// ==============================
class ColliderBase;

/// <summary>
/// Objectクラス
/// </summary>
class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

	void SetActive(_In_ bool In_IsActive) noexcept;

	virtual void OnEnable() noexcept {};

	virtual void Init() noexcept = 0;
	virtual void Update() noexcept {};
	virtual void LateUpdate() noexcept {};

protected:

	// 当たり判定用
	virtual void OnCollisionEnter(_In_ ColliderBase *In_Other) noexcept {};
	virtual void OnCollisionStay(_In_ ColliderBase *In_Other) noexcept {};
	virtual void OnCollisionExit(_In_ ColliderBase *In_Other) noexcept {};

protected:
	bool m_IsActive = true;
	bool m_IsInitialized = false;
};
