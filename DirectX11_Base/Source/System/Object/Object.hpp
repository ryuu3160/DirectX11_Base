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
#include "DirectX11/System/Gizmos.hpp"
// ==============================
//	前方宣言
// ==============================
class ColliderBase;
class DebugWindow;

/// <summary>
/// Objectクラス
/// </summary>
class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

	void SetActive(_In_ bool In_IsActive) noexcept;
	void SetActiveParent(_In_ bool In_IsActiveParent) noexcept;

	virtual void OnEnable() noexcept {}
	virtual void OnDisable() noexcept {}
	[[nodiscard]] bool IsActive() const noexcept { return m_IsActive; }
	[[nodiscard]] bool IsActiveParent() const noexcept { return m_IsActiveParent; }
	[[nodiscard]] bool IsInitialized() const noexcept { return m_IsInitialized; }

	virtual void Init() noexcept = 0;
	virtual void Awake() noexcept {}
	virtual void Update(_In_ float In_Tick) noexcept {}
	virtual void LateUpdate(_In_ float In_Tick) noexcept {}
	virtual void FixedUpdate(_In_ double In_FixedTick) noexcept {}

	virtual void DestroySelf() noexcept { m_IsDestroySelf = true;}
	[[nodiscard]] bool IsDestroySelf() const noexcept { return m_IsDestroySelf; }

protected:

	// 当たり判定用
	virtual void OnCollisionEnter(_In_ ColliderBase *In_Other) noexcept {}
	virtual void OnCollisionStay(_In_ ColliderBase *In_Other) noexcept {}
	virtual void OnCollisionExit(_In_ ColliderBase *In_Other) noexcept {}
	virtual void OnTriggerEnter(_In_ ColliderBase *In_Other) noexcept {}
	virtual void OnTriggerStay(_In_ ColliderBase *In_Other) noexcept {}
	virtual void OnTriggerExit(_In_ ColliderBase *In_Other) noexcept {}

	// ギズモ描画
	virtual void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept {}

#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) {}
#endif // _DEBUG

protected:
	bool m_IsActive = true;
	bool m_IsActiveParent = true;
	bool m_IsInitialized = false;
	bool m_IsDestroySelf = false;
};
