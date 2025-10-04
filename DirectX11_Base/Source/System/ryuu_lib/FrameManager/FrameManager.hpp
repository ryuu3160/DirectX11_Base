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
public:
	/// <summary>
	/// フレームレート初期化（main）
	/// </summary>
	/// <param name="[In_fFps]">更新するフレーム数</param>
	void Init(float In_fFps);

	/// <summary>
	/// フレームレートの制限（main）
	/// </summary>
	/// <returns>設定したフレーム数を超えていた場合trueを返す</returns>
	bool UpdateMain();

	/// <summary>
	/// 現在時間の取得（main）
	/// </summary>
	/// <returns>現在時間（ms）</returns>
	DWORD GetMainTime() const;

	/// <summary>
	/// <para>フレームレート制限の追加</para>
	/// <para>フレームレートはメインのfpsよりも大きな値は設定できない</para>
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <param name="[In_fFps]">フレームレート(1サイクルで処理したいフレーム数)</param>
	/// <param name="[In_nLapTime]">1サイクルにかける時間(デフォルトは1秒)</param>
	void AppendLimitation(const std::string &In_strName, float In_fFps, unsigned int In_nLapTime = 1);

	/// <summary>
	/// <para>間隔をあけるフレームデータの追加</para>
	/// <para>指定したフレーム数でtrueとfalseを切り替える</para>
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <param name="[In_fTrueFrame]">trueにするフレーム数</param>
	/// <param name="[In_fIntervalFrame]">falseにするフレーム数</param>
	void AppendInterval(const std::string &In_strName, float In_fTrueFrame, float In_fIntervalFrame);

	/// <summary>
	/// 指定したフレームデータの更新
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <returns>設定したフレーム数を超えていた場合trueを返す</returns>
	bool Update(const std::string &In_strName);

	/// <summary>
	/// 指定したフレームデータのカウントをリセット
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	void FrameCountReset(const std::string &In_strName);

	/// <summary>
	/// 指定したフレームデータの切替回数をリセット
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	void SwitchCountReset(const std::string &In_strName);

	/// <summary>
	/// 指定したフレームデータのフレームレートの変更
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <param name="[In_fFps]">フレームレート</param>
	/// <param name="[In_nLapTime]">1サイクルにかける時間(デフォルトは1秒)</param>
	void ChangeFps(const std::string &In_strName, float In_fFps,unsigned int In_nLapTime = 1);

	/// <summary>
	/// <para>指定したフレームデータのTrue-False切替フレーム数の変更</para>
	/// <para>指定するフレーム数に-1を入れた場合はその値は変更しない</para>
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <param name="[In_fTrueFrame]">trueにするフレーム数</param>
	/// <param name="[In_fIntervalFrame]">falseにするフレーム数</param>
	void ChangeIntervalFrame(const std::string &In_strName, float In_fTrueFrame = -1, float In_fIntervalFrame = -1);

	/// <summary>
	/// 指定したフレームデータが合計で何回TRUEとFALSEを切り替えたかを取得
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	/// <returns>切り替え回数 ※フレームデータが存在しない場合は-1を返す</returns>
	int GetSwitchCount(const std::string &In_strName);

	/// <summary>
	/// タイムカウンターを追加
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	/// <param name="[In_bIsStartNow]">すぐに計測を開始するか</param>
	void AppendTimeCounter(const std::string &In_strName, bool In_bIsStartNow = false);

	/// <summary>
	/// タイムカウンターの計測を開始
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	void StartTimeCounter(const std::string &In_strName);

	/// <summary>
	/// 全てのタイムカウンターの更新
	/// </summary>
	void UpdateAllTimeCounter();

	/// <summary>
	/// タイムカウンターの更新
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	void UpdateTimeCounter(const std::string &In_strName);

	/// <summary>
	/// 全てのタイムカウンターを止める
	/// </summary>
	void StopAllTimeCounter();

	/// <summary>
	/// タイムカウンターを止める
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	void StopTimeCounter(const std::string &In_strName);

	/// <summary>
	/// 全てのタイムカウンターをリセット
	/// </summary>
	void ResetAllTimeCounter();

	/// <summary>
	/// タイムカウンターをリセット
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	void ResetTimeCounter(const std::string &In_strName);

	/// <summary>
	/// タイムカウンターを秒に変換し取得
	/// </summary>
	/// <param name="[In_strName]">カウンター名</param>
	/// <returns>秒</returns>
	float GetTimeCountSecond(const std::string &In_strName);

	/// <summary>
	/// フレームデータの削除
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	void Delete(const std::string &In_strName);

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

private:
	FrameManager();
	~FrameManager();

private:
	// メインのフレームデータ
	DWORD m_dwTime;
	DWORD m_dwOldTime;
	float m_fMainFps;
	bool m_bMainExists;

	// スレッドセーフのためのミューテックス
	std::mutex m_mutexFrameLimit;
	std::mutex m_mutexInterval;
	std::mutex m_mutexTimeCounter;

	/// <summary>
	/// フレームデータ構造体
	/// </summary>
	struct FrameLimitData
	{
		int m_nFrameCount;			// フレームのカウント
		int m_nSwitchCount;			// 何回切り替えたか
		unsigned int m_nLapTime;	// 1サイクルにかける時間
		float m_fAdvanceFrame;		// 何メインフレームごとにフレームを進めるか
	};

	/// <summary>
	/// インターバルで使用する構造体
	/// </summary>
	struct IntervalData
	{
		FrameLimitData FrameData;	// フレームデータ
		float fIntervalFrame;		// フレームの間隔
		bool bReturn;				// 間隔をあけたサイクルに使う
	};

	/// <summary>
	/// カウント用の構造体
	/// </summary>
	struct TimeCountData
	{
		DWORD m_dwCount;	// カウント
		bool m_bIsStart;	// 計測中か
	};

	std::unordered_map<std::string, FrameLimitData> m_mapFrameLimitData;	// フレームデータ
	std::unordered_map<std::string, IntervalData> m_mapIntervalData;		// インターバルデータ
	std::unordered_map<std::string, TimeCountData> m_mapTimeCounter;		// 時間計測用

	// デバッグ用
#ifdef _DEBUG
	HWND m_hWnd = nullptr;	// デバッグ用ウィンドウハンドル
	LPCSTR m_lpcTitleName = "";	// タイトルバーの表示名
	DWORD FpsCount = 0;	// FPSカウント
	DWORD FPS = 0;		// 直近のFPS
	DWORD FpsTime = 0;
#endif // _DEBUG
private:

	/// <summary>
	/// フレームデータの更新
	/// </summary>
	/// <param name="[In_itr]">更新するフレームデータ</param>
	bool UpdateLimitation(std::unordered_map<std::string, FrameLimitData>::iterator In_itr);

	/// <summary>
	/// フレームデータの更新(間隔をあけるバージョン)
	/// </summary>
	/// <param name="[In_itr]">更新するフレームデータ</param>
	bool UpdateInterval(std::unordered_map<std::string, IntervalData>::iterator In_itr);

	/// <summary>
	/// フレームデータが存在するか
	/// </summary>
	/// <param name="[In_strName]">フレームデータ名</param>
	void CheckExistsLimitAndInterval(const std::string &In_strName);
};
