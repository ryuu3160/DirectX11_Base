/*+===================================================================
	File: TreeData.cpp
	Summary: ツリーデータクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/1 Mon AM 01:54:30 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "TreeData.hpp"
#include "OctreeCell.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

TreeData::TreeData()
	: m_pCell(nullptr), m_pCollider(nullptr)
{
}

TreeData::~TreeData()
{
	m_pCell = nullptr;
	m_pCollider = nullptr;
}

bool TreeData::Remove()
{
	// 既に逸脱しているときは処理終了
	if (!m_pCell)
		return false;

	// 自身を登録している空間に自信を通知
	if(!m_pCell->OnRemove(this))
		return false;

	// 逸脱処理
	// 前後のオブジェクトを結び付ける
	if (m_spPrev.get() != nullptr)
	{
		m_spPrev->m_spNext = m_spNext;
		m_spPrev.reset();
	}
	if (m_spNext.get() != nullptr)
	{
		m_spNext->m_spPrev = m_spPrev;
		m_spNext.reset();
	}
	m_pCell = nullptr;
	return true;
}

void TreeData::RegistCell(_In_ OctreeCell *In_pCell, _In_ int In_MortonNum) noexcept
{
	m_pCell = In_pCell;
	m_MortonNumber = In_MortonNum;
}
