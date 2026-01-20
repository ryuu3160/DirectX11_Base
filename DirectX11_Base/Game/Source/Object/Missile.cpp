/*+===================================================================
	File: Missile.cpp
	Summary: ミサイルオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/8 Mon AM 12:44:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Missile.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float cx_MoveSpeedScale = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_MissileRange = 18000.0f; // ミサイルの射程距離(180km)

	const inline constexpr float cx_SpeedDuration = 60.0f; // 速度変化にかかる時間
	const inline constexpr float cx_StartDuration = 15.0f; // 開始時のイージング時間
}

Missile::Missile(_In_ std::string In_Name, _In_ const bool &In_IsAddCollider)
	: GameObject(In_Name), m_Speed(0.0f), m_EaseData{}, m_StartPos{}, m_pTarget(nullptr)
	, m_IsAddCollider(In_IsAddCollider)
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Weapon/AIM-120.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する
	SetScale({ MissileScale,MissileScale,MissileScale });

	//if (m_IsAddCollider)
	//{
	//	// コライダー追加
	//	auto collider = AddComponent<SphereCollider>();
	//	collider->SetRadius(1.0f); // 半径を設定
	//}
}

Missile::~Missile()
{
}

void Missile::Update(_In_ float In_DeltaTime) noexcept
{
	if(IsAutoDestroy())
	{
		return;
	}

	// コライダーが有効な場合、衝突判定を行う
	if (m_IsAddCollider)
	{
		// 衝突していたら自分を消す
		auto pos1 = GetPos();
		auto pos2 = m_pTarget ? m_pTarget->GetPos() : DirectX::XMFLOAT3{};
		float radiusSum = 1.0f + 2.0f;
		float distSq = (pos1.x - pos2.x) * (pos1.x - pos2.x) +
			(pos1.y - pos2.y) * (pos1.y - pos2.y) +
			(pos1.z - pos2.z) * (pos1.z - pos2.z);

		// 半径の和の二乗と距離の二乗を比較
		if (distSq <= radiusSum * radiusSum)
		{
			SoundManager::GetInstance().Play("Explosion");
			DestroySelf();

			dynamic_cast<Enemy *>(m_pTarget)->DestroySelf();

			return;
		}

		/*if(GetComponent<SphereCollider>()->IsCollision())
		{
			SoundManager::GetInstance().Play("Explosion");
			DestroySelf();
			return;
		}*/
	}

	float speed;
	if (m_EaseData.fNowTime < m_EaseData.fDuration)
	{
		speed = Ease::OutExpo(m_EaseData);
		m_EaseData.fNowTime += 1.0f;
	}
	else
	{
		speed = m_Speed;
	}

	// ターゲットが設定されている場合、ターゲットへ向かって移動
	if (m_pTarget)
	{
		DirectX::XMFLOAT3 targetPos = m_pTarget->GetPos();
		DirectX::XMVECTOR toTarget = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPos), DirectX::XMLoadFloat3(&m_Pos));
		toTarget = DirectX::XMVector3Normalize(toTarget);
		// 現在の前方ベクトル
		auto front = GetFront();
		DirectX::XMVECTOR currentFront = DirectX::XMLoadFloat3(&front);
		// 回転軸を計算
		DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(currentFront, toTarget);
		float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(currentFront, toTarget)));
		// 回転量を制限（1フレームあたりの最大回転角度）
		constexpr float maxRotationAngle = DirectX::XMConvertToRadians(4.0f); // 1フレームあたり4度まで回転
		if (angle > maxRotationAngle)
			angle = maxRotationAngle;
		if (DirectX::XMVectorGetX(DirectX::XMVector3Length(rotationAxis)) > 0.0001f)
		{
			rotationAxis = DirectX::XMVector3Normalize(rotationAxis);
			DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(rotationAxis, angle);
			DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&m_Quat);
			currentQuat = DirectX::XMQuaternionMultiply(quat, currentQuat);
			currentQuat = DirectX::XMQuaternionNormalize(currentQuat);
			DirectX::XMStoreFloat4(&m_Quat, currentQuat);
		}
	}


	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = GetFront();
	// 移動
	m_Pos += ((front * cx_MoveSpeedScale) * speed);
	GameObject::Update();
}

void Missile::SetSpeed(_In_ const float &In_Speed) noexcept
{
	m_Speed = In_Speed;
	m_EaseData.fStart = 0.0f;
	m_EaseData.fEnd = In_Speed;
	m_EaseData.fNowTime = cx_StartDuration;
	m_EaseData.fDuration = cx_SpeedDuration; // 目標速度に到達するまでの時間
}

bool Missile::IsAutoDestroy() noexcept
{
	float Length;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&m_StartPos);
	DirectX::XMVECTOR now = DirectX::XMLoadFloat3(&m_Pos);
	DirectX::XMVECTOR sub = DirectX::XMVectorSubtract(now, start);
	Length = DirectX::XMVectorGetX(DirectX::XMVector3Length(sub));

	if (Length >= cx_MissileRange)
	{
		DestroySelf();
		return true;
	}
	return false;
}
