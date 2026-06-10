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
private:
	// CRC-32(IEEE 802.3)逆多項式
	static constexpr uint32_t CRC32_POLY = 0xEDB88320u;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HashGenerator()
	{
		if(!m_bInitialized)
		{
			InitializeCrc32Table();
			m_bInitialized = true;
		}
	}
	~HashGenerator() = default;

	uint32_t GenerateHash(_In_ std::string_view In_String) const noexcept
	{
		uint32_t c = 0xFFFFFFFF;
		for(size_t i = 0; i < In_String.length(); i++)
		{
			c = m_ui32Crc32Table[(c ^ In_String[i]) & 0xFF] ^ (c >> 8);
		}
		return c ^ 0xFFFFFFFF;
	}

	size_t operator()(std::string_view key) const noexcept
	{
		return static_cast<size_t>(GenerateHash(key));
	}

	// ヘテロジニアス lookup を有効にするためのマーキング
	using is_transparent = void;


private:
	static void InitializeCrc32Table()
	{
		for(uint32_t i = 0; i < 256; ++i)
		{
			uint32_t c = i;
			for(int j = 0; j < 8; ++j)
			{
				c = (c & 1) ? (CRC32_POLY ^ (c >> 1)) : (c >> 1);
			}
			m_ui32Crc32Table[i] = c;
		}
	}
private:
	static inline bool m_bInitialized = false;
	static inline uint32_t m_ui32Crc32Table[CRC32_TABLE_SIZE] = { 0 };
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
