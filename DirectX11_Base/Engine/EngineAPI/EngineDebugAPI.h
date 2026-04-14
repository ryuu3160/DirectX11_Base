/*+===================================================================
	File: EngineDebugAPI.h
	Summary: Debug関連のEngineAPI定義
	Author: 青木雄一郎
	Date: 2026/04/14 Mon PM 12:08:34 初回作成
===================================================================+*/
#pragma once

// ==============================
// include
// ==============================
#include "EngineAPI.h"

namespace Debug
{
	extern const ForgeXEngineAPI_POD *g_Engine;

	inline void Log(const char *fmt, ...)
	{
		if(!g_Engine || !g_Engine->Log) return;

		va_list args;
		va_start(args, fmt);
		g_Engine->Log(fmt, args);
		va_end(args);
	}

	inline void LogWarning(const char *fmt, ...)
	{
		if(!g_Engine || !g_Engine->LogWarning) return;
		va_list args;
		va_start(args, fmt);
		g_Engine->LogWarning(fmt, args);
		va_end(args);
	}

	inline void LogError(const char *fmt, ...)
	{
		if(!g_Engine || !g_Engine->LogError) return;
		va_list args;
		va_start(args, fmt);
		g_Engine->LogError(fmt, args);
		va_end(args);
	}

}