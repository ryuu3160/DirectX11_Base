/*+===================================================================
	File: MovementComponent.cpp
	Summary: MovementComponentクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/26 Fri PM 09:09:19 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MovementComponent.hpp"

MovementComponent::MovementComponent()
	: Component("MovementComponent")
	, m_Transform(nullptr)
	, m_Velocity{ 0.0f, 0.0f, 0.0f }
	, m_InputVector{ 0.0f, 0.0f, 0.0f }
	, m_MaxSpeed(5.0f)
	, m_Acceleration(10.0f)
	, m_Deceleration(8.0f)
{
}

void MovementComponent::Init() noexcept
{
	m_Transform = GetGameObject()->GetTransform();
}

void MovementComponent::Update(_In_ float In_DeltaTime) noexcept
{
	// 蓄積した入力を使って移動
	ProcessMovement(In_DeltaTime);

	// 入力をリセット(次フレーム用)
	m_InputVector = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void MovementComponent::AddInputVector(_In_ const DirectX::XMFLOAT3 &In_Input)
{
	m_InputVector.x += In_Input.x;
	m_InputVector.y += In_Input.y;
	m_InputVector.z += In_Input.z;
}

void MovementComponent::Jump()
{
	// ジャンプ処理
	m_Velocity.y = 5.0f;
}

void MovementComponent::ProcessMovement(_In_ float In_DeltaTime)
{
    // 入力ベクトルを正規化
    DirectX::XMVECTOR input = DirectX::XMLoadFloat3(&m_InputVector);
    float inputLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(input));

    if(inputLength > 0.0f)
    {
        input = DirectX::XMVector3Normalize(input);

        // 加速
        DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&m_Velocity);
        DirectX::XMVECTOR acceleration = DirectX::XMVectorScale(input, m_Acceleration * In_DeltaTime);
        velocity = DirectX::XMVectorAdd(velocity, acceleration);

        // 最大速度制限(水平のみ)
        DirectX::XMFLOAT3 velocityFloat;
        DirectX::XMStoreFloat3(&velocityFloat, velocity);

        float horizontalSpeed = std::sqrt(velocityFloat.x * velocityFloat.x + velocityFloat.z * velocityFloat.z);
        if(horizontalSpeed > m_MaxSpeed)
        {
            float scale = m_MaxSpeed / horizontalSpeed;
            velocityFloat.x *= scale;
            velocityFloat.z *= scale;
        }

        m_Velocity = velocityFloat;
    }
    else
    {
        // 減速
        DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&m_Velocity);
        DirectX::XMVECTOR deceleration = DirectX::XMVectorScale(
            DirectX::XMVector3Normalize(velocity),
            -m_Deceleration * In_DeltaTime
        );

        float currentSpeed = DirectX::XMVectorGetX(DirectX::XMVector3Length(velocity));
        float decelAmount = m_Deceleration * In_DeltaTime;

        if(decelAmount >= currentSpeed)
        {
            m_Velocity = DirectX::XMFLOAT3(0, m_Velocity.y, 0); // Y軸は保持
        }
        else
        {
            velocity = DirectX::XMVectorAdd(velocity, deceleration);
            DirectX::XMStoreFloat3(&m_Velocity, velocity);
        }
    }

    // Transformを更新
    DirectX::XMFLOAT3 pos = m_Transform->GetPosition();
    pos.x += m_Velocity.x * In_DeltaTime;
    pos.y += m_Velocity.y * In_DeltaTime;
    pos.z += m_Velocity.z * In_DeltaTime;
    m_Transform->SetPosition(pos);
}
