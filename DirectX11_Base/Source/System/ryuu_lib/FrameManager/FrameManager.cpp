/*+===================================================================
	File: FrameManager.cpp
	Summary: フレームレート管理クラスのソースファイル
	Author: ryuu3160
	Date: 2024/08/17	   初回作成
			  /11/19 10:14 クラス全体の改良
		  2025/04/08 11:40 クラス全体のリファクタリング
				 /09 03:07 大幅なリファクタリング

	(C) 2021 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "FrameManager.hpp"

void FrameManager::Init(_In_ float In_fFps, _In_ bool In_YieldWhenWaiting)
{
	// ------------------------------
	// メインフレームデータの設定
	// ------------------------------
	m_StartTime = clock::now();
	m_TargetDuration = std::chrono::duration<double>(static_cast<double>(1.0f / std::max(1e-6f, In_fFps)));

	m_LastTime = clock::now() - std::chrono::duration_cast<clock::duration>(m_TargetDuration);
	m_FpsTime = clock::now();
	m_fMainFps = In_fFps;				// フレームレートの設定
	m_bInitialized = true;				// 初期化完了フラグ
	m_bYieldWhenWaiting = In_YieldWhenWaiting;
}

bool FrameManager::UpdateMain()
{
	if (!m_bInitialized)
		return false;

	// 現在時間の更新
	auto now = clock::now();
	auto elapsed = now - m_LastTime;

	// フレームレートの制限
	if (elapsed >= m_TargetDuration)
	{
		// 経過時間を計算して内部状態を更新
		std::chrono::duration<float> dt = elapsed;
		constexpr float max_dt = 0.1f; // 大きすぎる dt を抑える（必要なら調整）
		m_fTick = std::min(dt.count(), max_dt);

		m_LastTime = now;

		// FPSの計測
#ifdef _DEBUG
		++m_FrameCount;
		auto fps_elapsed = now - m_FpsTime;
		if (fps_elapsed >= std::chrono::seconds(1))
		{
			m_LastFps = static_cast<double>(m_FrameCount) / std::chrono::duration<double>(fps_elapsed).count();
			m_FrameCount = 0;
			m_FpsTime = now;

			if (m_hWnd != nullptr)
			{
				std::string title = m_lpcTitleName;
				std::string fpsStr;
				fpsStr = ToString(static_cast<int>(m_LastFps + 0.5));
				title += " [fps]:" + fpsStr;
				SetWindowTextA(m_hWnd, title.c_str());
			}
		}
#endif // _DEBUG
		return true;
	}
	else
	{
		// まだ時間が来ていない -> 非ブロッキングでfalse を返す
		// ただしCPU負荷を下げたい場合はヒントを出す（yield）
		if (m_bYieldWhenWaiting)
		{
			std::this_thread::yield();
		}
		return false;
	}
}

void FrameManager::ChangeMainFps(_In_ float In_fFps)
{
	m_fMainFps = In_fFps;
	m_TargetDuration = std::chrono::duration<double>(static_cast<double>(1.0f / std::max(1e-6f, In_fFps)));
}

float FrameManager::GetElapsedSeconds() const
{
	return static_cast<float>(std::chrono::duration<double>(clock::now() - m_StartTime).count());
}

double FrameManager::GetElapsedMilliSeconds() const
{
	return std::chrono::duration<double, std::milli>(clock::now() - m_StartTime).count();
}

std::string FrameManager::GetNowTimeString() const
{
	std::string result;
	result.clear();
	// 現在のシステム時刻を取得
	auto now = std::chrono::system_clock::now();

	// フォーマットして表示
#if defined(_MSC_VER)
#if _MSC_VER >= 1950 // Visual Studio 2026
	// 現地時刻（タイムゾーン考慮）
	std::chrono::zoned_time zt{ std::chrono::current_zone(), now };
	auto local_tp = zt.get_local_time(); // システムタイム->ローカルタイム

	// 秒未満を切り捨て（ミリ秒以下を切る）
	auto local_sec = std::chrono::floor<std::chrono::seconds>(local_tp);
	result = std::format("{:%H:%M:%S}", local_sec);
#elif _MSC_VER >= 1930 // Visual Studio 2022
	// std::format の代わりに std::put_time + std::ostringstream を使う
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm{};
	localtime_s(&local_tm, &t);
	std::ostringstream oss;
	oss << std::put_time(&local_tm, "%H:%M:%S");
	result = oss.str();
#endif
#endif
	return result;
}

FrameManager::FrameManager()
	: m_fMainFps(0.0f), m_bInitialized(false), m_FrameCount(0), m_LastFps(0.0)
	, m_TargetDuration(std::chrono::duration<double>(0.0)), m_bYieldWhenWaiting(true)
	, m_fTick(0.0f), m_FixedDeltaTime(0.02), m_AccumulatedTime(0.0), m_MaxStepCount(5)
	, m_fTimeScale(1.0f)
{
}

FrameManager::~FrameManager()
{
	m_bInitialized = false;
}
