/*+===================================================================
	File: HashGenerator.hpp
	Summary: ハッシュ値を生成するクラス
	Author: ryuu3160
	Date: 2024/12/03 初回作成

	(C) 2024 ryuu3160. All rights reserved.
===================================================================+*/
#pragma once

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

	/// <summary>
	/// デストラクタ
	/// </summary>
	~HashGenerator();

	uint32_t GenerateHash(std::string In_String);

private:
	uint32_t *m_pui32Crc32Table;
};
