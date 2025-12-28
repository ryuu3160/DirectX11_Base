/*+===================================================================
	File: PlayerController.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/26 Fri PM 08:47:33 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Component.hpp"
#include "System/Component/MovementComponent.hpp"

/// <summary>
/// PlayerControllerクラス
/// </summary>
class PlayerController : public Component
{
public:
    PlayerController();
    ~PlayerController() override;

    void Init() noexcept override;
    void Awake() noexcept override;
    void Update(_In_ float In_DeltaTime) noexcept override;

    /// <summary>
    /// <para>カメラオブジェクトを設定します</para>
    /// <para>設定しなかった場合は、進行方向がオブジェクトの正面方向になります</para>
    /// </summary>
    /// <param name="[In_CameraObj]">設定するカメラオブジェクトへのポインタ</param>
    void SetCameraObj(_In_ GameObject *In_CameraObj);

private:
    // カメラの前方向・右方向を取得
    DirectX::XMFLOAT3 GetCameraForward();
    DirectX::XMFLOAT3 GetCameraRight();
private:
    MovementComponent *m_MovementComponent;
    GameObject *m_CameraObj;
};
