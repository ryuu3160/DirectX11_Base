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

// ==============================
//	定数定義
// ==============================
namespace
{
}

CollisionManager::CollisionManager()
	: m_OctreeLevel(0), m_MaxCellNum(0)
	, m_Width(0.0f), m_Height(0.0f), m_Depth(0.0f)
	, m_UnitWidth(0.0f), m_UnitHeight(0.0), m_UnitDepth(0.0f), m_Pow{}
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::InitOctreeSpace(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack, _In_ const int In_Level) noexcept
{
	if (In_Level < 0 || In_Level > cx_MaxLevel)
	{
		DebugManager::GetInstance().DebugLogError("CollisionManager::InitOctreeSpace(): 指定されたレベルが不正です。Level=%d", In_Level);
		return;
	}

	// 各レベルでの空間数を算出
	// ルートは1、その子は4、さらにその子（孫）は16、と4^nで増えていく
	// ※ Octreeの場合は底が8となり、1, 8, 64, 512・・となる。
	m_Pow[0] = 1;
	for (int i = 1; i <= cx_MaxLevel + 1; i++)
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
	m_OctreeCells.reserve(m_MaxCellNum);

	// 有効領域を登録
	// 左上手前の座標と幅、高さ、深度を保持
	m_OffsetLeft = In_LeftTopFront.x;
	m_OffsetBottom = In_RightBottomBack.y;
	m_OffsetFront = In_LeftTopFront.z;
	m_Width = In_RightBottomBack.x - In_LeftTopFront.x;
	m_Height = In_LeftTopFront.y - In_RightBottomBack.y;
	m_Depth = In_RightBottomBack.z - In_LeftTopFront.z;

	// 分割数に応じた単位幅と単位高を求める
	// 分割数はlevelを指数とした2の累乗分増えてくため、
	// シフトで求めた数で割ることで単位を求める
	// e.g.)
	// 0レベルなら分割は1、1レベルなら分割は2（2^1）、2レベルなら4（2^2）
	int unit = 1 << In_Level;
	m_UnitWidth = m_Width / unit;
	m_UnitHeight = m_Height / unit;
	m_UnitDepth = m_Depth / unit;

	m_OctreeLevel = In_Level;

	m_ParentShift = static_cast<int>(std::log2(cx_DivisionNumber));
}

void CollisionManager::AddColliderComponent(_In_ ColliderBase *In_Collider) noexcept
{
	// コライダーリストに追加
	m_ColliderList.push_back(In_Collider);
}

void CollisionManager::RemoveColliderComponent(_In_ ColliderBase *In_Collider)
{
	// コライダーリストから削除
	auto itr = std::find(m_ColliderList.begin(), m_ColliderList.end(), In_Collider);
	if(itr != m_ColliderList.end())
	{
		m_ColliderList.erase(itr);
	}
}

void CollisionManager::CheckAllCollisions() noexcept
{
	for(int i = 0; i < static_cast<int>(m_ColliderList.size()); ++i)
	{
		for(int j = i + 1; j < static_cast<int>(m_ColliderList.size()); ++j)
		{
			// コライダー同士の当たり判定をチェック
			m_ColliderList[i]->CheckCollision(m_ColliderList[j]);
		}
	}

	// チェックが終わったらリストをクリア
	m_ColliderList.clear();
}

CollisionManager::MortonInfo CollisionManager::GetMortonInfo(_In_ const DirectX::XMFLOAT3 In_LeftTopFront, _In_ const DirectX::XMFLOAT3 In_RightBottomBack)
{
	int ltd_x = static_cast<int>(In_LeftTopFront.x / m_UnitWidth);
	int ltd_y = static_cast<int>(In_LeftTopFront.y / m_UnitHeight);
	int ltd_z = static_cast<int>(In_LeftTopFront.z / m_UnitDepth);
	int ltd = Get3DMortonOrder(static_cast<BYTE>(ltd_x), static_cast<BYTE>(ltd_y), static_cast<BYTE>(ltd_z));

	return {};
}

int CollisionManager::Get3DMortonOrder(_In_ BYTE In_X, _In_ BYTE In_Y, _In_ BYTE In_Z) const noexcept
{
	return DX11Math::BitSeparateFor3D(In_X) | DX11Math::BitSeparateFor3D(In_Y) << 1 | DX11Math::BitSeparateFor3D(In_Z) << 2;
}
