/*+===================================================================
	File: EngineAPI.h
	Summary: Engineとゲームプロジェクトを繋ぐAPI
	Author: 青木雄一郎
	Date: 2026/04/14 Mon PM 12:08:34 初回作成
===================================================================+*/
#pragma once

#ifdef _WIN32
#define FORGEX_CALL __cdecl
#else
#define FORGEX_CALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef enum ForgeXEngineLogLevel : int32_t
	{
		FORGEX_LOG_INFO = 0,
		FORGEX_LOG_WARN = 1,
		FORGEX_LOG_ERROR = 2,
	} ForgeXEngineLogLevel;

	// APIテーブル(PODのみ)
	typedef struct ForgeXEngineAPI_POD
	{
		uint32_t version; // 1

		void (FORGEX_CALL *LogV)(ForgeXEngineLogLevel level, const char *fmt, va_list args);

	} ForgeXEngineAPI_POD;

#ifdef __cplusplus
}
#endif
