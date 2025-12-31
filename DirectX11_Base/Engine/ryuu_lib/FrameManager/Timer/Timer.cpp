/*+===================================================================
	File: Timer.cpp
	Summary: タイマークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/27 Thu PM 01:26:21 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Timer.hpp"

Timer::Timer()
	: m_FrameManager(FrameManager::GetInstance())
	, m_fMainFps(m_FrameManager.GetMainFps())
{
}

Timer::~Timer()
{
}

void Timer::AppendLimitation(const std::string &In_strName, float In_fFps, unsigned int In_nLapTime)
{
	// スレッドセーフにするためのロック
	std::lock_guard<std::mutex> lock(m_mutexFrameLimit);

	if (In_fFps > m_fMainFps) // メインのfpsよりも大きい値を設定できない
	{
		throw std::invalid_argument("\"Main fps\"よりも大きな値を設定することはできません。");
	}

	// 重複チェック
	CheckExistsLimitAndInterval(In_strName);

	FrameLimitData data{};
	data.m_nFrameCount = 0;			// フレームのカウント
	data.m_nLapTime = In_nLapTime;	// 1サイクルにかける時間
	data.m_nSwitchCount = 0;		// 切替回数の初期化

	data.m_fAdvanceFrame = (m_fMainFps * In_nLapTime) / In_fFps;	// 何メインフレームごとに1フレームを進めるかの計算

	m_mapFrameLimitData[In_strName] = data;	// フレームデータの追加
}

void Timer::AppendInterval(const std::string &In_strName, float In_fTrueFrame, float In_fIntervalFrame)
{
	// スレッドセーフにするためのロック
	std::lock_guard<std::mutex> lock(m_mutexInterval);

	// 重複チェック
	CheckExistsLimitAndInterval(In_strName);

	IntervalData data{};
	data.FrameData.m_nFrameCount = 0;		// フレームのカウント
	data.FrameData.m_nLapTime = 1;		// 1サイクルにかける時間
	data.FrameData.m_nSwitchCount = 0;	// 切替回数の初期化

	data.FrameData.m_fAdvanceFrame = In_fTrueFrame;	// trueのフレーム数
	data.fIntervalFrame = In_fIntervalFrame;	// falseのフレーム数

	data.bReturn = false;		// 間隔をあけたサイクルに使う

	m_mapIntervalData[In_strName] = data;	// フレームデータの追加
}

bool Timer::Update(const std::string &In_strName)
{
	auto LimitItr = m_mapFrameLimitData.find(In_strName);	// フレームデータの探索
	std::unordered_map<std::string, IntervalData>::iterator IntervalItr; // インターバルデータのイテレータ

	// フレームデータの探索
	if (LimitItr != m_mapFrameLimitData.end())
	{
		return UpdateLimitation(LimitItr);	// フレームデータの更新処理
	}
	// インターバルデータの探索
	else if ((IntervalItr = m_mapIntervalData.find(In_strName)) != m_mapIntervalData.end())
	{
		return UpdateInterval(IntervalItr);	// 間隔をあけるバージョンの更新処理
	}

	// フレームデータが存在しない場合はfalseを返す
	return false;
}

void Timer::FrameCountReset(const std::string &In_strName)
{
	auto itr = m_mapFrameLimitData.find(In_strName);
	if (itr != m_mapFrameLimitData.end())
		itr->second.m_nFrameCount = 0;	// フレームのカウントをリセット
}

void Timer::SwitchCountReset(const std::string &In_strName)
{
	auto itr = m_mapFrameLimitData.find(In_strName);
	if (itr != m_mapFrameLimitData.end())
		itr->second.m_nSwitchCount = 0;	// 切替回数のリセット
}

void Timer::ChangeFps(const std::string &In_strName, float In_fFps, unsigned int In_nLapTime)
{
	auto itr = m_mapFrameLimitData.find(In_strName);
	if (itr != m_mapFrameLimitData.end())
	{
		itr->second.m_fAdvanceFrame = (m_fMainFps * In_nLapTime) / In_fFps;	// 何メインフレームごとに1フレームを進めるかの計算
	}
}

void Timer::ChangeIntervalFrame(const std::string &In_strName, float In_fTrueFrame, float In_fIntervalFrame)
{
	auto itr = m_mapIntervalData.find(In_strName);
	if (itr != m_mapIntervalData.end())
	{
		if (In_fTrueFrame > 0)
			itr->second.FrameData.m_fAdvanceFrame = In_fTrueFrame;
		if (In_fIntervalFrame > 0)
			itr->second.fIntervalFrame = In_fIntervalFrame;
	}
}

int Timer::GetSwitchCount(const std::string &In_strName)
{
	auto itr = m_mapFrameLimitData.find(In_strName);
	if (itr != m_mapFrameLimitData.end())
		return itr->second.m_nSwitchCount;
	else
		return -1;
}

void Timer::AppendTimeCounter(const std::string &In_strName, bool In_bIsStartNow)
{
	// スレッドセーフにするためのロック
	std::lock_guard<std::mutex> lock(m_mutexTimeCounter);

	// 名前の重複チェック
	if (m_mapTimeCounter.find(In_strName) != m_mapTimeCounter.end())
		return;

	TimeCountData data{};
	data.m_dwCount = 0;

	// すぐに計測を始めるかどうか
	if (In_bIsStartNow)
		data.m_bIsStart = true;
	else
		data.m_bIsStart = false;

	m_mapTimeCounter[In_strName] = data; // データ追加
}

void Timer::StartTimeCounter(const std::string &In_strName)
{
	auto itr = m_mapTimeCounter.find(In_strName);
	if (itr != m_mapTimeCounter.end())
		itr->second.m_bIsStart = true;	// 計測開始
}

void Timer::UpdateAllTimeCounter()
{
	for (auto &data : m_mapTimeCounter)
	{
		if (data.second.m_bIsStart)
			++data.second.m_dwCount;	// カウントを進める
	}
}

void Timer::UpdateTimeCounter(const std::string &In_strName)
{
	auto itr = m_mapTimeCounter.find(In_strName);
	if (itr == m_mapTimeCounter.end())
		return;

	if (itr->second.m_bIsStart)
		++itr->second.m_dwCount;	// カウントを進める
}

void Timer::StopAllTimeCounter()
{
	for (auto &data : m_mapTimeCounter)
	{
		data.second.m_bIsStart = false;	// 計測を止める
	}
}

void Timer::StopTimeCounter(const std::string &In_strName)
{
	auto itr = m_mapTimeCounter.find(In_strName);
	if (itr != m_mapTimeCounter.end())
		itr->second.m_bIsStart = false;	// 計測を止める
}

void Timer::ResetAllTimeCounter()
{
	for (auto &data : m_mapTimeCounter)
		data.second.m_dwCount = 0;
}

void Timer::ResetTimeCounter(const std::string &In_strName)
{
	auto itr = m_mapTimeCounter.find(In_strName);
	if (itr != m_mapTimeCounter.end())
		itr->second.m_dwCount = 0;	// カウントをリセット
}

float Timer::GetTimeCountSecond(const std::string &In_strName)
{
	auto itr = m_mapTimeCounter.find(In_strName);
	if (itr == m_mapTimeCounter.end())
		return -1.0f;

	return static_cast<float>(itr->second.m_dwCount) / m_fMainFps;
}

void Timer::Delete(const std::string &In_strName)
{
	auto itr = m_mapFrameLimitData.find(In_strName);
	if (itr != m_mapFrameLimitData.end()) m_mapFrameLimitData.erase(itr);	// フレームデータの削除
}

bool Timer::UpdateLimitation(std::unordered_map<std::string, FrameLimitData>::iterator In_itr)
{
	// 現在時間の更新
	++In_itr->second.m_nFrameCount; // フレームのカウントを進める

	// フレームレートの制限
	if (In_itr->second.m_nFrameCount >= In_itr->second.m_fAdvanceFrame)
	{
		In_itr->second.m_nFrameCount = 0;
		++In_itr->second.m_nSwitchCount;
		return true;
	}
	return false;
}

bool Timer::UpdateInterval(std::unordered_map<std::string, IntervalData>::iterator In_itr)
{
	//現在フレームの更新
	++In_itr->second.FrameData.m_nFrameCount;

	// 現在のReturnの状態に応じて、フレーム数の目標値を取得
	float fFrame;
	if (In_itr->second.bReturn)
		fFrame = In_itr->second.FrameData.m_fAdvanceFrame;
	else
		fFrame = In_itr->second.fIntervalFrame;

	// フレームレートの制限
	if (In_itr->second.FrameData.m_nFrameCount >= fFrame)
	{
		In_itr->second.FrameData.m_nFrameCount = 0;// フレームのカウントをリセット
		++In_itr->second.FrameData.m_nSwitchCount; // 切替回数のカウント
		In_itr->second.bReturn = !In_itr->second.bReturn; // Returnの状態を反転
	}

	return In_itr->second.bReturn;
}

void Timer::CheckExistsLimitAndInterval(const std::string &In_strName)
{
	// フレームデータ名がLimitation,Intervalのどちらかに既に存在する場合はエラーメッセージを出力して、スキップ
	if (m_mapFrameLimitData.find(In_strName) != m_mapFrameLimitData.end())
	{
		throw std::invalid_argument("この名前は既に\"Limitation\"で使用されています。");
	}
	else if (m_mapIntervalData.find(In_strName) != m_mapIntervalData.end())
	{
		throw std::invalid_argument("この名前は既に\"Interval\"で使用されています。");
	}
}
