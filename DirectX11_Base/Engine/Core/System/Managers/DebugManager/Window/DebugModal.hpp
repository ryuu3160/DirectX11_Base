/*+===================================================================
	File: DebugModal.hpp
	Summary: モーダルダイアログ
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu PM 04:25:19 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DebugWindowBase.hpp"

/// <summary>
/// DebugModalクラス
/// </summary>
class DebugModal : public DebugWindowBase
{
	friend class DebugManager;
public:
	DebugModal(_In_ std::string_view In_Name, _In_ ImGuiWindowFlags In_Flags = 0);
	~DebugModal();

	void Draw() noexcept override;

	/// <summary>
	/// モーダルを開く
	/// </summary>
	void Open() noexcept { m_ShouldOpen = true; }

	/// <summary>
	/// モーダルを閉じる
	/// </summary>
	void Close() noexcept;

	/// <summary>
	/// OKボタンが押された時のコールバックを設定
	/// </summary>
	void SetOkCallback(_In_ std::function<void()> In_Callback) noexcept { m_OkCallback = In_Callback; }

	/// <summary>
	/// キャンセルボタンが押された時のコールバックを設定
	/// </summary>
	void SetCancelCallback(_In_ std::function<void()> In_Callback) noexcept { m_CancelCallback = In_Callback; }

	/// <summary>
	/// 開いた時のコールバックを設定
	/// </summary>
	void SetOpenCallback(_In_ std::function<void()> In_Callback) noexcept { m_OpenCallback = In_Callback; }

	/// <summary>
	/// 閉じた時のコールバックを設定
	/// </summary>
	void SetCloseCallback(_In_ std::function<void()> In_Callback) noexcept { m_CloseCallback = In_Callback; }

	/// <summary>
	/// デフォルトのOK/Cancelボタンを表示するか
	/// </summary>
	void SetShowDefaultButtons(_In_ bool In_Show) noexcept { m_ShowDefaultButtons = In_Show; }

	/// <summary>
	/// 閉じるボタンの表示を設定
	/// </summary>
	void SetShowCloseButton(_In_ bool In_IsShow) noexcept { m_IsShowCloseButton = In_IsShow; }

private:
	/// <summary>
	/// デフォルトのOK/Cancelボタンを描画
	/// </summary>
	void DrawDefaultButtons() noexcept;

private:
	bool m_ShouldOpen;
	bool m_ShouldClose;
	bool m_ShowDefaultButtons;
	bool m_IsShowCloseButton;
	std::function<void()> m_OkCallback;
	std::function<void()> m_CancelCallback;
	std::function<void()> m_OpenCallback;
	std::function<void()> m_CloseCallback;
};
