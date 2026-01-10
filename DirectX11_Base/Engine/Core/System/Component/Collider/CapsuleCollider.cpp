/*+===================================================================
	File: CapsuleCollider.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/22 Mon AM 05:12:05 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CapsuleCollider.hpp"
#include "SphereCollider.hpp"
#include "BoxCollider.hpp"
#include "Core/System/Object/GameObject.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

CapsuleCollider::CapsuleCollider()
	: ColliderBase("CapsuleCollider")
	, m_Height(2.0f), m_Radius(0.5f), m_Direction{ 0.0f, 1.0f, 0.0f }
	, m_WorldCenter{ 0.0f, 0.0f, 0.0f }, m_WorldPointA{ 0.0f, 0.0f, 0.0f }, m_WorldPointB{ 0.0f, 0.0f, 0.0f }
	, m_WorldDirection{ 0.0f, 1.0f, 0.0f }
{
	m_Type = COLLIDER_CAPSULE;
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::Init() noexcept
{
	ColliderBase::Init();
    UpdateWorldSegment();
}

void CapsuleCollider::Update(_In_ float In_DeltaTime) noexcept
{
    UpdateWorldSegment();
}

void CapsuleCollider::GetWorldSegment(_Out_ DirectX::XMFLOAT3 &Out_PointA, _Out_ DirectX::XMFLOAT3 &Out_PointB) const noexcept
{
    Out_PointA = m_WorldPointA;
    Out_PointB = m_WorldPointB;
}

void CapsuleCollider::GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept
{
    // 線分の両端 + 半径を考慮
	DirectX::XMFLOAT3 Min, Max;
    Min.x = std::min(m_WorldPointA.x, m_WorldPointB.x) - m_Radius;
    Min.y = std::min(m_WorldPointA.y, m_WorldPointB.y) - m_Radius;
    Min.z = std::min(m_WorldPointA.z, m_WorldPointB.z) - m_Radius;

    Max.x = std::max(m_WorldPointA.x, m_WorldPointB.x) + m_Radius;
    Max.y = std::max(m_WorldPointA.y, m_WorldPointB.y) + m_Radius;
    Max.z = std::max(m_WorldPointA.z, m_WorldPointB.z) + m_Radius;
    Out_LeftTopFront = DirectX::XMFLOAT3(Min.x, Max.y, Min.z);
	Out_RightBottomBack = DirectX::XMFLOAT3(Max.x, Min.y, Max.z);
}

void CapsuleCollider::DrawColliderOutline(_In_ Gizmos *In_Gizmos) noexcept
{
    auto Obj = m_pGameObject;

    const int segments = 16; // 円の分割数
    const int hemisphereRings = 4; // 半球の分割数
    const float angleStep = TWO_PI / segments;
	auto LocalPointA = GetLocalPointA();
	auto LocalPointB = GetLocalPointB();

    // 色
    DirectX::XMFLOAT4 ColorX(0.7f, 1.0f, 0.0f, 0.7f);
    DirectX::XMFLOAT4 ColorY(0.5f, 1.0f, 0.0f, 0.7f);
    DirectX::XMFLOAT4 ColorLat(0.5f, 1.0f, 0.0f, 0.4f);

    // カプセルの軸方向ベクトル
    DirectX::XMVECTOR pointA = DirectX::XMLoadFloat3(&LocalPointA);
    DirectX::XMVECTOR pointB = DirectX::XMLoadFloat3(&LocalPointB);
    DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(pointB - pointA);

    // 軸に垂直な2つのベクトルを求める
    DirectX::XMVECTOR right, up;
    DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0, 1, 0, 0);

    if(std::abs(DirectX::XMVectorGetY(dir)) > 0.99f)
    {
        // ほぼY軸方向の場合はX軸を基準にする
        right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dir, DirectX::XMVectorSet(1, 0, 0, 0)));
    }
    else
    {
        right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dir, worldUp));
    }
    up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dir, right));

    // ===========================
    // 1. 上端と下端の(円水平方向)
    // ===========================
    auto drawCircle = [&](const DirectX::XMVECTOR &center, const DirectX::XMVECTOR &normal)
        {
            for(int i = 0; i < segments; ++i)
            {
                float angle1 = angleStep * i;
                float angle2 = angleStep * ((i + 1) % segments);

                DirectX::XMVECTOR offset1 = right * (m_Radius * cosf(angle1)) + up * (m_Radius * sinf(angle1));
                DirectX::XMVECTOR offset2 = right * (m_Radius * cosf(angle2)) + up * (m_Radius * sinf(angle2));

                DirectX::XMFLOAT3 p1, p2;
                DirectX::XMStoreFloat3(&p1, center + offset1);
                DirectX::XMStoreFloat3(&p2, center + offset2);

                In_Gizmos->AddLine(Obj, p1, p2, ColorY, ColorY);
            }
        };

    drawCircle(pointA, dir);  // 上端の円
    drawCircle(pointB, -dir); // 下端の円

    // ===========================
    // 2. 側面の縦線(4本)
    // ===========================
    for(int i = 0; i < 8; ++i)
    {
        float angle = angleStep * i * (segments / 8);
        DirectX::XMVECTOR offset = right * (m_Radius * cosf(angle)) + up * (m_Radius * sinf(angle));

        DirectX::XMFLOAT3 p1, p2;
        DirectX::XMStoreFloat3(&p1, pointA + offset);
        DirectX::XMStoreFloat3(&p2, pointB + offset);

        In_Gizmos->AddLine(Obj, p1, p2, ColorX, ColorX);
    }

    // ===========================
    // 3. 半球部分の弧(アーチ)
    // ===========================
    auto drawHemisphere = [&](const DirectX::XMVECTOR &center, float direction)
        {
            // direction:  1.0f = 上向き, -1.0f = 下向き

            // 複数方向から見た半円を描画
            for(int axis = 0; axis < 8; ++axis)
            {
                float baseAngle = angleStep * axis * (segments / 8);
                DirectX::XMVECTOR tangent = right * cosf(baseAngle) + up * sinf(baseAngle);

                // 半球の弧を描画
                for(int i = 0; i < hemisphereRings; ++i)
                {
                    float theta1 = (HALF_PI / hemisphereRings) * i;
                    float theta2 = (HALF_PI / hemisphereRings) * (i + 1);

                    float r1 = m_Radius * cosf(theta1);
                    float h1 = m_Radius * sinf(theta1) * direction;

                    float r2 = m_Radius * cosf(theta2);
                    float h2 = m_Radius * sinf(theta2) * direction;

                    DirectX::XMVECTOR p1 = center + dir * h1 + tangent * r1;
                    DirectX::XMVECTOR p2 = center + dir * h2 + tangent * r2;

                    DirectX::XMFLOAT3 point1, point2;
                    DirectX::XMStoreFloat3(&point1, p1);
                    DirectX::XMStoreFloat3(&point2, p2);

                    In_Gizmos->AddLine(Obj, point1, point2, ColorX, ColorX);
                }
            }

            // 追加：中間の緯度線(半球の輪郭を強調)
            for(int ring = 1; ring < hemisphereRings; ++ring)
            {
                float theta = (HALF_PI / hemisphereRings) * ring;
                float r = m_Radius * cosf(theta);
                float h = m_Radius * sinf(theta) * direction;

                for(int i = 0; i < segments; ++i)
                {
                    float angle1 = angleStep * i;
                    float angle2 = angleStep * ((i + 1) % segments);

                    DirectX::XMVECTOR offset1 = right * (r * cosf(angle1)) + up * (r * sinf(angle1));
                    DirectX::XMVECTOR offset2 = right * (r * cosf(angle2)) + up * (r * sinf(angle2));

                    DirectX::XMFLOAT3 p1, p2;
                    DirectX::XMStoreFloat3(&p1, center + dir * h + offset1);
                    DirectX::XMStoreFloat3(&p2, center + dir * h + offset2);

                    In_Gizmos->AddLine(Obj, p1, p2, ColorLat, ColorLat);
                }
            }
        };

    drawHemisphere(pointA, -1.0f);  // 上の半球
    drawHemisphere(pointB, 1.0f); // 下の半球
}

void CapsuleCollider::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
    ColliderBase::RegisterDebugInspector(In_pWindow);
	ItemGroup &Group = In_pWindow->GetGroupItem("CapsuleCollider");
	Group.CreateGroupItem<ItemBind>("Height", ItemBind::Kind::Float, &m_Height);
	Group.CreateGroupItem<ItemBind>("Radius", ItemBind::Kind::Float, &m_Radius);
}

bool CapsuleCollider::IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept
{
    BoxCollider *box = dynamic_cast<BoxCollider *>(In_Other);
    if(!box)
        return false;

    // Box 側の判定を呼び出す(逆方向)
    return box->IsCollisionToCapsule(this);
}

bool CapsuleCollider::IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept
{
    SphereCollider*sphere = dynamic_cast<SphereCollider*>(In_Other);
    if (!sphere)
        return false;

    DirectX::XMFLOAT3 spherePos = sphere->GetGameObject()->GetPosition();
    float sphereRadius = sphere->GetRadius();

    // 球の中心からカプセル線分への最短距離の二乗
    float dummyParam = 0.0f;
    float distSq = PointToSegmentDistanceSquared(spherePos, m_WorldPointA, m_WorldPointB, &dummyParam);

    // 半径の和の二乗と比較
    float radiusSum = m_Radius + sphereRadius;
    return distSq <= (radiusSum * radiusSum);
}

bool CapsuleCollider::IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept
{
    CapsuleCollider *other = dynamic_cast<CapsuleCollider *>(In_Other);
    if(!other)
        return false;

    // 2つの線分の最短距離の二乗を計算
    // C6387警告回避のため、未使用でもダミー変数を渡す
    float dummySeg = 0.0f;
    float dummyParam = 0.0f;
    float distSq = SegmentToSegmentDistanceSquared(
        m_WorldPointA, m_WorldPointB,
        other->m_WorldPointA, other->m_WorldPointB,
        &dummySeg, &dummyParam
    );

    // 半径の和の二乗と比較
    float radiusSum = m_Radius + other->m_Radius;
    return distSq <= (radiusSum * radiusSum);
}

void CapsuleCollider::UpdateWorldSegment() noexcept
{
    // ワールド行列を取得
    DirectX::XMFLOAT4X4 worldMatrix = m_pGameObject->GetWorld(false);
    DirectX::XMMATRIX matWorld = DirectX::XMLoadFloat4x4(&worldMatrix);

    // 中心をワールド座標に変換
    DirectX::XMVECTOR localCenter = DirectX::XMLoadFloat3(&m_Center);
    DirectX::XMVECTOR worldCenter = DirectX::XMVector3TransformCoord(localCenter, matWorld);
    DirectX::XMStoreFloat3(&m_WorldCenter, worldCenter);

    // ローカルの方向をオブジェクトの回転に合わせる
    m_Direction = m_pGameObject->GetUp();

    // 方向ベクトルを回転(スケールは無視)
    DirectX::XMVECTOR localDir = DirectX::XMLoadFloat3(&m_Direction);
    DirectX::XMVECTOR worldDir = DirectX::XMVector3Normalize(
        DirectX::XMVector3TransformNormal(localDir, matWorld)
    );
    DirectX::XMStoreFloat3(&m_WorldDirection, worldDir);

    // 線分の長さを計算(高さから両端の半球を引く)
    float segmentLength = std::max(0.0f, m_Height - m_Radius * 2.0f);
    float halfSegment = segmentLength * 0.5f;

    // 線分の両端を計算
    DirectX::XMVECTOR pointA = worldCenter + worldDir * halfSegment;
    DirectX::XMVECTOR pointB = worldCenter - worldDir * halfSegment;

    DirectX::XMStoreFloat3(&m_WorldPointA, pointA);
    DirectX::XMStoreFloat3(&m_WorldPointB, pointB);
}

float CapsuleCollider::PointToSegmentDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_Point, _In_ const DirectX::XMFLOAT3 &In_SegmentA,
	_In_ const DirectX::XMFLOAT3 &In_SegmentB, _Out_ float *Out_Param) const noexcept
{
    DirectX::XMFLOAT3 ab = In_SegmentB - In_SegmentA;
    DirectX::XMFLOAT3 ap = In_Point - In_SegmentA;

    float abLenSq = LengthSquared(ab);

    if(abLenSq < 1e-8f)
    {
        // 線分が点に縮退
        if(Out_Param)
            *Out_Param = 0.0f;
        return LengthSquared(ap);
    }

    float t = Dot(ap, ab) / abLenSq;
    t = std::max(0.0f, std::min(1.0f, t)); // クランプ [0, 1]

    if(Out_Param)
        *Out_Param = t;

    DirectX::XMFLOAT3 closest = {
        In_SegmentA.x + ab.x * t,
        In_SegmentA.y + ab.y * t,
        In_SegmentA.z + ab.z * t
    };

    DirectX::XMFLOAT3 diff = In_Point - closest;
    return LengthSquared(diff);
}

float CapsuleCollider::SegmentToSegmentDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_A1, _In_ const DirectX::XMFLOAT3 &In_A2,
	_In_ const DirectX::XMFLOAT3 &In_B1, _In_ const DirectX::XMFLOAT3 &In_B2, _Out_ float *Out_Seg, _Out_ float *Out_Param) const noexcept
{
    DirectX::XMFLOAT3 d1 = In_A2 - In_A1;
    DirectX::XMFLOAT3 d2 = In_B2 - In_B1;
    DirectX::XMFLOAT3 r = In_A1 - In_B1;

    float a = LengthSquared(d1);
    float e = LengthSquared(d2);
    float f = Dot(d2, r);

    const float epsilon = 1e-8f;
    float s = 0.0f, t = 0.0f;

    // 両方とも点に縮退
    if(a <= epsilon && e <= epsilon)
    {
        if(Out_Seg)
            *Out_Seg = 0.0f;
        if(Out_Param)
            *Out_Param = 0.0f;
        return LengthSquared(r);
    }

    // 線分1が点
    if(a <= epsilon)
    {
        s = 0.0f;
        t = std::max(0.0f, std::min(1.0f, f / e));
    }
    else
    {
        float c = Dot(d1, r);

        // 線分2が点
        if(e <= epsilon)
        {
            t = 0.0f;
            s = std::max(0.0f, std::min(1.0f, -c / a));
        }
        else
        {
            // 一般的なケース
            float b = Dot(d1, d2);
            float denom = a * e - b * b;

            if(std::abs(denom) > epsilon)
            {
                s = std::max(0.0f, std::min(1.0f, (b * f - c * e) / denom));
            }
            else
            {
                s = 0.0f;
            }

            t = (b * s + f) / e;

            // t をクランプして s を再計算
            if(t < 0.0f)
            {
                t = 0.0f;
                s = std::max(0.0f, std::min(1.0f, -c / a));
            }
            else if(t > 1.0f)
            {
                t = 1.0f;
                s = std::max(0.0f, std::min(1.0f, (b - c) / a));
            }
        }
    }

    if(Out_Seg)
        *Out_Seg = s;
    if(Out_Param)
        *Out_Param = t;

    DirectX::XMFLOAT3 c1 = {
        In_A1.x + d1.x * s,
        In_A1.y + d1.y * s,
        In_A1.z + d1.z * s
    };

    DirectX::XMFLOAT3 c2 = {
        In_B1.x + d2.x * t,
        In_B1.y + d2.y * t,
        In_B1.z + d2.z * t
    };

    DirectX::XMFLOAT3 diff = c1 - c2;
    return LengthSquared(diff);
}

DirectX::XMFLOAT3 CapsuleCollider::GetLocalPointA() const
{
    DirectX::XMFLOAT3 dirNormalized;
    float len = std::sqrt(m_Direction.x * m_Direction.x + m_Direction.y * m_Direction.y + m_Direction.z * m_Direction.z);
    if(len > EPSILON)
    {
        dirNormalized = {
            m_Direction.x / len,
            m_Direction.y / len,
            m_Direction.z / len
        };
    }
    else
    {
        dirNormalized = { 0.0f, 1.0f, 0.0f }; // デフォルトの向き
    }
    float halfSegment = (m_Height - m_Radius * 2.0f) * 0.5f;
    return {
        m_Center.x + dirNormalized.x * halfSegment,
        m_Center.y + dirNormalized.y * halfSegment,
        m_Center.z + dirNormalized.z * halfSegment
	};
}

DirectX::XMFLOAT3 CapsuleCollider::GetLocalPointB() const
{
    DirectX::XMFLOAT3 dirNormalized;
    float len = std::sqrt(m_Direction.x * m_Direction.x + m_Direction.y * m_Direction.y + m_Direction.z * m_Direction.z);
    if(len > EPSILON)
    {
        dirNormalized = {
            m_Direction.x / len,
            m_Direction.y / len,
            m_Direction.z / len
        };
    }
    else
    {
        dirNormalized = { 0.0f, 1.0f, 0.0f }; // デフォルトの向き
    }
    float halfSegment = (m_Height - m_Radius * 2.0f) * 0.5f;
    return {
        m_Center.x - dirNormalized.x * halfSegment,
        m_Center.y - dirNormalized.y * halfSegment,
        m_Center.z - dirNormalized.z * halfSegment
	};
}
