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

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
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
