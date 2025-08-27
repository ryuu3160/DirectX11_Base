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
{

}

MainCamera::~MainCamera()
{
}

void MainCamera::Update()
{
	if (m_pPlayer)
	{
		// 3人称視点カメラ
		UpdateThirdPerson();
	}
}

void MainCamera::SetTargetPlayer(_In_ Player *In_pPlayer) noexcept
{
	m_pPlayer = In_pPlayer;
}

void MainCamera::UpdateThirdPerson() noexcept
{
	DirectX::XMFLOAT3 PlayerPosition = m_pPlayer->GetPos();
	DirectX::XMFLOAT3 PlayerForward = m_pPlayer->GetFront();

	// サードパーソンビュー
	m_pComponent->SetFocus(cx_ThirdPerson_Distance);
	/*m_Target = PlayerPosition;
	m_Target.x += CAMERA_TARGET_OFFSET.x;
	m_Target.y += CAMERA_TARGET_OFFSET.y;
	m_Target.z += CAMERA_TARGET_OFFSET.z;*/

	m_Pos.x = PlayerPosition.x - PlayerForward.x * cx_ThirdPerson_Distance + cx_ThirdPerson_Offset.x;
	m_Pos.y = PlayerPosition.y - PlayerForward.y * cx_ThirdPerson_Distance + cx_ThirdPerson_Offset.y;
	m_Pos.z = PlayerPosition.z - PlayerForward.z * cx_ThirdPerson_Distance + cx_ThirdPerson_Offset.z;
}
