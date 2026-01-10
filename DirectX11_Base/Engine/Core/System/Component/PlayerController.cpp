/*+===================================================================
	File: PlayerController.cpp
	Summary: デフォルトのプレイヤーコントローラーコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/26 Fri PM 08:47:40 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "PlayerController.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Input/InputSystem.hpp"
#include "Core/DirectX11/System/DX11_Math.hpp"

PlayerController::PlayerController()
	: Component("PlayerController")
	, m_MovementComponent(nullptr)
    , m_CameraObj(nullptr)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Init() noexcept
{
	// MovementComponent を取得
	m_MovementComponent = GetGameObject()->GetComponent<MovementComponent>();

	if(!m_MovementComponent)
	{
		DebugManager::GetInstance().DebugLogError("PlayerController:  MovementComponent not found!");
	}

    InputSystem::GetInstance().RegisterKeyCallBack(VK_SPACE, InputSystem::KeyState::Trigger, [this]()
    {
        if(m_MovementComponent)
        {
            m_MovementComponent->Jump();
        }
	});
}

void PlayerController::Awake() noexcept
{
    m_CameraObj = m_pGameObject;
}

void PlayerController::Update(_In_ float In_DeltaTime) noexcept
{
    if(!m_MovementComponent)
        return;

    // Input名前空間から直接入力を取得
    float horizontal = 0.0f;
    float vertical = 0.0f;

    if(Input::IsKeyPress('W')) vertical += 1.0f;
    if(Input::IsKeyPress('S')) vertical -= 1.0f;
    if(Input::IsKeyPress('A')) horizontal -= 1.0f;
    if(Input::IsKeyPress('D')) horizontal += 1.0f;

    // カメラの方向を考慮した移動方向を計算
    DirectX::XMFLOAT3 forward = GetCameraForward();
    DirectX::XMFLOAT3 right = GetCameraRight();

    DirectX::XMVECTOR forwardVec = DirectX::XMLoadFloat3(&forward);
    DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&right);

    DirectX::XMVECTOR moveDir =
        DirectX::XMVectorScale(forwardVec, vertical) +
        DirectX::XMVectorScale(rightVec, horizontal);

    DirectX::XMFLOAT3 moveDirFloat;
    DirectX::XMStoreFloat3(&moveDirFloat, moveDir);

    // MovementComponentに入力を渡す
    m_MovementComponent->AddInputVector(moveDirFloat);
}

void PlayerController::SetCameraObj(_In_ GameObject *In_CameraObj)
{
    m_CameraObj = In_CameraObj;
}

DirectX::XMFLOAT3 PlayerController::GetCameraForward()
{
    return m_CameraObj->GetFront();
}

DirectX::XMFLOAT3 PlayerController::GetCameraRight()
{
    return m_CameraObj->GetRight();
}
