/*+===================================================================
	File: Defines.hpp
	Summary: 共通で使用出来る定義や列挙型、構造体、定数などを定義するヘッダーファイル
	Author: AT13C192 01 青木雄一郎
	Date: 2025/6/22 Sun PM 11:21:46 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// C++20以降の機能を利用するための条件付きインクルード
#if __cplusplus >= 202002L
#include <format>
#endif



// ==============================
//	using宣言
// ==============================
using FilePath = std::string_view; // ファイルパスを表す型。std::string_viewを使用して、文字列の参照を保持する

// ==============================
//  マクロ/関数定義
// ==============================

#if __cplusplus <= 201703L
// エラーメッセージを表示する関数
template<typename... Args>
inline void Error(std::string_view format, Args&&... args)
{
    // 必要なバッファサイズを取得
    int required = std::snprintf(nullptr, 0, format.data(), args...);

    if (required < 0)
    {
        MessageBoxA(nullptr, "Format error", "Error", MB_OK);
        return;
    }

    std::vector<char> buf(required + 1);
    std::snprintf(buf.data(), buf.size(), format.data(), args...);

    MessageBoxA(nullptr, buf.data(), "Error", MB_OK);
}
#elif __cplusplus >= 202002L
// C++20以降のstd::formatを使用してエラーメッセージを表示する関数
template<typename... Args>
inline void Error(std::string_view In_Format, Args&&... In_Args)
{
	std::string msg = std::vformat(In_Format, std::make_format_args(In_Args...));
	MessageBoxA(nullptr, msg.c_str(), "Error", MB_OK);
}
#endif

