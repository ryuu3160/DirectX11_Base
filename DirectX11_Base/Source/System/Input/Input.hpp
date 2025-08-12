/*+===================================================================
  File : Input.hpp
  Summary : キーボードの入力処理を行う
  Author : AT13C 01 青木雄一郎
  Date : 2024/10/16

  (C) 2024 AT13C 01 青木雄一郎. All Rights Reserved.
===================================================================+*/
#pragma once

#include <sal.h>
#include <Windows.h>
#undef max
#undef min

namespace Input
{
	/// <summary>
	/// 入力処理の初期化
	/// </summary>
	/// <returns>HRESULT型</returns>
	HRESULT Init();
	/// <summary>
	/// 入力処理の終了
	/// </summary>
	void Uninit();
	/// <summary>
	/// 入力処理の更新
	/// </summary>
	void Update();

	/// <summary>
	/// 全ての更新処理が完了した後に呼び出す
	/// </summary>
	void EndUpdate() noexcept;

	/// <summary>
	/// キーが押されているか
	/// </summary>
	/// <param name="[key]">キーコード</param>
	/// <returns>true:押されている false:押されていない</returns>
	bool IsKeyPress(BYTE key);
	/// <summary>
	/// キーが押された瞬間か
	/// </summary>
	/// <param name="[key]">キーコード</param>
	/// <returns>押された瞬間か否か</returns>
	bool IsKeyTrigger(BYTE key);
	/// <summary>
	/// キーが離された瞬間か
	/// </summary>
	/// <param name="[key]">キーコード</param>
	/// <returns>離された瞬間か否か</returns>
	bool IsKeyRelease(BYTE key);

	/// <summary>
	/// キーがリピートされているか
	/// </summary>
	/// <param name="[key]">キーコード</param>
	/// <returns>リピートされているか否か</returns>
	bool IsKeyRepeat(BYTE key);

	/// <summary>
	/// マウスの現在位置を、ウィンドウまたは基準点からの相対座標で取得します。
	/// </summary>
	/// <returns>マウスの相対座標を表す POINTS 構造体。</returns>
	POINTS GetMouseRelativePos() noexcept;

	/// <summary>
	/// マウスカーソルの現在位置を、画面中心を原点（0,0）とした座標で取得します。
	/// </summary>
	/// <returns>画面中心を原点としたマウスカーソルの相対座標を表す POINTS 構造体。</returns>
	POINTS GetMouseRelativePos_CenterZero() noexcept;

	/// <summary>
	/// マウスの位置の差分を取得します。
	/// </summary>
	/// <returns>現在のマウス位置と前回のマウス位置との差分を表す POINT 構造体。</returns>
	POINT GetMouseDifference() noexcept;

	/// <summary>
	/// マウスホイールの回転量を取得します。
	/// </summary>
	/// <returns>マウスホイールの回転量を表す浮動小数点数。正の値は上方向、負の値は下方向の回転を示します。</returns>
	int GetMouseWheelDelta() noexcept;

	/// <summary>
	/// 指定されたウィンドウメッセージを処理するカスタムウィンドウプロシージャです。
	/// </summary>
	/// <param name="[In_hWnd]">メッセージが送信されたウィンドウのハンドル。</param>
	/// <param name="[In_Msg]">処理するウィンドウメッセージの識別子。</param>
	/// <param name="[In_wParam]">メッセージに関連する追加情報（ワード値）。</param>
	/// <param name="[In_lParam]">メッセージに関連する追加情報（ロング値）。</param>
	/// <returns>メッセージ処理の結果として返される値（LRESULT型）。</returns>
	LRESULT InputCustomProc(_In_ HWND In_hWnd, _In_ UINT In_Msg, _In_ WPARAM In_wParam, _In_ LPARAM In_lParam);

} // namespace Input