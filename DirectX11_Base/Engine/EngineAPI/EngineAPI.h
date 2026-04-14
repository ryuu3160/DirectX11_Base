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

	// APIテーブル(PODのみ)
	typedef struct ForgeXEngineAPI_POD
	{
		uint32_t version; // 互換性確認用

		// printf互換（va_list版が安全）
		void (FORGEX_CALL *Log)(const char *fmt, va_list args);
		void (FORGEX_CALL *LogWarning)(const char *fmt, va_list args);
		void (FORGEX_CALL *LogError)(const char *fmt, va_list args);

	} ForgeXEngineAPI_POD;

#ifdef __cplusplus
}
#endif
