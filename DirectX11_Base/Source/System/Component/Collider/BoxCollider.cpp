/*+===================================================================
	File: BoxCollider.cpp
	Summary: BOXコライダークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/21 Sun AM 04:08:45 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "BoxCollider.hpp"
#include "SphereCollider.hpp"

BoxCollider::BoxCollider()
	: ColliderBase("BoxCollider")
	, m_LeftTopFront(DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f))
	, m_RightBottomBack(DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f))
{
	m_Type = COLLIDER_BOX;
}

BoxCollider::~BoxCollider()
{
}

bool BoxCollider::CheckCollision(_In_ ColliderBase *In_Other) noexcept
{
	// 相手がいなければ処理しない
	if(!In_Other)
		return false;
	// 自分自身とは当たらない
	if(In_Other == this)
		return false;

	switch(In_Other->GetType())
	{
	case COLLIDER_SPHERE:
		// 球対球の当たり判定
		if(IsCollidingBoxToSphere(In_Other))
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
		// 球対箱の当たり判定
		if(IsCollidingBoxToBox(In_Other))
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
	}

	return false;
}

void BoxCollider::DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept
{
	// 当たり判定のアウトラインをAddLineで描画する
	GameObject *obj = m_pTransform;

	auto LeftBottomFront = DirectX::XMFLOAT3(m_LeftTopFront.x, m_RightBottomBack.y, m_LeftTopFront.z);
	auto LeftTopBack = DirectX::XMFLOAT3(m_LeftTopFront.x, m_LeftTopFront.y, m_RightBottomBack.z);
	auto LeftBottomBack = DirectX::XMFLOAT3(m_LeftTopFront.x, m_RightBottomBack.y, m_RightBottomBack.z);
	auto RightTopFront = DirectX::XMFLOAT3(m_RightBottomBack.x, m_LeftTopFront.y, m_LeftTopFront.z);
	auto RightBottomFront = DirectX::XMFLOAT3(m_RightBottomBack.x, m_RightBottomBack.y, m_LeftTopFront.z);
	auto RightTopBack = DirectX::XMFLOAT3(m_RightBottomBack.x, m_LeftTopFront.y, m_RightBottomBack.z);

	In_Gizmos->AddLine(obj, m_LeftTopFront, LeftBottomFront);
	In_Gizmos->AddLine(obj, m_LeftTopFront, LeftTopBack);
	In_Gizmos->AddLine(obj, m_LeftTopFront, RightTopFront);
	In_Gizmos->AddLine(obj, LeftBottomFront, LeftBottomBack);
	In_Gizmos->AddLine(obj, LeftTopBack, LeftBottomBack);
	In_Gizmos->AddLine(obj, RightTopFront, RightTopBack);
	In_Gizmos->AddLine(obj, RightTopFront, RightBottomFront);
	In_Gizmos->AddLine(obj, RightBottomFront, m_RightBottomBack);
	In_Gizmos->AddLine(obj, RightTopBack, m_RightBottomBack);
	In_Gizmos->AddLine(obj, LeftTopBack, RightTopBack);
	In_Gizmos->AddLine(obj, LeftBottomBack, m_RightBottomBack);


}

bool BoxCollider::IsCollidingBoxToSphere(_In_ ColliderBase *In_Other) const noexcept
{
	SphereCollider *other = dynamic_cast<SphereCollider *>(In_Other);
	if(!other) return false;
	DirectX::XMFLOAT3 boxPos = m_pTransform->GetPosition();
	DirectX::XMFLOAT3 spherePos = other->GetGameObject()->GetPosition();
	DirectX::XMFLOAT3 halfExtents = GetHalfExtents();
	// ボックスの各軸に対して、球の中心点をクランプする
	float closestX = std::max(boxPos.x - halfExtents.x, std::min(spherePos.x, boxPos.x + halfExtents.x));
	float closestY = std::max(boxPos.y - halfExtents.y, std::min(spherePos.y, boxPos.y + halfExtents.y));
	float closestZ = std::max(boxPos.z - halfExtents.z, std::min(spherePos.z, boxPos.z + halfExtents.z));
	// クランプされた点と球の中心点との距離を計算
	float distanceX = spherePos.x - closestX;
	float distanceY = spherePos.y - closestY;
	float distanceZ = spherePos.z - closestZ;
	float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ);
	float radius = other->GetRadius();
	// 距離の二乗が半径の二乗以下なら衝突している
	return distanceSquared <= (radius * radius);
}

bool BoxCollider::IsCollidingBoxToBox(_In_ ColliderBase *In_Other) const noexcept
{
	BoxCollider *other = dynamic_cast<BoxCollider *>(In_Other);
	if(!other) return false;

	DirectX::XMFLOAT3 pos1 = m_pTransform->GetPosition();
	DirectX::XMFLOAT3 pos2 = other->m_pTransform->GetPosition();
	DirectX::XMFLOAT3 halfExtents1 = GetHalfExtents();
	DirectX::XMFLOAT3 halfExtents2 = other->GetHalfExtents();
	// 各軸での重なりをチェック
	if (std::abs(pos1.x - pos2.x) <= (halfExtents1.x + halfExtents2.x) &&
		std::abs(pos1.y - pos2.y) <= (halfExtents1.y + halfExtents2.y) &&
		std::abs(pos1.z - pos2.z) <= (halfExtents1.z + halfExtents2.z))
	{
		return true;
	}
	return false;
}
