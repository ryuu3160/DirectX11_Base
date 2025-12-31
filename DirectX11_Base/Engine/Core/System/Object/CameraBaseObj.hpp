/*+===================================================================
	File: CameraBaseObj.hpp
	Summary: カメラオブジェクトのベース
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/8 Sat PM 11:36:51 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/System/Component/Camera.hpp"
#include "GameObject.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// CameraBaseObjクラス
/// </summary>
class CameraBaseObj : public GameObject
{
public:
	CameraBaseObj();
	CameraBaseObj(_In_ std::string In_Name);
	~CameraBaseObj();

	virtual void Awake() noexcept override;

protected:
	Camera *m_pComponent;
};
