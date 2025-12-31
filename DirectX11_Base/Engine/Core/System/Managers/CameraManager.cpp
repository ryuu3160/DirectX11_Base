/*+===================================================================
	File: CameraManager.cpp
	Summary: 作成されたカメラを管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun PM 09:49:19 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraManager.hpp"

void CameraManager::AddCameraObj(_In_ GameObject *In_pCameraObj) noexcept
{
	m_Cameras.push_back(In_pCameraObj);
}

void CameraManager::RemoveCameraObj(_In_ GameObject *In_pCameraObj) noexcept
{
	auto itr = std::find(m_Cameras.begin(), m_Cameras.end(), In_pCameraObj);
	if (itr != m_Cameras.end())
	{
		m_Cameras.erase(itr);
	}
}

GameObject *CameraManager::GetMainCameraObj() const noexcept
{
	return m_Cameras.empty() ? nullptr : m_Cameras[0];
}

GameObject *CameraManager::GetCameraObj(_In_ int In_Index) const noexcept
{
	if(In_Index < 0 || In_Index >= static_cast<int>(m_Cameras.size()))
		return nullptr;
	return m_Cameras[In_Index];
}

GameObject *CameraManager::GetCameraObj(_In_ std::string_view In_Name) const noexcept
{
	for(auto &itr : m_Cameras)
	{
		if(itr->GetName() == In_Name.data())
			return itr;
	}
	return nullptr;
}

CameraManager::CameraManager()
{
	m_Cameras.clear();
}

CameraManager::~CameraManager()
{
	m_Cameras.clear();
}
