/*+===================================================================
	File: EngineAPI_thunk.cpp
	Summary: EngineAPIÇÃé¿ëÃ
	Author: ê¬ñÿóYàÍòY
	Date: 2026/04/14 Tue PM 12:13:27 èââÒçÏê¨
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

static void ForgeXEngineAPI_POD Engine_Log(const char *In_Fmt, va_list In_Args)
{
	char buf[2048];
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, In_Fmt ? In_Fmt : "", In_Args);
	DebugManager::GetInstance().DebugLog("{}", buf);
}

static void ForgeXEngineAPI_POD Engine_LogWarning(const char *In_Fmt, va_list In_Args)
{
	char buf[2048];
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, In_Fmt ? In_Fmt : "", In_Args);
	DebugManager::GetInstance().DebugLog("[Warning] {}", buf);
}

static void ForgeXEngineAPI_POD Engine_LogError(const char *In_Fmt, va_list In_Args)
{
	char buf[2048];
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, In_Fmt ? In_Fmt : "", In_Args);
	DebugManager::GetInstance().DebugLog("[Error] {}", buf);
}
