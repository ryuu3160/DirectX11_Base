/*+===================================================================
	File: CameraBaseObj.cpp
	Summary: カメラオブジェクトのベース
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/8 Sat PM 11:36:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraBaseObj.hpp"

CameraBaseObj::CameraBaseObj()
	: CameraBaseObj("CameraBaseObj")
{
}

CameraBaseObj::CameraBaseObj(_In_ std::string In_Name)
	: GameObject(In_Name)
	, m_pComponent(nullptr)
{
}

CameraBaseObj::~CameraBaseObj()
{
}

void CameraBaseObj::Awake() noexcept
{
	m_pComponent = AddComponent<Camera>();
}
