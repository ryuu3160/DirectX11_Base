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
#include "App/GameObject/Character/Player.hpp"

MainCamera::MainCamera()
	: GameObject("MainCamera")
	, m_pComponent(AddComponent<Camera>())
	, m_pPlayer(nullptr)
	, m_CameraMode(0)
{
	m_pComponent->SetIsLockZ(false);
}

MainCamera::~MainCamera()
{
}

void MainCamera::Update()
{
	if (Input::IsKeyTrigger(cx_ChangeCameraKey))
	{
		m_CameraMode = (m_CameraMode + 1) % 2;
	}
}

void MainCamera::LateUpdate()
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

void MainCamera::SetTargetPlayer(_In_opt_ Player *In_pPlayer) noexcept
{
	m_pPlayer = In_pPlayer;
}

void MainCamera::UpdateThirdPerson() noexcept
{
	DirectX::XMFLOAT3 PlayerPosition = m_pPlayer->GetPos();		// プレイヤーオブジェクトの位置を取得
	DirectX::XMFLOAT3 PlayerForward = m_pPlayer->GetFront();	// プレイヤーオブジェクトの前方向ベクトルを取得
	DirectX::XMFLOAT3 PlayerUp = m_pPlayer->GetUp();			// プレイヤーオブジェクトの上方向ベクトルを取得

	m_Quat = m_pPlayer->GetQuat(); // プレイヤーの回転と同期

	// 焦点距離を設定
	m_pComponent->SetFocus(cx_ThirdPerson_Distance);

	// カメラの位置調整
	m_Pos = (PlayerPosition - PlayerForward * cx_ThirdPerson_Distance) + PlayerUp * cx_ThirdPerson_UpDistanceRate;
}

void MainCamera::UpdateFirstPerson() noexcept
{
	DirectX::XMFLOAT3 PlayerPosition = m_pPlayer->GetPos();		// プレイヤーオブジェクトの位置を取得
	DirectX::XMFLOAT3 PlayerForward = m_pPlayer->GetFront();	// プレイヤーオブジェクトの前方向ベクトルを取得
	DirectX::XMFLOAT3 PlayerUp = m_pPlayer->GetUp();			// プレイヤーオブジェクトの上方向ベクトルを取得
	m_Quat = m_pPlayer->GetQuat(); // プレイヤーの回転と同期

	// 焦点距離を設定
	m_pComponent->SetFocus(cx_FirstPerson_Distance);

	// カメラの位置調整
	m_Pos = (PlayerPosition + PlayerForward * cx_FirstPerson_Distance) + PlayerUp * cx_FirstPerson_UpDistance;
}
