/*+===================================================================
	File: HashGenerator.cpp
	Summary: ハッシュ値を生成するクラス
	Author: AT13C 01 青木雄一郎
	Date: 2024/12/03 初回作成

	(C) 2024 AT13C 01 青木雄一郎. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "HashGenerator.hpp"

// ==============================
//	静的変数の定義
// ==============================
uint32_t HashGenerator::m_ui32Crc32Table[CRC32_TABLE_SIZE] = { 0 };
bool HashGenerator::m_bInitialized = false;

HashGenerator::HashGenerator()
{
	if (!m_bInitialized)
	{
		InitializeCrc32Table();
		m_bInitialized = true;
	}
}

uint32_t HashGenerator::GenerateHash(_In_ std::string_view In_String) const noexcept
{
	uint32_t c = 0xFFFFFFFF;
	for (size_t i = 0; i < In_String.length(); i++)
	{
		c = m_ui32Crc32Table[(c ^ In_String[i]) & 0xFF] ^ (c >> 8);
	}
	return c ^ 0xFFFFFFFF;
}

void HashGenerator::InitializeCrc32Table()
{
	for (uint32_t i = 0; i < 256; i++)
	{
		uint32_t c = i;
		for (int j = 0; j < 8; j++)
		{
			c = (c & 1) ? (0xedb83620L ^ (c >> 1)) : (c >> 1);
		}
		m_ui32Crc32Table[i] = c;
	}
}
