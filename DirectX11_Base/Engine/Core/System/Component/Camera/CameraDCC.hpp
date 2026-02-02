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
#include "Core/System/Component/Component.hpp"
#include "Core/System/Component/Camera/Camera.hpp"

// ---ComponentDescription---
// エディター用のカメラコントロールコンポーネント
// --------------------------
class CameraDCC : public Component
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
	CameraDCC(_In_ std::string In_Name);
	~CameraDCC();

	void Init() noexcept override;

	/// <summary>
	/// オブジェクトの状態を更新します。
	/// </summary>
	void Update(_In_ float In_DeltaTime) noexcept final;

private:
	void UpdateState() noexcept;
	void UpdateOrbit(Argument &In_arg) noexcept;
	void UpdateTrack(Argument &In_arg) noexcept;
	void UpdateDolly(Argument &In_arg) noexcept;
	void UpdateFlight(Argument &In_arg) noexcept;

private:
	int m_nState;
	Camera *m_pCamera;
	POINT m_OldPos;
#ifdef _DEBUG
	char m_cMode[256];
#endif
};
