/*+===================================================================
	File: CollisionManager.cpp
	Summary: 当たり判定管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 05:05:24 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CollisionManager.hpp"
#include "System/CollisionManager/OctreeCell.hpp"
#include "System/CollisionManager/TreeData.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

void CollisionManager::InitOctreeSpace(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack, _In_ const int In_Level) noexcept
{
	if (In_Level < 0 || In_Level >= cx_MaxLevel)
	{
		DebugManager::GetInstance().DebugLogError("CollisionManager::InitOctreeSpace(): 指定されたレベルが不正です。Level=%d", In_Level);
		return;
	}

	// 各レベルでの空間数を算出
	// ルートは1、その子は4、さらにその子（孫）は16、と4^nで増えていく
	// ※ Octreeの場合は底が8となり、1, 8, 64, 512・・となる。
	m_Pow[0] = 1;
	for (int i = 1; i < cx_MaxLevel + 1; i++)
	{
		m_Pow[i] = m_Pow[i - 1] * cx_DivisionNumber;
	}

	// level（0基点）の線形配列作成
	// e.g.)
	// 0レベル（ルート）ならセル数は1
	// 1レベルならセル数は9（(64 - 1) / 7）
	// 2レベルならセル数は73（(512 - 1) / 7）
	// 割る数は分割数から1を引いた値。等比数列を利用して求める。
	int denom = cx_DivisionNumber - 1;
	m_MaxCellNum = (m_Pow[In_Level + 1] - 1) / denom;
	m_OctreeCells.resize(m_MaxCellNum);

	// 有効領域を登録
	// 左上手前の座標と幅、高さ、深度を保持
	m_LeftTopFront = In_LeftTopFront;
	m_RightBottomBack = In_RightBottomBack;
	m_Width = In_RightBottomBack.x - In_LeftTopFront.x;
	m_Height = In_LeftTopFront.y - In_RightBottomBack.y;
	m_Depth = In_RightBottomBack.z - In_LeftTopFront.z;

	// 分割数に応じた単位幅と単位高を求める
	// 分割数はlevelを指数とした2の累乗分増えてくため、
	// シフトで求めた数で割ることで単位を求める
	// 0レベルなら分割は1、1レベルなら分割は2（2^1）、2レベルなら4（2^2）
	int unit = 1 << In_Level;
	m_Unit.x = m_Width / unit;
	m_Unit.y = m_Height / unit;
	m_Unit.z = m_Depth / unit;

	m_OctreeLevel = In_Level;

	m_ParentShift = static_cast<int>(std::log2(cx_DivisionNumber));
}

void CollisionManager::AddColliderComponent(_In_ ColliderBase *In_Collider) noexcept
{
	// コライダーリストに追加
	RegisterObjectToOctree(In_Collider);
}

void CollisionManager::RemoveColliderComponent(_In_ ColliderBase *In_Collider)
{
	// 衝突ペアリストから削除
	RemoveColliderPair(In_Collider);
	// コライダーリストから削除
	In_Collider->GetTreeData()->Remove();
}

void CollisionManager::UpdateCollisionCells(_In_ ColliderBase *In_Collider)
{
	// セルが変化した場合、再登録
	int Elem = GetMortonNumberOfCollider(In_Collider);
	if (Elem != In_Collider->GetTreeData()->GetMortonNumber())
	{
		// 以前のセルから削除
		In_Collider->GetTreeData()->Remove();
		// 新しいセルに登録
		RegisterObjectToOctree(In_Collider);
	}
}

void CollisionManager::CheckAllCollisions() noexcept
{
	ColliderPairMap ColPairs;
	int ColNum = 0;
	// 衝突判定リストを作成する
	ColNum = GetAllCollisionList(ColPairs);

	// 既に登録されているペアを更新
	m_ColliderPairList.merge(ColPairs);

	// コライダー同士の当たり判定をチェック
	for(auto itr = m_ColliderPairList.begin(); itr != m_ColliderPairList.end();)
	{
		if(itr->first.pColliderA->CheckCollision(itr->first.pColliderB))
		{
			auto *pColliderA = itr->first.pColliderA;
			auto *pColliderB = itr->first.pColliderB;
			// 前回も当たっていた場合はStayコールバックを呼び出す
			if(itr->second)
			{
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderB,[pColliderA](ColliderBase *other) { pColliderA->CallOnStay(other); } });
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderA,[pColliderB](ColliderBase *other) { pColliderB->CallOnStay(other); } });
			}
			else
			{
				itr->second = true;
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderB,[pColliderA](ColliderBase *other) { pColliderA->CallOnEnter(other); } });
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderA,[pColliderB](ColliderBase *other) { pColliderB->CallOnEnter(other); } });
			}
			++itr;
		}
		else
		{
			// 前フレームで当たっていた場合はExitコールバックを呼び出す
			if(itr->second)
			{
				auto *pColliderA = itr->first.pColliderA;
				auto *pColliderB = itr->first.pColliderB;
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderB,[pColliderA](ColliderBase *other) { pColliderA->CallOnExit(other); } });
				m_CollisionCallbacks.emplace_back(ColliderCallbackInfo{ pColliderA,[pColliderB](ColliderBase *other) { pColliderB->CallOnExit(other); } });
			}
			// 当たっていなかった場合はフラグをリセット
			itr->second = false;

			itr = m_ColliderPairList.erase(itr);
		}
	}
}

void CollisionManager::CallAllCollisionCallbacks()
{
	for(auto &itr : m_CollisionCallbacks)
	{
		itr.Callback(itr.pOther);
	}
	m_CollisionCallbacks.clear();
}

CollisionManager::CollisionManager()
	: m_OctreeLevel(0), m_MaxCellNum(0), m_ParentShift(0)
	, m_Width(0.0f), m_Height(0.0f), m_Depth(0.0f)
	, m_Unit{ 0.0f,0.0f,0.0f }, m_LeftTopFront{ 0.0f,0.0f,0.0f }, m_RightBottomBack{ 0.0f,0.0f,0.0f }
	, m_Pow{}
{
}

CollisionManager::~CollisionManager()
{
	for(auto &cell : m_OctreeCells)
	{
		if (cell)
		{
			delete cell;
			cell = nullptr;
		}
	}
	m_OctreeCells.clear();
}

bool CollisionManager::CreateNewCell(_In_ int In_MortonNumber)
{
	// 引数の要素番号
	while (!m_OctreeCells[In_MortonNumber])
	{
		// 指定の要素番号に空間を新規作成
		m_OctreeCells[In_MortonNumber] = new OctreeCell;

		// 親空間にジャンプ
		In_MortonNumber = (In_MortonNumber - 1) >> 3;
		if (In_MortonNumber >= m_MaxCellNum || In_MortonNumber < 0)
			break;
	}
	return true;
}

int CollisionManager::GetMortonNumber(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack)
{
	// 最小レベルにおける各軸位置を算出
	int LT = GetPointElem(In_LeftTopFront);
	int RB = GetPointElem(In_RightBottomBack);

	// 空間番号を引き算して最上位区切りから所属レベルを算出
	int Def = RB ^ LT;
	unsigned int HiLevel = 0;
	unsigned int i;
	for (i = 0; static_cast<int>(i) < m_OctreeLevel; ++i)
	{
		int Check = (Def >> (i * 3)) & 0x7;
		if (Check != 0)
			HiLevel = i + 1;
	}
	int SpaceNum = RB >> (HiLevel * 3);

	// 修正: 添字が負や範囲外にならないようにチェック
	int powIndex = m_OctreeLevel - static_cast<int>(HiLevel);
	if (powIndex < 0 || powIndex > cx_MaxLevel)
		return 0xffffffff;

	// オーバーフロー防止のため64bitで計算
	int64_t numerator = static_cast<int64_t>(m_Pow[powIndex]) - 1;
	int64_t denominator = static_cast<int64_t>(cx_DivisionNumber) - 1;
	if (denominator == 0) return 0xffffffff; // 0除算防止

	int AddNum = static_cast<int>(numerator / denominator);
	SpaceNum += AddNum;

	if (SpaceNum > m_MaxCellNum)
		return 0xffffffff;

	return SpaceNum;
}

int CollisionManager::Get3DMortonOrder(_In_ BYTE In_X, _In_ BYTE In_Y, _In_ BYTE In_Z) const noexcept
{
	return DX11Math::BitSeparateFor3D(In_X) | DX11Math::BitSeparateFor3D(In_Y) << 1 | DX11Math::BitSeparateFor3D(In_Z) << 2;
}

int CollisionManager::GetPointElem(_In_ const DirectX::XMFLOAT3 In_Point) const noexcept
{
	BYTE x = static_cast<BYTE>((In_Point.x - m_LeftTopFront.x) / m_Unit.x);
	BYTE y = static_cast<BYTE>((In_Point.y - m_RightBottomBack.y) / m_Unit.y);
	BYTE z = static_cast<BYTE>((In_Point.z - m_LeftTopFront.z) / m_Unit.z);
	return Get3DMortonOrder(x, y, z);
}

int CollisionManager::GetMortonNumberOfCollider(_In_ ColliderBase *In_Collider) noexcept
{
	// オブジェクトの境界範囲から登録モートン番号を算出
	DirectX::XMFLOAT3 LeftTopFront;
	DirectX::XMFLOAT3 RightBottomBack;
	In_Collider->GetAABB(LeftTopFront, RightBottomBack);
	return GetMortonNumber(LeftTopFront, RightBottomBack);
}

bool CollisionManager::RegisterObjectToOctree(_In_ ColliderBase *In_Collider) noexcept
{
	// オブジェクトの境界範囲から登録モートン番号を算出
	int Elem = GetMortonNumberOfCollider(In_Collider);
	if (Elem < m_MaxCellNum)
	{
		// 空間が無い場合は新規作成
		if(!m_OctreeCells.size() || Elem >= static_cast<int>(m_OctreeCells.size()))
		{
			m_OctreeCells.resize(Elem + 1);
			CreateNewCell(Elem);
		}
		else if(!m_OctreeCells[Elem])
			CreateNewCell(Elem);

		return m_OctreeCells[Elem]->Push(In_Collider->GetTreeData(),Elem);
	}
	return false;
}

int CollisionManager::GetAllCollisionList(_In_ ColliderPairMap &In_ColPairs)
{
	// リスト（配列）は必ず初期化します
	In_ColPairs.clear();

	// ルート空間の存在をチェック
	if(m_OctreeCells[0] == nullptr)
		return 0;	// 空間が存在していない

	// ルート空間を処理
	std::list<ColliderBase *> ColStac;
	GetCollisionList(0, In_ColPairs, ColStac);

	return static_cast<int>(In_ColPairs.size());
}

bool CollisionManager::GetCollisionList(_In_ int In_Elem, _Inout_ ColliderPairMap &Inout_ColPairs, _Inout_ std::list<ColliderBase *> &Inout_ColStac)
{
	std::list<ColliderBase *>::iterator itr;
	// ① 空間内のオブジェクト同士の衝突リスト作成
	TreeData* Tree1 = m_OctreeCells[In_Elem]->GetFirstObj();
	while(Tree1 != nullptr)
	{
		TreeData* Tree2 = Tree1->GetNextTree();
		while(Tree2 != nullptr)
		{
			// 衝突リスト作成
			// unordered_setを使って重複登録を防止
			ColliderPairKey Pair(Tree1->GetCollider(), Tree2->GetCollider());
			Inout_ColPairs.emplace(Pair,false);
			Tree2 = Tree2->GetNextTree();
		}
		// ② 衝突スタックとの衝突リスト作成
		for(itr = Inout_ColStac.begin(); itr != Inout_ColStac.end(); ++itr)
		{
			ColliderPairKey Pair(Tree1->GetCollider(), *itr);
			Inout_ColPairs.emplace(Pair, false);
		}
		Tree1 = Tree1->GetNextTree();
	}

	bool ChildFlag = false;
	// ③ 子空間に移動
	int ObjNum = 0;
	int i, NextElem;
	for(i = 0; i < 8; i++)
	{
		NextElem = In_Elem * 8 + 1 + i;

		if(m_OctreeCells.size() <= static_cast<size_t>(NextElem))
			continue;

		if(NextElem < m_MaxCellNum && m_OctreeCells[In_Elem * 8 + 1 + i])
		{
			if(!ChildFlag)
			{
				// ④ 登録オブジェクトをスタックに追加
				Tree1 = m_OctreeCells[In_Elem]->GetFirstObj();
				while(Tree1)
				{
					Inout_ColStac.push_back(Tree1->GetCollider());
					ObjNum++;
					Tree1 = Tree1->GetNextTree();
				}
			}
			ChildFlag = true;
			GetCollisionList(In_Elem * 8 + 1 + i, Inout_ColPairs, Inout_ColStac);	// 子空間へ
		}
	}

	// ⑤ スタックからオブジェクトを外す
	if(ChildFlag)
	{
		for(i = 0; i < ObjNum; ++i)
			Inout_ColStac.pop_back();
	}

	return true;
}

void CollisionManager::RemoveColliderPair(_In_ ColliderBase *In_Collider)
{
	// 指定されたコライダーに関連するペアをすべて削除
	for(auto itr = m_ColliderPairList.begin(); itr != m_ColliderPairList.end(); )
	{
		if(itr->first.pColliderA == In_Collider || itr->first.pColliderB == In_Collider)
			itr = m_ColliderPairList.erase(itr);
		else
			++itr;
	}
}
