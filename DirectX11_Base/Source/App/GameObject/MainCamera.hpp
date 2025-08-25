/*+===================================================================
	File: MainCamera.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 11:28:47 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Camera.hpp"
#include "System/Object/GameObject.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// MainCameraクラス
/// </summary>
class MainCamera : public GameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MainCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MainCamera() = default;

	// ------------------------------
	//  Getter
	// ------------------------------


	// ------------------------------
	//  Setter
	// ------------------------------

private:
	Camera *m_pComponent;
};
