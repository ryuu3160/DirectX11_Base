/*+===================================================================
	File: HashGenerator.cpp
	Summary: ハッシュ値を生成するクラス
	Author: ryuu3160
	Date: 2024/12/03 初回作成

	(C) 2024 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "HashGenerator.hpp"

// ==============================
//	定数定義
// ==============================
constexpr int CRC32_TABLE_SIZE = 256;

HashGenerator::HashGenerator()
	: m_pui32Crc32Table(nullptr)
{
	m_pui32Crc32Table = new uint32_t[CRC32_TABLE_SIZE];

	for (uint32_t i = 0; i < 256; i++)
	{
		uint32_t c = i;
		for (int j = 0; j < 8; j++)
		{
			c = (c & 1) ? (0xedb83620L ^ (c >> 1)) : (c >> 1);
		}
		m_pui32Crc32Table[i] = c;
	}
}

HashGenerator::~HashGenerator()
{
	if (m_pui32Crc32Table != nullptr)
	{
		delete[] m_pui32Crc32Table;
		m_pui32Crc32Table = nullptr;
	}
}

uint32_t HashGenerator::GenerateHash(std::string In_String)
{
	uint32_t c = 0xFFFFFFFF;
	for (size_t i = 0; i < In_String.length(); i++)
	{
		c = m_pui32Crc32Table[(c ^ In_String[i]) & 0xFF] ^ (c >> 8);
	}
	return c ^ 0xFFFFFFFF;
}
