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
//  前方宣言
// ==============================
class Player;

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
	MainCamera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MainCamera();

	void Update() override final;

	void SetTargetPlayer(_In_ Player *In_pPlayer) noexcept;

private:
	// 3人称視点カメラの更新
	void UpdateThirdPerson() noexcept;

private:
	Player *m_pPlayer;
	Camera *m_pComponent;
};
