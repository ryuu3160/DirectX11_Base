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

	// ------------------------------
	//  固定フレーム用
	// ------------------------------

	double GetFixedDeltaTime() const { return m_FixedDeltaTime; }

	void SetFixedDeltaTime(_In_ double In_DeltaTime) { m_FixedDeltaTime = In_DeltaTime; }

	void ResetFixedDeltaTime() { m_FixedDeltaTime = 0.02; }

	int GetMaxStepCount() const { return m_MaxStepCount; }
	void SetMaxStepCount(_In_ int In_MaxStepCount) { m_MaxStepCount = In_MaxStepCount; }
	void ResetMaxStepCount() { m_MaxStepCount = 5; }

	const double &GetAccumulatedTime() const { return m_AccumulatedTime; }
	void AddAccumulatedTime(_In_ double In_DeltaTime) { m_AccumulatedTime += In_DeltaTime; }
	void SubAccumulatedTime(_In_ double In_DeltaTime) { m_AccumulatedTime -= In_DeltaTime; }
	void SetAccumulatedTime(_In_ double In_Time) { m_AccumulatedTime = In_Time; }
	void ResetAccumulatedTime() { m_AccumulatedTime = 0.0; }

	// ------------------------------
	//  タイムスケール用
	// ------------------------------

	/// <summary>
	/// 現在のタイムスケール（倍率）を取得する
	/// </summary>
	/// <returns>現在のタイムスケールを表すfloat値</returns>
	float GetTimeScale() const { return m_fTimeScale; }

	/// <summary>
	/// タイムスケール（時間の進行倍率）を設定する
	/// </summary>
	/// <param name="[In_fTimeScale]">設定するタイムスケールの値。時間の進行倍率（例：1.0で通常、0.5で半速、2.0で倍速）を指定する</param>
	void SetTimeScale(_In_ float In_fTimeScale) { m_fTimeScale = In_fTimeScale; }

	/// <summary>
	/// 時間スケールを1.0（標準速度）にリセットします。
	/// </summary>
	void ResetTimeScale() { m_fTimeScale = 1.0f; }

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

	// 固定フレーム用
	double m_FixedDeltaTime;
	double m_AccumulatedTime;
	int m_MaxStepCount;

	// タイムスケール
	float m_fTimeScale;

	// デバッグ用
#ifdef _DEBUG
	HWND m_hWnd = nullptr;	// デバッグ用ウィンドウハンドル
	LPCSTR m_lpcTitleName = "";	// タイトルバーの表示名
	DWORD FpsCount = 0;	// FPSカウント
	DWORD FPS = 0;		// 直近のFPS
	DWORD FpsTime = 0;
#endif // _DEBUG
};
