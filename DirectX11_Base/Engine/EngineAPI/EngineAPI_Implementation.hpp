/*+===================================================================
	File: EngineAPI_Implementation.hpp
	Summary: EngineAPIÇÃé¿ëÃ
	Author: ê¬ñÿóYàÍòY
	Date: 2026/04/14 Tue PM 12:13:27 èââÒçÏê¨
===================================================================+*/

// ==============================
//	include
// ==============================
#include "EngineAPI/EngineAPI.h"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

static void FORGEX_CALL Engine_LogV(ForgeXEngineLogLevel In_Level, const char *In_Fmt, va_list In_Args)
{
	char buf[2048];
	const char *safeFmt = In_Fmt ? In_Fmt : "";
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, safeFmt, In_Args);

	switch(In_Level)
	{
	case FORGEX_LOG_WARN:
		DebugManager::GetInstance().DebugLogWarning("{}", buf);
		break;
	case FORGEX_LOG_ERROR:
		DebugManager::GetInstance().DebugLogError("{}", buf);
		break;
	default:
		DebugManager::GetInstance().DebugLog("{}", buf);
		break;
	}
}

