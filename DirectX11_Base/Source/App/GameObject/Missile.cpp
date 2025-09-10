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
#include "DirectX11/System/ModelRenderer.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float cx_MoveSpeedScale = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_MissileRange = 180000.0f; // ミサイルの射程距離(180km)

	const inline constexpr float cx_SpeedDuration = 60.0f; // 速度変化にかかる時間
	const inline constexpr float cx_StartDuration = 15.0f; // 開始時のイージング時間
}

Missile::Missile()
	: GameObject("Missile"), m_Speed(0.0f), m_EaseData{}, m_StartPos{}
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Weapon/AIM-120.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する
	SetScale({ MissileScale,MissileScale,MissileScale });
}

Missile::~Missile()
{
}

void Missile::Update()
{
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
