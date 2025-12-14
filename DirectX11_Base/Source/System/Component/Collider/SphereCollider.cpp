/*+===================================================================
	File: SphereCollider.cpp
	Summary: スフィアコライダーのコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 07:51:57 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SphereCollider.hpp"

SphereCollider::SphereCollider()
	: ColliderBase("SphereCollider"), m_Radius(1.0f)
{
	m_Type = COLLIDER_SPHERE;
}

SphereCollider::~SphereCollider()
{
}

bool SphereCollider::CheckCollision(_In_ ColliderBase *In_Other) noexcept
{
	// 相手がいなければ処理しない
	if (!In_Other)
		return false;
	// 自分自身とは当たらない
	if (In_Other == this)
		return false;

	switch(In_Other->GetType())
	{
	case COLLIDER_SPHERE:
		// 球対球の当たり判定
		if (IsCollidingSphereToSphere(In_Other))
		{
			m_IsCollision = true;
			In_Other->SetIsCollision(true);
			return true;
		}
		else
		{
			m_IsCollision = false;
			In_Other->SetIsCollision(false);
			return false;
		}
		break;

	case COLLIDER_BOX:
		// box対boxの当たり判定
		// 未実装
		break;
	}

	return false;
}

bool SphereCollider::IsCollidingSphereToSphere(_In_ ColliderBase *In_Other) const noexcept
{
	SphereCollider *other = dynamic_cast<SphereCollider *>(In_Other);
	if (!other) return false;

	DirectX::XMFLOAT3 pos1 = m_pTransform->GetPosition();
	DirectX::XMFLOAT3 pos2 = other->m_pTransform->GetPosition();
	float radiusSum = m_Radius + other->m_Radius;
	float distSq = (pos1.x - pos2.x) * (pos1.x - pos2.x) +
		(pos1.y - pos2.y) * (pos1.y - pos2.y) +
		(pos1.z - pos2.z) * (pos1.z - pos2.z);

	// 半径の和の二乗と距離の二乗を比較
	if (distSq <= radiusSum * radiusSum)
	{
		return true;
	}

	return false;
}
