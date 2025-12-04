/*+===================================================================
	File: TreeData.hpp
	Summary: ツリーデータクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/1 Mon AM 01:54:38 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//  前方宣言
// ==============================
class ColliderBase;
class OctreeCell;

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// TreeDataクラス
/// </summary>
class TreeData
{
	friend class ColliderBase;
public:
	TreeData();
	~TreeData();

	bool Remove();

	void RegistCell(_In_ OctreeCell *In_pCell, _In_ int In_MortonNum) noexcept;

	// ------------------------------
	//  Getter
	// ------------------------------
	inline std::shared_ptr<TreeData> GetNextTree() noexcept
	{
		return m_spNext;
	}

	inline int GetMortonNumber() const noexcept
	{
		return m_MortonNumber;
	}

	inline bool CompareCell(_In_ OctreeCell *In_pCell) const noexcept
	{
		if(m_pCell == nullptr || In_pCell == nullptr)
			return false;

		return m_pCell == In_pCell;
	}

	// ------------------------------
	// Setter
	// ------------------------------
	inline void SetNextTree(_In_ std::shared_ptr<TreeData> In_spNext) noexcept
	{
		m_spNext = In_spNext;
	}

	inline void SetPrevTree(_In_ std::shared_ptr<TreeData> In_spPrev) noexcept
	{
		m_spPrev = In_spPrev;
	}

private:
	int m_MortonNumber; // モートン番号
	OctreeCell *m_pCell; // 登録されている空間
	ColliderBase *m_pCollider; // 登録されているコライダー
	std::shared_ptr<TreeData> m_spPrev; // 前のTreeDataへのスマートポインタ
	std::shared_ptr<TreeData> m_spNext; // 次のTreeDataへのスマートポインタ
};
