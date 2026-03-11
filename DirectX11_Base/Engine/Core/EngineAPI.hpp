/*+===================================================================
	File: EngineAPI.hpp
	Summary: エンジンAPI エクスポート/インポート定義
			 ENGINE_EXPORTS が定義されている場合：エンジンビルド時(dllexport)
			 ENGINE_EXPORTS が定義されていない場合：ゲームプロジェクトから参照時(dllimport)
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/11 Wed PM 11:45:52 初回作成
===================================================================+*/
#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

// テンプレートクラス用(インライン実装のみの場合は不要)
#define ENGINE_API_TEMPLATE
