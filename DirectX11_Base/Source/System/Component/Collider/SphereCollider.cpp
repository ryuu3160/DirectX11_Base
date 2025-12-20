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
    if (!box) return false;
    DirectX::XMFLOAT3 sphereCenter = m_pTransform->GetPosition();
    DirectX::XMFLOAT3 boxCenter = box->GetGameObject()->GetPosition();
    DirectX::XMFLOAT3 boxHalfExtents = box->GetHalfExtents();
    // 球の中心から箱の中心へのベクトルを計算
    DirectX::XMFLOAT3 diff;
    diff.x = sphereCenter.x - boxCenter.x;
    diff.y = sphereCenter.y - boxCenter.y;
    diff.z = sphereCenter.z - boxCenter.z;
    // 最近接点を計算
    DirectX::XMFLOAT3 closestPoint;
    closestPoint.x = std::max(-boxHalfExtents.x, std::min(diff.x, boxHalfExtents.x));
    closestPoint.y = std::max(-boxHalfExtents.y, std::min(diff.y, boxHalfExtents.y));
    closestPoint.z = std::max(-boxHalfExtents.z, std::min(diff.z, boxHalfExtents.z));
    // 最近接点と球の中心との距離を計算
    DirectX::XMFLOAT3 closestToSphere;
    closestToSphere.x = diff.x - closestPoint.x;
    closestToSphere.y = diff.y - closestPoint.y;
    closestToSphere.z = diff.z - closestPoint.z;
    float distSq = closestToSphere.x * closestToSphere.x +
        closestToSphere.y * closestToSphere.y +
        closestToSphere.z * closestToSphere.z;
    // 半径の二乗と距離の二乗を比較
    if (distSq <= m_Radius * m_Radius)
    {
        return true;
    }
	return false;
}
