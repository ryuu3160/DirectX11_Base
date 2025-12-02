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

// ===============================
//  定数
// ===============================
namespace
{
    LPCSTR lpClassName = "DX11Base";	// ウィンドウクラス名
    constexpr int cx_nWINDOW_WIDTH = 1280;			// ウィンドウ幅
    constexpr int cx_nWINDOW_HEIGHT = 720;			// ウィンドウ高さ
}

// ==============================
//  列挙
// ==============================
enum class NullpCheckMode : unsigned char
{
	NONE = 0,		// 何も表示しない
	OUTPUT,	    // コンソールに出力する
	MSGBOX,		// メッセージボックスを表示する
};
using NCMode = NullpCheckMode; // NullpCheckModeのエイリアス

// ==============================
//	using宣言
// ==============================
using FilePath = std::string_view;  // ファイルパスを表す型。std::string_viewを使用して、文字列の参照を保持する
using FilePathHold = std::string;   // ファイルパスを表す型。std::stringを使用して、文字列の所有権を持つ(メンバー変数の型に使用)

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

/// <summary>
/// 指定されたポインタがヌルかどうかをチェックし、モードに応じてエラーメッセージを出力します。
/// </summary>
/// <param name="[In_pData]">チェック対象のポインタ。</param>
/// <param name="[In_Mode]">ヌル時の動作モード（デフォルトはNullpCheckMode::NONE）。</param>
/// <param name="[In_strMessage]">エラー時に出力するメッセージ（省略可能）。</param>
/// <returns>ポインタがヌルの場合はtrue、それ以外はfalse。</returns>
inline bool NullCheck(_In_opt_ void *In_pData,_In_ NullpCheckMode In_Mode = NullpCheckMode::NONE, _In_ std::string_view In_strMessage = "") noexcept
{
    if (In_pData == nullptr)
    {
        switch (In_Mode)
        {
        case NullpCheckMode::NONE:
            return true;
        case NullpCheckMode::OUTPUT:
        {
            std::string message = "error : " + std::string(In_strMessage);
            OutputDebugStringA(message.c_str());
            return true;
        }
            case NullpCheckMode::MSGBOX:
            MessageBoxA(nullptr, In_strMessage.data(), "Error", MB_OK);
			return true;
        default:
		    return true;
        }
    }
    return false;
}

template<TypePOD T>
std::string ToString(_In_ T In_Value) noexcept
{
    std::string result;
	int size = std::numeric_limits<T>::digits10 + 2; // 符号と終端文字分を追加
	result.resize(size);
	std::to_chars(result.data(), result.data() + result.size(), In_Value);
    return result;
}

template<TypePOD T>
std::string ToString(_In_ T In_Value, _In_ int In_Precision) noexcept
{
    std::string result;
    int size = std::numeric_limits<T>::digits10 + 2; // 符号と終端文字分を追加
    result.resize(size);
    std::to_chars(result.data(), result.data() + result.size(), In_Value, std::chars_format::fixed, In_Precision);
    return result;
}

template<typename T>
T FromString(_In_ const std::string_view &In_Str) noexcept
{
    T result{};
    std::from_chars(In_Str.data(), In_Str.data() + In_Str.size(), result);
    return result;
}
