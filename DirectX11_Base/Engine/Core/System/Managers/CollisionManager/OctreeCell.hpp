/*+===================================================================
	File: OctreeCell.hpp
	Summary: 8分木の分割された空間ひとつを表すクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/1 Mon AM 01:17:30 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//  前方宣言
// ==============================
class TreeData;

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// OctreeCellクラス
/// </summary>
class OctreeCell
{
public:
	OctreeCell();
	~OctreeCell();

	void ResetLink(_In_ TreeData *In_spTree) noexcept;

	bool Push(_In_ TreeData *In_spTree, _In_ int In_MortonNum) noexcept;

	TreeData *GetFirstObj() noexcept
	{
		return m_spLatest;
	}

	bool OnRemove(_In_ TreeData *In_pTree) noexcept;

	void Clear() noexcept;

private:
	TreeData *m_spLatest; // 最新のTreeDataへのポインタ
};
