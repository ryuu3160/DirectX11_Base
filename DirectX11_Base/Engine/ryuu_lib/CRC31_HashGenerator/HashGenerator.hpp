/*+===================================================================
	File: HashGenerator.hpp
	Summary: ハッシュ値を生成するクラス
	Author: AT13C 01 青木雄一郎
	Date: 2024/12/03 初回作成

	(C) 2024 AT13C 01 青木雄一郎. All rights reserved.
===================================================================+*/
#pragma once

// ==============================
//  定数定義
// ==============================
namespace
{
	constexpr int CRC32_TABLE_SIZE = 256;
}

/// <summary>
/// HashGeneratorクラス
/// </summary>
class HashGenerator
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HashGenerator();
	~HashGenerator() = default;

	uint32_t GenerateHash(_In_ std::string_view In_String) const noexcept;

	size_t operator()(std::string_view key) const noexcept
	{
		return static_cast<size_t>(GenerateHash(key));
	}

	// ヘテロジニアス lookup を有効にするためのマーキング
	using is_transparent = void;


private:
	static void InitializeCrc32Table();
private:
	static bool m_bInitialized;
	static uint32_t m_ui32Crc32Table[CRC32_TABLE_SIZE];
};

struct StringEq {
	using is_transparent = void;
	bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
	bool operator()(const std::string &a, const std::string &b) const noexcept { return a == b; }
	// 必要なら他の組み合わせもテンプレートで受けられるようにする
	template<class A, class B>
	bool operator()(const A &a, const B &b) const noexcept
	{
		return std::string_view(a) == std::string_view(b);
	}
};
