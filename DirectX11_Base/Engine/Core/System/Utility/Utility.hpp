/*+===================================================================
	File: Utility.hpp
	Summary: 汎用的な処理をまとめたもの
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/3 Wed AM 02:21:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Concept.hpp"
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
//	定数定義
// ==============================
namespace
{
}

namespace Util
{
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
    /// 指定されたポインタがヌルかどうかをチェックし、モードに応じてエラーメッセージを出力します
    /// </summary>
    /// <param name="[In_pData]">チェック対象のポインタ</param>
    /// <param name="[In_Mode]">ヌル時の動作モード(デフォルトはNullpCheckMode::NONE)</param>
    /// <param name="[In_strMessage]">エラー時に出力するメッセージ(省略可能)</param>
    /// <returns>ポインタがヌルの場合はtrue、それ以外はfalse</returns>
    inline bool NullCheck(_In_opt_ void *In_pData, _In_ NullpCheckMode In_Mode = NullpCheckMode::NONE, _In_ std::string_view In_strMessage = "") noexcept
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

    /// <summary>
	/// 値を文字列に変換して返します
    /// </summary>
    /// <typeparam name="T">変換対象の型。TypePOD(POD／数値型など)という制約を満たす必要があります</typeparam>
    /// <param name="[In_Value]">変換する値(PODや数値型の引数を想定しています)</param>
    /// <returns>In_Value の文字列表現を格納した std::string を返します</returns>
    template<TypePOD T>
    std::string ToString(_In_ T In_Value) noexcept
    {
        std::string result;
        int size = std::numeric_limits<T>::digits10 + 2; // 符号と終端文字分を追加
        result.resize(size);
        auto res = std::to_chars(result.data(), result.data() + result.size(), In_Value);
		result.resize(res.ptr - result.data()); // 実際に使用されたサイズに調整
        return result;
    }

    /// <summary>
    /// 数値を指定した小数精度で固定表記にフォーマットし、文字列として返します
    /// </summary>
    /// <typeparam name="T">変換する数値の型(PODの数値型を指定します)</typeparam>
    /// <param name="[In_Value]">変換対象の数値(数値型(例: float, double, int)を想定します)</param>
    /// <param name="[In_Precision]">小数点以下の桁数(表示する精度)</param>
    /// <returns>指定した精度で固定小数点表記に整形されたstd::stringを返します</returns>
    template<TypePOD T>
    std::string ToString(_In_ T In_Value, _In_ int In_Precision) noexcept
    {
        std::string result;
        int size = std::numeric_limits<T>::digits10 + 2; // 符号と終端文字分を追加

		// 小数点以下の桁数に応じてサイズを調整
        if(In_Precision > size - 2)
			size += In_Precision - (size - 2);

        result.resize(size);
        auto res = std::to_chars(result.data(), result.data() + result.size(), In_Value, std::chars_format::fixed, In_Precision);
		result.resize(res.ptr - result.data()); // 実際に使用されたサイズに調整
        return result;
    }

    /// <summary>
    /// 文字列を型Tの値に変換します
    /// </summary>
    /// <typeparam name="T">変換先の型(整数や浮動小数点など)</typeparam>
    /// <param name="[In_Str]">解析対象の入力文字列(std::string_view)</param>
    /// <returns>解析結果の値(POD型)、解析に失敗した場合は初期化されたTの値が返されます</returns>
    template<TypePOD T>
    T FromString(_In_ std::string_view In_Str) noexcept
    {
        T result{};
        std::from_chars(In_Str.data(), In_Str.data() + In_Str.size(), result);
        return result;
    }

    /// <summary>
	/// ワイド文字列をUTF-8文字列に変換します
    /// </summary>
    /// <param name="[In_WideStr]">変換するワイド文字列(例:L"文字")</param>
    /// <returns>UTF8に変換したstd::string型の文字列</returns>
    std::string WideToUTF8(_In_ const std::wstring_view In_WideStr);

	/// <summary>
	/// UTF-8文字列をワイド文字列に変換します
	/// </summary>
    /// <param name="[In_UTF8Str]">変換するUTF-8文字列</param>
	/// <returns>>ワイド文字列に変換したstd::wstring型の文字列</returns>
	std::wstring UTF8ToWide(_In_ std::string_view In_UTF8Str);

	/// <summary>
	/// UTF-16文字列をUTF-8文字列に変換します
	/// </summary>
	/// <param name="[In_UTF16Str]">>変換するUTF-16文字列</param>
	/// <returns>>UTF8に変換したstd::string型の文字列</returns>
	std::string UTF16ToUTF8(_In_ const std::u16string_view In_UTF16Str);

	/// <summary>
	/// UTF-8文字列をUTF-16文字列に変換します
	/// </summary>
	/// <param name="[In_UTF8Str]">変換するUTF-8文字列</param>
	/// <returns>>UTF16に変換したstd::u16string型の文字列</returns>
	std::u16string UTF8ToUTF16(_In_ std::string_view In_UTF8Str);

	/// <summary>
	/// Shift-JIS文字列をUTF-8文字列に変換します
	/// </summary>
	/// <param name="[In_ShiftJISStr]">変換するShift-JIS文字列</param>
	/// <returns>>UTF8に変換したstd::string型の文字列</returns>
	std::string ShiftJISToUTF8(_In_ std::string_view In_ShiftJISStr);

    template<typename ...Args>
	std::string FormatString(_In_ std::string_view In_Format, Args&& ...In_Args)
    {
        return std::vformat(In_Format, std::make_format_args(In_Args...));
    }
}

using namespace Util; // Util名前空間をグローバルに展開
