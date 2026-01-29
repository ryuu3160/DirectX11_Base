/*+===================================================================
	File: EngineUtility.hpp
	Summary: エンジン用のユーティリティ関数群
	Author: AT13C192 01 青木雄一郎
	Date: 2026/1/23 Fri AM 06:02:37 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility.hpp"

namespace Engine
{
	namespace Debug
	{
        // ================================
        //  Logging
        // ================================

		/// <summary>
		/// デバッグメッセージをログに出力します
		/// </summary>
		/// <param name="In_Msg">ログに出力するメッセージ</param>
		void DebugLog(_In_ std::string_view In_Message);

		/// <summary>
		/// デバッグ用の警告メッセージをログに記録します
		/// </summary>
		/// <param name="In_Msg">ログに記録する警告メッセージ</param>
		void DebugLogWarning(_In_ std::string_view In_Message);

		/// <summary>
		/// デバッグ用のエラーメッセージをログに記録します
		/// </summary>
		/// <param name="In_Msg">ログに記録するエラーメッセージ</param>
		void DebugLogError(_In_ std::string_view In_Message);

        template<typename... Args>
		void DebugLog(_In_ std::string_view In_Format, Args&&... In_Args)
		{
			auto Msg = FormatString(In_Format, In_Args...);
			DebugLog(Msg);
		}

        template<typename... Args>
		void DebugLogWarning(_In_ std::string_view In_Format, Args&&... In_Args)
		{
			auto Msg = FormatString(In_Format, In_Args...);
			DebugLogWarning(Msg);
		}

        template<typename... Args>
		void DebugLogError(_In_ std::string_view In_Format, Args&&... In_Args)
		{
			auto Msg = FormatString(In_Format, In_Args...);
			DebugLogError(Msg);
		}
	}

    // ================================
    //  Utility
    // ================================

    /// <summary>
    /// ポインターがnullかどうかをチェックします。
    /// </summary>
    /// <param name="In_Ptr">チェックするポインター。</param>
    /// <param name="In_Message">チェック時に使用するメッセージ。</param>
    /// <returns>ポインターがnullの場合はtrue、それ以外の場合はfalse。</returns>
    bool IsValid(_In_opt_ void *In_Ptr, _In_ std::string_view In_Message = "nullptr error");

    /// <summary>
    /// 条件をアサートします。
    /// </summary>
    /// <param name="In_Condition">チェックする条件。</param>
    /// <param name="In_Message">アサートが失敗した場合に表示するメッセージ。</param>
    void Assert(_In_ bool In_Condition, _In_ std::string_view In_Message = "Assertion failed");

} // namespace Engine

using namespace Engine;
