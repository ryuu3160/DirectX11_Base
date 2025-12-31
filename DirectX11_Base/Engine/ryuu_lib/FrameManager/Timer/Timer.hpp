/*+===================================================================
	File: Timer.hpp
	Summary: タイマークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/27 Thu PM 01:25:17 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// Timerクラス
/// </summary>
class Timer
{
public:
	Timer();
	~Timer();

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
	void ChangeFps(const std::string &In_strName, float In_fFps, unsigned int In_nLapTime = 1);

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

	// ==============================================
	// タイムカウンター関連
	// ==============================================

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

private:
	FrameManager &m_FrameManager;
	const float &m_fMainFps;

	// スレッドセーフのためのミューテックス
	std::mutex m_mutexFrameLimit;
	std::mutex m_mutexInterval;
	std::mutex m_mutexTimeCounter;

	struct FrameLimitData
	{
		int m_nFrameCount;			// フレームのカウント
		int m_nSwitchCount;			// 何回切り替えたか
		unsigned int m_nLapTime;	// 1サイクルにかける時間
		float m_fAdvanceFrame;		// 何メインフレームごとにフレームを進めるか
	};
	struct IntervalData
	{
		FrameLimitData FrameData;	// フレームデータ
		float fIntervalFrame;		// フレームの間隔
		bool bReturn;				// 間隔をあけたサイクルに使う
	};
	struct TimeCountData
	{
		DWORD m_dwCount;	// カウント
		bool m_bIsStart;	// 計測中か
	};

	std::unordered_map<std::string, FrameLimitData> m_mapFrameLimitData;	// フレームデータ
	std::unordered_map<std::string, IntervalData> m_mapIntervalData;		// インターバルデータ
	std::unordered_map<std::string, TimeCountData> m_mapTimeCounter;		// 時間計測用

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
