/*+===================================================================
	File: CameraObj.cpp
	Summary: カメラオブジェクトのベース
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/8 Sat PM 11:36:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraObj.hpp"
#include "Core/System/Managers/CameraManager.hpp"

CameraObj::CameraObj()
	: CameraObj("CameraObj")
{
}

CameraObj::CameraObj(_In_ std::string In_Name)
	: GameObject(In_Name)
	, m_pComponent(nullptr)
{
	CameraManager::GetInstance().AddCameraObj(this);
}

CameraObj::~CameraObj()
{
	CameraManager::GetInstance().RemoveCameraObj(this);
}

void CameraObj::Awake() noexcept
{
	m_pComponent = AddComponent<Camera>();
}
