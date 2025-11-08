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
#include "CameraBaseObj.hpp"

/// <summary>
/// CameraDCCクラス
/// </summary>
class CameraDCC : public CameraBaseObj
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
	void Update() noexcept final;

private:
	void UpdateState() noexcept;
	void UpdateOrbit(Argument &In_arg) noexcept;
	void UpdateTrack(Argument &In_arg) noexcept;
	void UpdateDolly(Argument &In_arg) noexcept;
	void UpdateFlight(Argument &In_arg) noexcept;

private:
	int m_nState;
	POINT m_OldPos;
#ifdef _DEBUG
	char m_cMode[256];
#endif
};
