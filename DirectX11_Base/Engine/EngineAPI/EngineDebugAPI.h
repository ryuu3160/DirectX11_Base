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

extern const ForgeXEngineAPI_POD *g_EngineAPI;

namespace Debug
{
	inline void Log(const char *In_Fmt, ...)
	{
		if(!g_EngineAPI || !g_EngineAPI->LogV)
			return;
		va_list args;
		va_start(args, In_Fmt);
		g_EngineAPI->LogV(FORGEX_LOG_INFO, In_Fmt, args);
		va_end(args);
	}

	inline void LogWarning(const char *In_Fmt, ...)
	{
		if(!g_EngineAPI || !g_EngineAPI->LogV)
			return;
		va_list args;
		va_start(args, In_Fmt);
		g_EngineAPI->LogV(FORGEX_LOG_WARN, In_Fmt, args);
		va_end(args);
	}

	inline void LogError(const char *In_Fmt, ...)
	{
		if(!g_EngineAPI || !g_EngineAPI->LogV)
			return;
		va_list args;
		va_start(args, In_Fmt);
		g_EngineAPI->LogV(FORGEX_LOG_ERROR, In_Fmt, args);
		va_end(args);
	}

	inline bool IsValid(_In_opt_ void *In_Ptr, _In_ const char *In_Message = "Not Valid")
	{
		if(In_Ptr == nullptr)
		{
			LogError("Null Pointer Exception: {}", In_Message);
			return false;
		}

		return true;
	}

	inline void Assert(_In_ bool In_Condition, _In_ const char *In_Message = "Assertion failed")
	{
		if(!In_Condition)
		{
			LogError(In_Message);
		}
	}

}