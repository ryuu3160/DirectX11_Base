/*+===================================================================
	File: OctreeCell.cpp
	Summary: 8分木の分割された空間ひとつを表すクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/1 Mon AM 01:17:37 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "OctreeCell.hpp"
#include "TreeData.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

OctreeCell::OctreeCell()
	: m_spLatest(nullptr)
{
}

OctreeCell::~OctreeCell()
{
	Clear();
}

void OctreeCell::ResetLink(_In_ TreeData *In_spTree) noexcept
{
	if(!In_spTree)
		return;

	if(In_spTree->GetNextTree() != nullptr)
		ResetLink(In_spTree->GetNextTree());
	In_spTree->Remove();
}

bool OctreeCell::Push(_In_ TreeData *In_spTree, _In_ int In_MortonNum) noexcept
{
	if (!In_spTree)
		return false; // 無効オブジェクトは登録しない
	if (In_spTree->CompareCell(this))
		return false; // すでに登録されている場合は登録しない

	if (!m_spLatest)
	{
		m_spLatest = In_spTree;
	}
	else
	{
		// 最新のツリーオブジェクトを更新
		In_spTree->SetNextTree(m_spLatest);
		m_spLatest->SetPrevTree(In_spTree);
		m_spLatest = In_spTree;
	}
	In_spTree->RegistCell(this,In_MortonNum); // 空間を登録
	return true;
}

bool OctreeCell::OnRemove(_In_ TreeData *In_pTree) noexcept
{
	// 削除対象が先頭ノードの場合
	if(m_spLatest == In_pTree)
	{
		// 次のオブジェクトにすげ替え
		if(m_spLatest != nullptr)
			m_spLatest = m_spLatest->GetNextTree();
	}
	return true;
}

void OctreeCell::Clear() noexcept
{
	// リンクリストのすべてのノードを削除
	while(m_spLatest != nullptr)
	{
		TreeData *current = m_spLatest;
		m_spLatest = current->GetNextTree();

		// TreeDataを削除
		current->Remove();
	}
}
