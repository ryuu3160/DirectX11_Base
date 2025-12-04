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
{
}

OctreeCell::~OctreeCell()
{
}

void OctreeCell::ResetLink(_In_ std::shared_ptr<TreeData> In_spTree) noexcept
{
	if(In_spTree->GetNextTree().get() != nullptr)
		ResetLink(In_spTree->GetNextTree());
	In_spTree.reset();
}

bool OctreeCell::Push(_In_ std::shared_ptr<TreeData> In_spTree, _In_ int In_MortonNum) noexcept
{
	if (In_spTree.get() == nullptr)
		return false; // 無効オブジェクトは登録しない
	if (In_spTree->CompareCell(this))
		return false; // すでに登録されている場合は登録しない

	if (m_spLatest.get() != nullptr)
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
	if (m_spLatest.get() == In_pTree)
	{
		// 次のオブジェクトにすげ替え
		if(m_spLatest.get() != nullptr)
			m_spLatest = m_spLatest->GetNextTree();
	}
	return true;
}
