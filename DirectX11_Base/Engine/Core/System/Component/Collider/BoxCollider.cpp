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
#include "CapsuleCollider.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/DirectX11/System/DX11_Math.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

BoxCollider::BoxCollider()
	: ColliderBase("BoxCollider")
	, m_HalfExtents(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f))
	, m_WorldCenter(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_AxisX(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)), m_AxisY(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)), m_AxisZ(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f))
	, m_Rotation{ 0.0f, 0.0f, 0.0f }
{
	m_Type = COLLIDER_BOX;
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::SaveLoad(_In_ DataAccessor *In_Data)
{
	ColliderBase::SaveLoad(In_Data);
	In_Data->AccessValue<DirectX::XMFLOAT3>("HalfExtents", &m_HalfExtents);
}

void BoxCollider::Init() noexcept
{
	UpdateWorldSegment();
	ColliderBase::Init();
}

void BoxCollider::Update(_In_ float In_DeltaTime) noexcept
{
	UpdateWorldSegment();
}

void BoxCollider::GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept
{
	// OBB の8頂点を取得
	DirectX::XMFLOAT3 vertices[8];
	GetLocalVertices(vertices);

	// ワールド座標に変換
	auto ObjPos = m_pGameObject->GetPosition();
	for(int i = 0; i < 8; ++i)
	{
		vertices[i] += ObjPos;
	}

	// 8頂点を包む AABB を計算
	DirectX::XMFLOAT3 Min, Max;
	Min = vertices[0];
	Max = vertices[0];

	for(int i = 1; i < 8; ++i)
	{
		Min.x = std::min(Min.x, vertices[i].x);
		Min.y = std::min(Min.y, vertices[i].y);
		Min.z = std::min(Min.z, vertices[i].z);

		Max.x = std::max(Max.x, vertices[i].x);
		Max.y = std::max(Max.y, vertices[i].y);
		Max.z = std::max(Max.z, vertices[i].z);
	}

	Out_LeftTopFront.x = Min.x;
	Out_LeftTopFront.y = Max.y;
	Out_LeftTopFront.z = Min.z;
	Out_RightBottomBack.x = Max.x;
	Out_RightBottomBack.y = Min.y;
	Out_RightBottomBack.z = Max.z;
}

void BoxCollider::DrawColliderOutline(_In_ Gizmos *In_Gizmos) noexcept
{
	// 当たり判定のアウトラインをAddLineで描画する
	GameObject *obj = m_pGameObject;

	// 色
	DirectX::XMFLOAT4 EdgeColorX(0.7f, 1.0f, 0.0f, 0.7f);
	DirectX::XMFLOAT4 EdgeColorY(0.5f, 1.0f, 0.0f, 0.7f);
	DirectX::XMFLOAT4 EdgeColorZ(0.3f, 1.0f, 0.0f, 0.7f);

	// 8つの頂点をワールド座標で取得
	DirectX::XMFLOAT3 v[8];
	GetLocalVertices(v);

	auto AddLineBottom = std::async(std::launch::async, [&]()
		{
			// 底面(4本)
			In_Gizmos->AddLine(obj, v[0], v[1], EdgeColorX, EdgeColorX);
			In_Gizmos->AddLine(obj, v[1], v[2], EdgeColorZ, EdgeColorZ);
			In_Gizmos->AddLine(obj, v[2], v[3], EdgeColorX, EdgeColorX);
			In_Gizmos->AddLine(obj, v[3], v[0], EdgeColorZ, EdgeColorZ);
		});
	auto AddLineTop = std::async(std::launch::async, [&]()
		{
			// 上面(4本)
			In_Gizmos->AddLine(obj, v[4], v[5], EdgeColorX, EdgeColorX);
			In_Gizmos->AddLine(obj, v[5], v[6], EdgeColorZ, EdgeColorZ);
			In_Gizmos->AddLine(obj, v[6], v[7], EdgeColorX, EdgeColorX);
			In_Gizmos->AddLine(obj, v[7], v[4], EdgeColorZ, EdgeColorZ);
		});
	auto AddLineVertical = std::async(std::launch::async, [&]()
		{
			// 縦の辺(4本(）)
			In_Gizmos->AddLine(obj, v[0], v[4], EdgeColorY, EdgeColorY);
			In_Gizmos->AddLine(obj, v[1], v[5], EdgeColorY, EdgeColorY);
			In_Gizmos->AddLine(obj, v[2], v[6], EdgeColorY, EdgeColorY);
			In_Gizmos->AddLine(obj, v[3], v[7], EdgeColorY, EdgeColorY);
		});
	AddLineBottom.get();
	AddLineTop.get();
	AddLineVertical.get();
}

void BoxCollider::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	ColliderBase::RegisterDebugInspector(In_pWindow);
	ItemGroup &Group = In_pWindow->GetGroupItem("BoxCollider");
	Group.CreateGroupItem<ItemBind>("HalfExtents", ItemBind::Kind::Vector, &m_HalfExtents);
	Group.CreateGroupItem<ItemBind>("Rotation##BoxCollider", ItemBind::Kind::Vector, &m_Rotation);
}

void BoxCollider::GetLocalVertices(_Out_ DirectX::XMFLOAT3 outVertices[8]) const noexcept
{
	// ローカル座標での8頂点
	DirectX::XMFLOAT3 localVertices[8] = {
		{-m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z}, // 0: 左下奥
		{ m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z}, // 1: 右下奥
		{ m_HalfExtents.x, -m_HalfExtents.y,  m_HalfExtents.z}, // 2: 右下手前
		{-m_HalfExtents.x, -m_HalfExtents.y,  m_HalfExtents.z}, // 3: 左下手前
		{-m_HalfExtents.x,  m_HalfExtents.y, -m_HalfExtents.z}, // 4: 左上奥
		{ m_HalfExtents.x,  m_HalfExtents.y, -m_HalfExtents.z}, // 5: 右上奥
		{ m_HalfExtents.x,  m_HalfExtents.y,  m_HalfExtents.z}, // 6: 右上手前
		{-m_HalfExtents.x,  m_HalfExtents.y,  m_HalfExtents.z}, // 7: 左上手前
	};

	DirectX::XMVECTOR axisX = DirectX::XMLoadFloat3(&m_AxisX);
	DirectX::XMVECTOR axisY = DirectX::XMLoadFloat3(&m_AxisY);
	DirectX::XMVECTOR axisZ = DirectX::XMLoadFloat3(&m_AxisZ);

	// 各頂点に対して回転を適用
	for(int i = 0; i < 8; ++i)
	{
		DirectX::XMVECTOR local = DirectX::XMLoadFloat3(&localVertices[i]);

		// pos = local.x * axisX + local.y * axisY + local.z * axisZ
		DirectX::XMVECTOR pos = DirectX::XMVectorScale(axisX, localVertices[i].x)
			+ DirectX::XMVectorScale(axisY, localVertices[i].y)
			+ DirectX::XMVectorScale(axisZ, localVertices[i].z);

		DirectX::XMStoreFloat3(&outVertices[i], pos);
	}
}

float BoxCollider::GetProjectedRadius(_In_ const DirectX::XMFLOAT3 &In_HalfExtents, _In_ const DirectX::XMFLOAT3 &In_AxisX,
	_In_ const DirectX::XMFLOAT3 &In_AxisY, _In_ const DirectX::XMFLOAT3 &In_AxisZ, _In_ const DirectX::XMFLOAT3 &In_Axis) const noexcept
{
	return In_HalfExtents.x * std::abs(Dot(In_AxisX, In_Axis))
		+ In_HalfExtents.y * std::abs(Dot(In_AxisY, In_Axis))
		+ In_HalfExtents.z * std::abs(Dot(In_AxisZ, In_Axis));
}

bool BoxCollider::CheckAxis(_In_ const DirectX::XMFLOAT3 &In_Diff, _In_ const DirectX::XMFLOAT3 &In_Axis,
	_In_ float In_RadiusA, _In_ float In_RadiusB) const noexcept
{
	// 軸上の距離を計算
	float distanceOnAxis = std::abs(Dot(In_Diff, In_Axis));
	return distanceOnAxis <= In_RadiusA + In_RadiusB; // 重なっていればtrue
}

float BoxCollider::SegmentToOBBDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_SegA, _In_ const DirectX::XMFLOAT3 &In_SegB) const noexcept
{
	DirectX::XMFLOAT3 segDir = {
		In_SegB.x - In_SegA.x,
		In_SegB.y - In_SegA.y,
		In_SegB.z - In_SegA.z
	};

	float segLengthSq = LengthSquared(segDir);
	if(segLengthSq < EPSILON)
	{
		return PointToOBBDistanceSquared(In_SegA);
	}

	float segLength = std::sqrt(segLengthSq);
	DirectX::XMFLOAT3 segDirNorm = {
		segDir.x / segLength,
		segDir.y / segLength,
		segDir.z / segLength
	};

	// 線分の各端点とOBBの距離
	float distSqA = PointToOBBDistanceSquared(In_SegA);
	float distSqB = PointToOBBDistanceSquared(In_SegB);

	// 最小値を初期値とする
	float minDistSq = std::min(distSqA, distSqB);

	// OBBの各面との交差をチェック
	// 線分上の複数点でサンプリング
	const int samples = 10;
	for(int i = 1; i < samples; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(samples);
		DirectX::XMFLOAT3 samplePoint = {
			In_SegA.x + segDir.x * t,
			In_SegA.y + segDir.y * t,
			In_SegA.z + segDir.z * t
		};

		float distSq = PointToOBBDistanceSquared(samplePoint);
		minDistSq = std::min(minDistSq, distSq);
	}

	return minDistSq;
}

float BoxCollider::PointToOBBDistanceSquared(_In_ const DirectX::XMFLOAT3 &In_Point) const noexcept
{
	// 点をOBBのローカル座標系に変換
	DirectX::XMFLOAT3 diff = {
		In_Point.x - m_WorldCenter.x,
		In_Point.y - m_WorldCenter.y,
		In_Point.z - m_WorldCenter.z
	};

	// 各軸への射影
	float projX = Dot(diff, m_AxisX);
	float projY = Dot(diff, m_AxisY);
	float projZ = Dot(diff, m_AxisZ);

	// OBBの範囲内にクランプ(最近接点を求める)
	float closestX = std::max(-m_HalfExtents.x, std::min(projX, m_HalfExtents.x));
	float closestY = std::max(-m_HalfExtents.y, std::min(projY, m_HalfExtents.y));
	float closestZ = std::max(-m_HalfExtents.z, std::min(projZ, m_HalfExtents.z));

	// 距離の二乗を計算
	float dx = projX - closestX;
	float dy = projY - closestY;
	float dz = projZ - closestZ;

	return dx * dx + dy * dy + dz * dz;
}

void BoxCollider::UpdateWorldSegment() noexcept
{
	// オブジェクトのワールド行列を取得
	DirectX::XMFLOAT4X4 worldMatrix = m_pGameObject->GetWorld(false);
	DirectX::XMMATRIX matWorld = DirectX::XMLoadFloat4x4(&worldMatrix);

	// ローカル中心をワールド座標に変換
	DirectX::XMVECTOR localCenter = DirectX::XMLoadFloat3(&m_Center);
	DirectX::XMVECTOR worldCenter = DirectX::XMVector3TransformCoord(localCenter, matWorld);
	DirectX::XMStoreFloat3(&m_WorldCenter, worldCenter);

	// 各軸方向を取得（回転のみ適用、スケールは無視）
	DirectX::XMVECTOR axisX = DirectX::XMVector3Normalize(matWorld.r[0]); // X軸
	DirectX::XMVECTOR axisY = DirectX::XMVector3Normalize(matWorld.r[1]); // Y軸
	DirectX::XMVECTOR axisZ = DirectX::XMVector3Normalize(matWorld.r[2]); // Z軸

	// RotationをQuatに変換
	auto Rad = ToRad(m_Rotation);
	auto VecQuat = DirectX::XMQuaternionRotationRollPitchYaw(Rad.x, Rad.y, Rad.z);
	// 回転を各軸に適用
	axisX = DirectX::XMVector3Rotate(axisX, VecQuat);
	axisY = DirectX::XMVector3Rotate(axisY, VecQuat);
	axisZ = DirectX::XMVector3Rotate(axisZ, VecQuat);

	DirectX::XMStoreFloat3(&m_AxisX, axisX);
	DirectX::XMStoreFloat3(&m_AxisY, axisY);
	DirectX::XMStoreFloat3(&m_AxisZ, axisZ);
}

bool BoxCollider::IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept
{
	SphereCollider *other = dynamic_cast<SphereCollider *>(In_Other);
	if(!other)
		return false;

	// 計算用に球の中心とBoxの中心、各軸をベクトル化
	DirectX::XMFLOAT3 spherePos = other->GetWorldCenter();
	DirectX::XMVECTOR spherePosVec = DirectX::XMLoadFloat3(&spherePos);
	DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&m_WorldCenter);

	// 球の中心をBoxのローカル座標系に変換
	DirectX::XMVECTOR diff = spherePosVec - centerVec;

	// 各軸への射影を計算
	float projX = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&m_AxisX)));
	float projY = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&m_AxisY)));
	float projZ = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diff, DirectX::XMLoadFloat3(&m_AxisZ)));

	// クランプして最近接点を求める
	float closestX = std::max(-m_HalfExtents.x, std::min(projX, m_HalfExtents.x));
	float closestY = std::max(-m_HalfExtents.y, std::min(projY, m_HalfExtents.y));
	float closestZ = std::max(-m_HalfExtents.z, std::min(projZ, m_HalfExtents.z));

	// 距離の二乗を計算
	float distSq = (projX - closestX) * (projX - closestX)
		+ (projY - closestY) * (projY - closestY)
		+ (projZ - closestZ) * (projZ - closestZ);

	// 半径の二乗と比較
	float radius = other->GetRadius();
	return distSq <= (radius * radius);
}

bool BoxCollider::IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept
{
	BoxCollider *other = dynamic_cast<BoxCollider *>(In_Other);
	if(!other)
		return false;

	// ワールド座標系での中心、軸、半分のサイズを取得
	DirectX::XMFLOAT3 centerA = m_WorldCenter;
	DirectX::XMFLOAT3 axisA[3] = { m_AxisX, m_AxisY, m_AxisZ };
	DirectX::XMFLOAT3 halfA = m_HalfExtents;
	// ぶつかった相手の情報
	DirectX::XMFLOAT3 centerB = other->m_WorldCenter;
	DirectX::XMFLOAT3 axisB[3] = { other->m_AxisX, other->m_AxisY, other->m_AxisZ };
	DirectX::XMFLOAT3 halfB = other->m_HalfExtents;

	// 中心間のベクトル
	DirectX::XMFLOAT3 diff = {
		centerB.x - centerA.x,
		centerB.y - centerA.y,
		centerB.z - centerA.z
	};

	// BoxAの3軸
	float halfAArray[3] = { halfA.x, halfA.y, halfA.z };
	for(int i = 0; i < 3; ++i)
	{
		// 投影半径を計算、軸での分離をチェック
		float radiusB = GetProjectedRadius(halfB, axisB[0], axisB[1], axisB[2], axisA[i]);
		if(!CheckAxis(diff, axisA[i], halfAArray[i], radiusB))
			return false;
	}

	// BoxBの3軸
	float halfBArray[3] = { halfB.x, halfB.y, halfB.z };
	for(int i = 0; i < 3; ++i)
	{
		// 投影半径を計算、軸での分離をチェック
		float radiusA = GetProjectedRadius(halfA, axisA[0], axisA[1], axisA[2], axisB[i]);
		if(!CheckAxis(diff, axisB[i], radiusA, halfBArray[i]))
			return false;
	}

	// 外積軸(9本)
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			// 軸を計算
			DirectX::XMFLOAT3 axis = Cross(axisA[i], axisB[j]);
			float lengthSq = LengthSquared(axis);

			if(lengthSq < EPSILON)
				continue; // 平行な軸はスキップ

			// 正規化
			float invLength = 1.0f / std::sqrt(lengthSq);
			axis.x *= invLength;
			axis.y *= invLength;
			axis.z *= invLength;

			// 投影半径を計算
			float radiusA = GetProjectedRadius(halfA, axisA[0], axisA[1], axisA[2], axis);
			float radiusB = GetProjectedRadius(halfB, axisB[0], axisB[1], axisB[2], axis);

			// 軸での分離をチェック
			if(!CheckAxis(diff, axis, radiusA, radiusB))
				return false;
		}
	}

	return true; // 衝突している
}

bool BoxCollider::IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept
{
	CapsuleCollider *capsule = dynamic_cast<CapsuleCollider *>(In_Other);
	if(!capsule)
		return false;

	// カプセルの線分と半径を取得
	DirectX::XMFLOAT3 capA = capsule->GetWorldPointA();
	DirectX::XMFLOAT3 capB = capsule->GetWorldPointB();
	float capRadius = capsule->GetRadius();

	// 線分とOBBの最短距離の二乗を計算
	float distSq = SegmentToOBBDistanceSquared(capA, capB);

	// 半径の二乗と比較
	return distSq <= (capRadius * capRadius);
}
