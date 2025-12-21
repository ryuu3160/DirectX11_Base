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
#include "BoxCollider.hpp"

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
		// 球対箱の当たり判定
        if (IsCollidingSphereToBox(In_Other))
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

void SphereCollider::DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept
{
	// 当たり判定のアウトラインをDrawLineで描画する
	int segments = 32;              // セグメント数
    float angleStep = 2.0f * PI / segments;  // 1セグメントあたりの角度
	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 中心位置
	int latitudeLines = 4; // 追加の緯度線の数

    // === XY平面の円（Z軸周りの円）===
    auto ColorXY = DirectX::XMFLOAT4(0.7f, 1.0f, 0.0f, 0.7f);
    for(int i = 0; i < segments; i++)
    {
        float angle1 = angleStep * i;
        float angle2 = angleStep * (i + 1);

        DirectX::XMFLOAT3 start(
            center.x + m_Radius * cosf(angle1),
            center.y + m_Radius * sinf(angle1),
            center.z
        );

		DirectX::XMFLOAT3 end(
            center.x + m_Radius * cosf(angle2),
            center.y + m_Radius * sinf(angle2),
            center.z
        );

		In_Gizmos->AddLine(m_pTransform,start, end, ColorXY, ColorXY);
    }

    // === XZ平面の円（Y軸周りの円）===
    auto ColorXZ = DirectX::XMFLOAT4(0.5f, 1.0f, 0.0f, 0.7f);
    for(int i = 0; i < segments; i++)
    {
        float angle1 = angleStep * i;
        float angle2 = angleStep * (i + 1);

		DirectX::XMFLOAT3 start(
            center.x + m_Radius * cosf(angle1),
            center.y,
            center.z + m_Radius * sinf(angle1)
        );

		DirectX::XMFLOAT3 end(
            center.x + m_Radius * cosf(angle2),
            center.y,
            center.z + m_Radius * sinf(angle2)
        );

		In_Gizmos->AddLine(m_pTransform, start, end, ColorXZ, ColorXZ);
    }

    // === YZ平面の円（X軸周りの円）===
    auto ColorYZ = DirectX::XMFLOAT4(0.3f, 1.0f, 0.0f, 0.7f);
    for(int i = 0; i < segments; i++)
    {
        float angle1 = angleStep * i;
        float angle2 = angleStep * (i + 1);

		DirectX::XMFLOAT3 start(
            center.x,
            center.y + m_Radius * cosf(angle1),
            center.z + m_Radius * sinf(angle1)
        );

		DirectX::XMFLOAT3 end(
            center.x,
            center.y + m_Radius * cosf(angle2),
            center.z + m_Radius * sinf(angle2)
        );

		In_Gizmos->AddLine(m_pTransform, start, end, ColorYZ, ColorYZ);
    }
    // === 追加の緯度線（赤道から上下に）===
    auto ColorLat = DirectX::XMFLOAT4(0.5f, 1.0f, 0.0f, 0.4f);
    for(int lat = 1; lat <= latitudeLines; lat++)
    {
        float latAngle = (PI / 2.0f / (latitudeLines + 1)) * lat;
        float latRadius = m_Radius * cosf(latAngle);
        float yOffset = m_Radius * sinf(latAngle);

        // 上半球の緯度線
        for(int i = 0; i < segments; i++)
        {
            float angle1 = angleStep * i;
            float angle2 = angleStep * (i + 1);

            DirectX::XMFLOAT3 start(
                center.x + latRadius * cosf(angle1),
                center.y + yOffset,
                center.z + latRadius * sinf(angle1)
            );

            DirectX::XMFLOAT3 end(
                center.x + latRadius * cosf(angle2),
                center.y + yOffset,
                center.z + latRadius * sinf(angle2)
            );

            In_Gizmos->AddLine(m_pTransform, start, end, ColorLat, ColorLat);
        }

        // 下半球の緯度線
        for(int i = 0; i < segments; i++)
        {
            float angle1 = angleStep * i;
            float angle2 = angleStep * (i + 1);

            DirectX::XMFLOAT3 start(
                center.x + latRadius * cosf(angle1),
                center.y - yOffset,
                center.z + latRadius * sinf(angle1)
            );

            DirectX::XMFLOAT3 end(
                center.x + latRadius * cosf(angle2),
                center.y - yOffset,
                center.z + latRadius * sinf(angle2)
            );

            In_Gizmos->AddLine(m_pTransform, start, end, ColorLat, ColorLat);
        }
    }
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

bool SphereCollider::IsCollidingSphereToBox(_In_ ColliderBase *In_Other) const noexcept
{
    BoxCollider *box = dynamic_cast<BoxCollider *>(In_Other);
    if (!box)
        return false;

	DirectX::XMFLOAT3 spherePos = m_pTransform->GetPosition();
    DirectX::XMVECTOR spherePosVec = DirectX::XMLoadFloat3(&spherePos);
    DirectX::XMFLOAT3 center = box->GetWorldCenter();
    DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&center);
	DirectX::XMFLOAT3 HalfExtents = box->GetHalfExtents(); 
	DirectX::XMFLOAT3 AxisX = box->GetAxisX();
	DirectX::XMFLOAT3 AxisY = box->GetAxisY();
	DirectX::XMFLOAT3 AxisZ = box->GetAxisZ();

    // 球の中心をBoxのローカル座標系に変換
    DirectX::XMVECTOR diff = spherePosVec - centerVec;

    float projX = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&AxisX)));
    float projY = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&AxisY)));
    float projZ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&AxisZ)));

    // クランプして最近接点を求める
    float closestX = std::max(-HalfExtents.x, std::min(projX, HalfExtents.x));
    float closestY = std::max(-HalfExtents.y, std::min(projY, HalfExtents.y));
    float closestZ = std::max(-HalfExtents.z, std::min(projZ, HalfExtents.z));

    // 距離の二乗を計算
    float distSq = (projX - closestX) * (projX - closestX)
        + (projY - closestY) * (projY - closestY)
        + (projZ - closestZ) * (projZ - closestZ);

    float radius = m_Radius;
    return distSq <= (radius * radius);
}
