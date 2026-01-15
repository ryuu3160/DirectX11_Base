/*+===================================================================
	File: MovementComponent.hpp
	Summary: MovementComponentクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/26 Fri PM 09:09:13 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Component/Component.hpp"

// ==============================
//	前方宣言
// ==============================
class Transform;

/// <summary>
/// MovementComponentクラス
/// </summary>
class MovementComponent : public Component
{
public:
	MovementComponent();
	~MovementComponent() = default;

	void Init() noexcept override;

	void Update(_In_ float In_DeltaTime) noexcept override;

    // 入力を追加
	void AddInputVector(_In_ const DirectX::XMFLOAT3 &In_Input);

    void Jump();

private:
    void ProcessMovement(_In_ float In_DeltaTime);

private:
	Transform *m_Transform;
	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_InputVector; // フレーム内の入力を蓄積

	float m_MaxSpeed;
	float m_Acceleration;
	float m_Deceleration;
};
