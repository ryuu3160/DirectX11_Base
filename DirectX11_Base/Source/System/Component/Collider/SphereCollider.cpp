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
{
	m_Type = COLLIDER_SPHERE;
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<bool>(&m_IsTrigger);
	In_Data->Access<bool>(&m_IsCollision);
	// 半径の読み書き
	In_Data->Access<float>(&m_Radius);
}

void SphereCollider::CheckCollision(_In_opt_ ColliderBase *In_Other) noexcept
{
	// 相手がいなければ処理しない
	if (!In_Other)
		return;
	// 自分自身とは当たらない
	if (In_Other == this)
		return;

	switch(In_Other->GetType())
	{
	case COLLIDER_SPHERE:
		// 球対球の当たり判定
		if (IsCollidingSphereToSphere(In_Other))
		{
			m_IsCollision = true;
			In_Other->SetIsCollision(true);
		}
		break;

	case COLLIDER_BOX:
		// box対boxの当たり判定
		// 未実装
		break;
	}
}

bool SphereCollider::IsCollidingSphereToSphere(_In_ ColliderBase *In_Other) const noexcept
{
	SphereCollider *other = dynamic_cast<SphereCollider *>(In_Other);
	if (!other) return false;

	DirectX::XMFLOAT3 pos1 = m_pTransform->GetPos();
	DirectX::XMFLOAT3 pos2 = other->m_pTransform->GetPos();
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
