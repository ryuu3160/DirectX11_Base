/*+===================================================================
	File: Observer.cpp
	Summary: Observerクラス
	Author: ryuu3160
	Date: 2025/02/07 13:57 初回作成
				 /09 06:34 関数ポインタを追加する関数を追加
					 06:34 登録されている関数を全て実行する関数を追加
					 06:38 登録されている関数をクリアする関数を追加

	(C) 2025 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Observer.hpp"

Observer::Observer()
{
	// リストをクリア
	m_Function.clear();
}

Observer::~Observer()
{
}

void Observer::AppendFunction(std::function<void()> In_Function, std::string In_strNotifyName)
{
	// 指定の通知名で関数を追加
	// std::functionをそのまま追加
	m_Function[In_strNotifyName].push_back(In_Function);
}

void Observer::Notify(std::string In_strNotifyName)
{
	// 指定の通知名が存在しない場合は何もしない
	if (m_Function.find(In_strNotifyName) == m_Function.end()) return;

	// 指定の通知名で登録されている関数を全て実行
	for (auto &Func : m_Function.at(In_strNotifyName))
	{
		Func(); // 関数を実行
	}
}

void Observer::ClearFunction(std::string In_strNotifyName)
{
	// 指定の通知名が存在しない場合は何もしない
	if (m_Function.find(In_strNotifyName) == m_Function.end()) return;

	// 指定の通知名で登録されている関数を全て削除
	m_Function.at(In_strNotifyName).clear();
}

void Observer::AllClearFunction()
{
	for (auto &Func : m_Function)
	{
		// 登録されている関数を全て削除
		Func.second.clear();
	}
	// リストをクリア
	m_Function.clear();
}
