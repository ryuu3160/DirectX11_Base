/*+===================================================================
	File: CameraDCC.hpp
	Summary: カメラオブジェクト 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/03 Thu PM 03:07:03 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Camera.hpp"
#include "GameObject.hpp"

// ==============================
//  前方宣言
// ==============================
class Player;

/// <summary>
/// CameraDCCクラス
/// </summary>
class CameraDCC : public GameObject
{
private:
	struct Argument
	{
		DirectX::XMFLOAT2 mouseMove;
		DirectX::XMVECTOR vCamFront;
		DirectX::XMVECTOR vCamSide;
		DirectX::XMVECTOR vCamUp;
		DirectX::XMVECTOR vCamPos;
		DirectX::XMVECTOR vCamLook;
		float speed;
	};
public:
	CameraDCC();
	~CameraDCC();

	/// <summary>
	/// オブジェクトの状態を更新します。
	/// </summary>
	void Update() final;

	void LateUpdate() override final;

	void SetTargetPlayer(_In_ Player *In_pPlayer) noexcept;

private:
	void UpdateState() noexcept;
	void UpdateOrbit(Argument &In_arg) noexcept;
	void UpdateTrack(Argument &In_arg) noexcept;
	void UpdateDolly(Argument &In_arg) noexcept;
	void UpdateFlight(Argument &In_arg) noexcept;

	// 3人称視点カメラの更新
	void UpdateThirdPerson() noexcept;
	// 1人称視点カメラの更新
	void UpdateFirstPerson() noexcept;

private:
	int m_nState;
	POINT m_OldPos;
	Camera *m_pComponent;
	Player *m_pPlayer;
	bool m_bIsFreeCamera;
#ifdef _DEBUG
	char m_cMode[256];
#endif
};
	