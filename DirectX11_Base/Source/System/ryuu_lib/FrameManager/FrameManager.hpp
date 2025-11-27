/*+===================================================================
	File: FrameManager.hpp
	Summary: フレームレート管理クラスのヘッダファイル
	Author: ryuu3160
	Date: 2024/08/17	   初回作成
			  /11/19 10:14 クラス全体の改良
		  2025/04/08 11:40 クラス全体のリファクタリング
				 /09 03:07 大幅なリファクタリング

	(C) 2021 ryuu3160. All rights reserved.
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "../Singleton/Singleton.hpp"
#include <windows.h>
#include <unordered_map>
// timeGetTimeなどを使うために必要
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

/// <summary>
/// フレームレート管理クラス
/// </summary>
class FrameManager : public Singleton<FrameManager>
{
	friend class Singleton<FrameManager>;
	using clock = std::chrono::steady_clock;
public:
	/// <summary>
	/// フレームレート初期化（main）
	/// </summary>
	/// <param name="[In_fFps]">更新するフレーム数</param>
	void Init(_In_ float In_fFps = 60.0f, _In_ bool In_YieldWhenWaiting = true);

	/// <summary>
	/// フレームレートの制限（main）
	/// </summary>
	/// <returns>設定したフレーム数を超えていた場合trueを返す</returns>
	bool UpdateMain();

	float GetTick() const { return m_fTick; }

	const float &GetMainFps() const { return m_fMainFps; }

	void ChangeMainFps(_In_ float In_fFps);

	void SetYieldWhenWaiting(_In_ bool In_bYield) { m_bYieldWhenWaiting = In_bYield; }

	/// <summary>
	/// 経過時間を取得
	/// </summary>
	/// <returns>経過時間（秒）</returns>
	float GetNowTimeSecond() const;

	/// <summary>
	/// 現在の時刻をミリ秒単位で取得します。
	/// </summary>
	/// <returns>現在の時刻をミリ秒単位で表した double 型の値を返します。</returns>
	double GetNowTimeMilliSec() const;

#ifdef _DEBUG
	/// <summary>
	/// fpsを表示するウィンドウのハンドルを設定(デバッグ用)
	/// </summary>
	/// <param name="In_hWnd">設定するウィンドウハンドル。</param>
	void _SetHwnd(HWND In_hWnd) { m_hWnd = In_hWnd; }

	/// <summary>
	/// ウィンドウのタイトルを設定します。
	/// </summary>
	/// <param name="In_lpcTitleName">設定するウィンドウタイトルの文字列（LPCSTR型）。</param>
	void _SetWindowTitle(LPCSTR In_lpcTitleName) { m_lpcTitleName = In_lpcTitleName; }
#endif

private:
	FrameManager();
	~FrameManager();

private:
	// メインのフレームデータ
	std::chrono::duration<double> m_TargetDuration;
	std::chrono::steady_clock::time_point m_LastTime;

	bool m_bYieldWhenWaiting;

	std::chrono::steady_clock::time_point m_FpsTime;
	double m_LastFps;
	float m_fMainFps;
	float m_fTick;
	int m_FrameCount;
	bool m_bInitialized;

	// デバッグ用
#ifdef _DEBUG
	HWND m_hWnd = nullptr;	// デバッグ用ウィンドウハンドル
	LPCSTR m_lpcTitleName = "";	// タイトルバーの表示名
	DWORD FpsCount = 0;	// FPSカウント
	DWORD FPS = 0;		// 直近のFPS
	DWORD FpsTime = 0;
#endif // _DEBUG
};
