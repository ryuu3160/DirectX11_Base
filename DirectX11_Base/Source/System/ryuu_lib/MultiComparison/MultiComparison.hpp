/*+===================================================================
	File: MultiComparison.hpp
	Summary: ビット演算による多重比較クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat AM 04:32:18 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <unordered_map>
#include <initializer_list>

/// <summary>
/// MultiComparisonクラス
/// </summary>
template <typename T>
class MultiComparison
{
public:
	MultiComparison() = default;
	~MultiComparison() = default;

	/// <summary>
	/// <para>initializer_listの要素をビット位置としてビットマスクを作成し、そのマスクと指定したキーで比較エントリを追加する</para>
	/// <para>既に同じキーが存在する場合は、そのエントリの比較ビットが新しいマスクで上書きされる</para>
	/// </summary>
	/// <param name="[In_Key]">比較エントリに関連付けるキー</param>
	/// <param name="[In_ComparList]">ビットマスク化する値の一覧</param>
	void MakeComparison(_In_ const T &In_Key, _In_ std::initializer_list<T> In_ComparList)
	{
		uint64_t Mask = 0;
		for (auto itr : In_List)
			Mask |= (1 << static_cast<uint64_t>(itr));

		AddComparison(In_Key, Mask);
	}

	/// <summary>
	/// 指定したキーに対して、指定した比較ビットが設定されているかを判定する
	/// </summary>
	/// <param name="[In_Key]">検索対象のキー。m_ComparisonMap で照合される</param>
	/// <param name="[In_Comparison]">判定する比較値。ビット位置として扱われ、対応するビットが設定されているかを確認する</param>
	/// <returns>キーが見つかり、対応するビットが設定されていればtrue、キーが存在しないかビットが設定されていなければfalseを返す</returns>
	bool CheckComparison(_In_ const T &In_Key, _In_ const T &In_Comparison)
	{
		auto itr = m_ComparisonMap.find(In_Key);
		if (itr == m_ComparisonMap.end())
			return false;
		uint64_t ComparisonBit = (1 << static_cast<uint64_t>(In_Comparison));
		return (itr->second & ComparisonBit) != 0;
	}

private:

	/// <summary>
	/// 指定したキーに対する比較ビットを m_ComparisonMap に挿入または更新する
	/// </summary>
	/// <param name="[In_Key]">マップのキーとして使用する値。対応するエントリがなければ新規挿入し、既存ならそのエントリが更新される</param>
	/// <param name="[In_ComparisonBit]">キーに関連付けて保存する比較ビット（uint64_t）。既存エントリがある場合はこの値で上書きされる</param>
	void AddComparison(_In_ const T &In_Key, _In_ const uint64_t &In_ComparisonBit)
	{
		auto result = m_ComparisonMap.try_emplace(In_Key, In_ComparisonBit);

		if(!result.second)
			result.first->second = In_ComparisonBit;
	}

private:
	std::unordered_map<T, uint64_t> m_ComparisonMap;
};
