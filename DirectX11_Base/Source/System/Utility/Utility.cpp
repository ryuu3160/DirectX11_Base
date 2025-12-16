/*+===================================================================
	File: Utility.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/3 Wed AM 02:21:35 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Utility.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

namespace Util
{
	std::string WideToUTF8(_In_ const std::wstring_view In_WideStr)
	{
		if(In_WideStr.empty()) return "";

		// まず必要なバッファサイズを取得
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, In_WideStr.data(), -1, nullptr, 0, nullptr, nullptr);
		std::string utf8Str(sizeNeeded, '\0');

		// UTF-8に変換
		WideCharToMultiByte(CP_UTF8, 0, In_WideStr.data(), -1, &utf8Str[0], sizeNeeded, nullptr, nullptr);
		return utf8Str;
	}
	std::wstring UTF8ToWide(_In_ const std::string_view In_UTF8Str)
	{
		// まず必要なバッファサイズを取得
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, nullptr, 0);
		std::wstring wideStr(sizeNeeded, L'\0'); // ワイド文字列を作成

		// UTF-8 → Wide文字 に変換
		MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, &wideStr[0], sizeNeeded);
		return wideStr;
	}
	std::string UTF16ToUTF8(_In_ const std::u16string_view In_UTF16Str)
	{
		std::string utf8Str;
		utf8Str.reserve(In_UTF16Str.size() * 2); // サイズをある程度予測して予約
		for(char16_t ch : In_UTF16Str)
		{
			if(ch <= 0x7F)
			{
				utf8Str.push_back(static_cast<char>(ch)); // 1バイト文字
			}
			else if(ch <= 0x7FF)
			{
				utf8Str.push_back(static_cast<char>(0xC0 | ((ch >> 6) & 0x1F))); // 2バイト文字
				utf8Str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
			}
			else
			{
				utf8Str.push_back(static_cast<char>(0xE0 | ((ch >> 12) & 0x0F))); // 3バイト文字
				utf8Str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
				utf8Str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
			}
		}
		return utf8Str;
	}
	std::u16string UTF8ToUTF16(_In_ const std::string_view In_UTF8Str)
	{
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, nullptr, 0);
		std::u16string utf16Str(sizeNeeded, u'\0');

		MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, reinterpret_cast<wchar_t *>(&utf16Str[0]), sizeNeeded);
		return utf16Str;
	}
	std::string ShiftJISToUTF8(_In_ const std::string_view In_ShiftJISStr)
	{
		// Shift-JIS → UTF-16 に変換
		int wideCharSize = MultiByteToWideChar(CP_ACP, 0, In_ShiftJISStr.data(), -1, nullptr, 0);
		std::wstring wideStr(wideCharSize, L'\0');
		MultiByteToWideChar(CP_ACP, 0, In_ShiftJISStr.data(), -1, &wideStr[0], wideCharSize);

		// UTF-16 → UTF-8 に変換
		int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string utf8Str(utf8Size, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);

		return utf8Str;
	}
}