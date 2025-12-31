/*+===================================================================
	File: Particle.cpp
	Summary: パーティクルクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:20:49 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Particle.hpp"

Particle::Particle()
    : m_Position(0, 0, 0), m_Velocity(0, 0, 0)
    , m_Color(1, 1, 1, 1), m_Size(1, 1)
    , m_Rotation(0, 0, 0)
    , m_LifeTime(0.0f), m_MaxLifeTime(1.0f)
    , m_GravityScale(1.0f), m_IsActive(false)
{
}

void Particle::Update(_In_ float In_DeltaTime, _In_ const DirectX::XMFLOAT3 &In_Gravity)
{
    if(!m_IsActive)
        return;

    m_LifeTime += In_DeltaTime;

    // 寿命チェック
    if(m_LifeTime >= m_MaxLifeTime)
    {
        m_IsActive = false;
        return;
    }

    // 重力適用
    DirectX::XMVECTOR Gravity = DirectX::XMLoadFloat3(&In_Gravity);
    Gravity = DirectX::XMVectorScale(Gravity, m_GravityScale * In_DeltaTime);

    DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&m_Velocity);
    Velocity = DirectX::XMVectorAdd(Velocity, Gravity);
    DirectX::XMStoreFloat3(&m_Velocity, Velocity);

    // 位置更新
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_Position);
    DirectX::XMVECTOR vel = DirectX::XMVectorScale(Velocity, In_DeltaTime);
    pos = DirectX::XMVectorAdd(pos, vel);
    DirectX::XMStoreFloat3(&m_Position, pos);

    // アルファフェード (寿命に応じて透明化)
    float lifeRatio = m_LifeTime / m_MaxLifeTime;
    m_Color.w = 1.0f - lifeRatio;
}

void Particle::Reset()
{
    m_Position = DirectX::XMFLOAT3(0, 0, 0);
    m_Velocity = DirectX::XMFLOAT3(0, 0, 0);
    m_Color = DirectX::XMFLOAT4(1, 1, 1, 1);
    m_Size = DirectX::XMFLOAT2(1, 1);
    m_Rotation = DirectX::XMFLOAT3(0, 0, 0);
    m_LifeTime = 0.0f;
    m_MaxLifeTime = 1.0f;
    m_GravityScale = 1.0f;
    m_IsActive = false;
}

float Particle::GetLifeRatio() const noexcept
{
    return m_MaxLifeTime > 0.0f ? m_LifeTime / m_MaxLifeTime : 0.0f;
}
