/*+===================================================================
	File: MainCamera.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 11:28:53 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MainCamera.hpp"
#include "Game/Object/Character/PlayerObj.hpp"

MainCamera::MainCamera()
	: Component("MainCamera")
	, m_pComponent(nullptr)
	, m_pPlayer(nullptr)
	, m_CameraMode(0)
{
	m_pComponent->SetIsLockZ(false);
}

MainCamera::~MainCamera()
{
}

void MainCamera::Init() noexcept
{
	m_pComponent = m_pGameObject->GetComponent<Camera>();
}

void MainCamera::Update(_In_ float In_DeltaTime) noexcept
{
	if (Input::IsKeyTrigger('z'))
	{
		m_CameraMode = (m_CameraMode + 1) % 2;
	}
}

void MainCamera::LateUpdate(_In_ float In_DeltaTime) noexcept
{
	if (m_pPlayer)
	{
		switch (m_CameraMode)
		{
		case 0:
			UpdateThirdPerson();
			break;
		case 1:
			UpdateFirstPerson();
			break;
		}
	}
}

void MainCamera::SetTargetPlayer(_In_opt_ PlayerObj *In_pPlayer) noexcept
{
	m_pPlayer = In_pPlayer;
}

void MainCamera::UpdateThirdPerson() noexcept
{
	DirectX::XMFLOAT3 PlayerPosition = m_pPlayer->GetPosition();		// プレイヤーオブジェクトの位置を取得
	DirectX::XMFLOAT3 PlayerForward = m_pPlayer->GetFront();	// プレイヤーオブジェクトの前方向ベクトルを取得
	DirectX::XMFLOAT3 PlayerUp = m_pPlayer->GetUp();			// プレイヤーオブジェクトの上方向ベクトルを取得

	m_pGameObject->SetQuat(m_pPlayer->GetQuat()); // プレイヤーの回転と同期

	// 焦点距離を設定
	m_pComponent->SetFocus(10.0f);

	// カメラの位置調整
	m_pGameObject->SetPosition((PlayerPosition - PlayerForward * 10.0f) + PlayerUp * 1.0f);
}

void MainCamera::UpdateFirstPerson() noexcept
{
	DirectX::XMFLOAT3 PlayerPosition = m_pPlayer->GetPosition();		// プレイヤーオブジェクトの位置を取得
	DirectX::XMFLOAT3 PlayerForward = m_pPlayer->GetFront();	// プレイヤーオブジェクトの前方向ベクトルを取得
	DirectX::XMFLOAT3 PlayerUp = m_pPlayer->GetUp();			// プレイヤーオブジェクトの上方向ベクトルを取得
	m_pGameObject->SetQuat(m_pPlayer->GetQuat()); // プレイヤーの回転と同期

	// 焦点距離を設定
	m_pComponent->SetFocus(10.0f);

	// カメラの位置調整
	m_pGameObject->SetPosition((PlayerPosition + PlayerForward * 10.0f) + PlayerUp * 1.0f);
}
