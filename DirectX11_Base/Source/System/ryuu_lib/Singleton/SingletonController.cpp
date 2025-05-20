/*+===================================================================
	File: SingletonController.cpp
	Summary: 自作シングルトン
	Author: ryuu3160
	Date: 2024/08/16       初回作成

	(C) 2024 ryuu3160. All Rights Reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SingletonController.hpp"

// ==============================
//	グローバル変数宣言
// ==============================
namespace
{
	std::mutex g_mutex;			// 排他制御用のミューテックス
	int g_nControllerCount = 0;	// 管理下にあるシングルトンクラスの数

	std::vector<SingletonController::ControllerFunc> g_ControllerList; // オブジェクト破棄関数を格納するVector
}

void SingletonController::AddController(ControllerFunc func)
{
	std::lock_guard<std::mutex> lock(g_mutex); // 同時にアクセスされないようにロック
	if (g_nControllerCount < g_ControllerList.max_size())
	{
		g_ControllerList.push_back(func);
	}
}

void SingletonController::Release()
{
	std::lock_guard<std::mutex> lock(g_mutex); // 同時にアクセスされないようにロック

	for (auto func = g_ControllerList.crbegin(); func != g_ControllerList.crend(); func++)
	{
		(*func)();
	}
	g_ControllerList.clear();
}