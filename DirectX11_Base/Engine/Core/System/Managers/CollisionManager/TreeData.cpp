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
#include "Engine/DebugEditor.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

TreeData::TreeData()
	: m_pCell(nullptr), m_pCollider(nullptr)
	, m_spPrev(nullptr), m_spNext(nullptr)
	, m_MortonNumber(-1)
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

	TreeData *prev = m_spPrev;
	TreeData *next = m_spNext;
	OctreeCell *cell = m_pCell;

	// 自身を登録している空間に自信を通知
	if(cell && !cell->OnRemove(this))
		return false;

	// 逸脱処理
	// 前後のオブジェクトを結び付ける
	if(prev != nullptr)
	{
		prev->m_spNext = next;
	}
	if(next != nullptr)
	{
		next->m_spPrev = prev;
	}

	// 自身のリンクをリセット
	m_spPrev = nullptr;
	m_spNext = nullptr;
	m_pCell = nullptr;
	m_pCollider = nullptr;

	return true;
}

void TreeData::RegistCell(_In_ OctreeCell *In_pCell, _In_ int In_MortonNum) noexcept
{
	m_pCell = In_pCell;
	m_MortonNumber = In_MortonNum;
}
