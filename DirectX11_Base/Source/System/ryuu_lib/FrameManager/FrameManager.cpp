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
	timeBeginPeriod(1);	// 分解能の設定

	// ------------------------------
	// メインフレームデータの設定
	// ------------------------------
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
				title += " [fps]:" + std::to_string(static_cast<int>(m_LastFps + 0.5));
				SetWindowTextA(m_hWnd, title.c_str());
			}
		}
#endif // _DEBUG
		return true;
	}
	else
	{
		// まだ時間が来ていない -> 非ブロッキングで false を返す
		// ただし CPU 負荷を下げたい場合は少しだけヒントを出す（yield）
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

float FrameManager::GetNowTimeSecond() const
{
	return static_cast<float>(std::chrono::duration<double>(clock::now().time_since_epoch()).count());
}

double FrameManager::GetNowTimeMilliSec() const
{
	return std::chrono::duration<double, std::milli>(clock::now().time_since_epoch()).count();
}



FrameManager::FrameManager()
	: m_fMainFps(0.0f), m_bInitialized(false), m_FrameCount(0), m_LastFps(0.0)
	, m_TargetDuration(std::chrono::duration<double>(0.0)), m_bYieldWhenWaiting(true)
	, m_fTick(0.0f)
{
}

FrameManager::~FrameManager()
{
	m_bInitialized = false;
	timeEndPeriod(1);	// 分解能の解除
}
