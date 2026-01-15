/*+===================================================================
	File: CameraDCC.cpp
	Summary: カメラオブジェクト 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/03 Thu PM 03:07:13 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CameraDCC.hpp"
#include "Core/System/Input/Input.hpp"

enum CameraDCCKind
{
	CAM_DCC_NONE,
	CAM_DCC_ORBIT,
	CAM_DCC_TRACK,
	CAM_DCC_DOLLY,
	CAM_DCC_FLIGHT,
};

CameraDCC::CameraDCC()
	: CameraDCC("CameraDCC")
{
}

CameraDCC::CameraDCC(_In_ std::string In_Name)
	: CameraBaseObj(In_Name)
	, m_nState(CAM_DCC_NONE)
	, m_OldPos{ 0, 0 }
{
#ifdef _DEBUG
	sprintf_s(m_cMode, "None");
#endif
}

CameraDCC::~CameraDCC()
{
}

void CameraDCC::Update(_In_ float In_DeltaTime) noexcept
{
	Argument arg{};
#ifdef _DEBUG
//	auto &window = debug::Menu::Get("00_Info");
//	arg.speed = window["Camera"]["Speed"].GetFloat();
//	arg.mouseMove.x = window["Camera"]["FlipMouseX"].GetBool() ? -1.0f : 1.0f;
//	arg.mouseMove.y = window["Camera"]["FlipMouseY"].GetBool() ? -1.0f : 1.0f;
	arg.speed = 1.0f; // デバッグ中は常に1.0f
	arg.mouseMove.x = Input::IsKeyPress(VK_LBUTTON) ? -1.0f : 1.0f;
	arg.mouseMove.y = Input::IsKeyPress(VK_MBUTTON) ? -1.0f : 1.0f;
#endif

	UpdateState();
#ifdef _DEBUG
	const char *list[] = {
		"None", "Orbit", "Track", "Dolly", "Flight"
	};
	sprintf_s(m_cMode, "%s", list[m_nState]);
#endif
	if (m_nState == CAM_DCC_NONE) return;

	// マウス移動量
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	arg.mouseMove.x *= cursorPos.x - m_OldPos.x;
	arg.mouseMove.y *= cursorPos.y - m_OldPos.y;

	m_OldPos = cursorPos;
	// カメラ情報
	DirectX::XMFLOAT3 front = m_pTransform->GetFront();
	DirectX::XMFLOAT3 side = m_pTransform->GetRight();
	DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 look = m_pComponent->GetLook();
	DirectX::XMFLOAT3 Pos = m_pTransform->GetPosition();
	arg.vCamFront = DirectX::XMLoadFloat3(&front);
	arg.vCamSide = DirectX::XMLoadFloat3(&side);
	arg.vCamPos = DirectX::XMLoadFloat3(&Pos);
	arg.vCamLook = DirectX::XMLoadFloat3(&look);
	arg.vCamUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(arg.vCamFront, arg.vCamSide));

	switch (m_nState)
	{
	case CAM_DCC_ORBIT:		UpdateOrbit(arg);	break;
	case CAM_DCC_TRACK:		UpdateTrack(arg);	break;
	case CAM_DCC_DOLLY:		UpdateDolly(arg);	break;
	case CAM_DCC_FLIGHT:	UpdateFlight(arg);	break;
	}
}

void CameraDCC::UpdateState() noexcept
{
	CameraDCCKind prev = (CameraDCCKind)m_nState;
	if (Input::IsKeyPress(VK_MENU))
	{
		m_nState = CAM_DCC_NONE;
		if (Input::IsKeyPress(VK_LBUTTON)) m_nState = CAM_DCC_ORBIT;
		if (Input::IsKeyPress(VK_MBUTTON)) m_nState = CAM_DCC_TRACK;
		if (Input::IsKeyPress(VK_RBUTTON)) m_nState = CAM_DCC_DOLLY;
	}
	else if (Input::IsKeyPress(VK_RBUTTON))
	{
		m_nState = CAM_DCC_FLIGHT;
	}
	else
	{
		m_nState = CAM_DCC_NONE;
	}
	if (prev != m_nState)
	{
		GetCursorPos(&m_OldPos);
	}
}

void CameraDCC::UpdateOrbit(Argument &In_arg) noexcept
{
	// マウスの移動量 / 画面サイズ の比率から、画面全体でどれだけ回転するか指定する。
	float angleX = 360.0f * In_arg.mouseMove.x * In_arg.speed / 1280.0f;
	float angleY = 180.0f * In_arg.mouseMove.y * In_arg.speed / 720.0f;

	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(
		DirectX::XMVectorSet(0, 1, 0, 0), DirectX::XMConvertToRadians(angleX)
	);
	auto Quat = m_pTransform->GetQuat();
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&Quat);
	qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);

	DirectX::XMVECTOR vAxisX = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	vAxisX = DirectX::XMVector3Rotate(vAxisX, qRotate);
	quat = DirectX::XMQuaternionRotationAxis(vAxisX, DirectX::XMConvertToRadians(angleY));
	qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);

	DirectX::XMStoreFloat4(&Quat, qRotate);
	m_pTransform->SetQuat(Quat);

	// 注視点からカメラの後方へフォーカス距離だけ移動させる
	DirectX::XMFLOAT3 dir = m_pTransform->GetFront();
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat3(&dir);
	vDir = DirectX::XMVectorScale(vDir, -m_pComponent->GetFocus());
	DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(In_arg.vCamLook, vDir);
	auto Pos = m_pTransform->GetPosition();
	DirectX::XMStoreFloat3(&Pos, vPos);
	m_pTransform->SetPosition(Pos);
}

void CameraDCC::UpdateTrack(Argument &In_arg) noexcept
{
	float fClip = m_pComponent->GetFar();

	// 高さA、底辺Bとする三角形について tanΘ = A / Bが成り立つ
	// 上記式をもとに割り出した遠景の高さを、移動量 / 画面サイズ の比率から、移動量として求める
	float width = m_pComponent->GetFovy();
	float farScreenHeight = tanf(width * 0.5f) * fClip;
	float screenRateW = In_arg.mouseMove.x / 640.0f;
	float screenRateH = In_arg.mouseMove.y / 360.0f;
	float farMoveX = -farScreenHeight * screenRateW * m_pComponent->GetAspect();
	float farMoveY = farScreenHeight * screenRateH;

	// カメラの姿勢をもとに移動
	float rate = m_pComponent->GetFocus() / fClip;
	DirectX::XMVECTOR vCamMove = DirectX::XMVectorZero();
	vCamMove = DirectX::XMVectorAdd(vCamMove, DirectX::XMVectorScale(In_arg.vCamSide, farMoveX * rate));
	vCamMove = DirectX::XMVectorAdd(vCamMove, DirectX::XMVectorScale(In_arg.vCamUp, farMoveY * rate));
	vCamMove = DirectX::XMVectorScale(vCamMove, In_arg.speed);
	auto Pos = m_pTransform->GetPosition();
	DirectX::XMStoreFloat3(&Pos, DirectX::XMVectorAdd(In_arg.vCamPos, vCamMove));
	m_pTransform->SetPosition(Pos);
}

void CameraDCC::UpdateDolly(Argument &In_arg) noexcept
{
	float focus = m_pComponent->GetFocus();
	float fClip = m_pComponent->GetFar();
	float nClip = m_pComponent->GetNear();

	// フォーカス位置とクリップ面の位置に応じて補正移動量を計算
	float clipDistance = fClip - nClip;
	float rate = (focus - nClip) / clipDistance;
	rate *= fClip * In_arg.speed * 0.01f; // ファークリップの大きさに応じて移動に補正

	// 移動量
	float move = rate * (In_arg.mouseMove.x + In_arg.mouseMove.y);
	focus = std::min(fClip, std::max(nClip, focus - move));

	// カメラ位置更新
	DirectX::XMVECTOR vMove = DirectX::XMVectorScale(In_arg.vCamFront, -focus);
	auto Pos = m_pTransform->GetPosition();
	DirectX::XMStoreFloat3(&Pos, DirectX::XMVectorAdd(In_arg.vCamLook, vMove));
	m_pComponent->SetFocus(focus);
	m_pTransform->SetPosition(Pos);
}

void CameraDCC::UpdateFlight(Argument &In_arg) noexcept
{
	// マウスの移動量 / 画面サイズ の比率から、画面全体でどれだけ回転するか指定する。
	float angleX = 360.0f * In_arg.mouseMove.x / 1280.0f;
	float angleY = 180.0f * In_arg.mouseMove.y / 720.0f;

	// 横回転
	auto Quat = m_pTransform->GetQuat();
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&Quat);
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(
		In_arg.vCamUp, DirectX::XMConvertToRadians(angleX)
	);
	qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);

	// 縦回転
	DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
	quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(angleY));
	qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);

	// 回転の更新
	DirectX::XMStoreFloat4(&Quat, qRotate);
	m_pTransform->SetQuat(Quat);

	// 軸の取得
	DirectX::XMFLOAT3 front = m_pTransform->GetFront();
	DirectX::XMFLOAT3 side = m_pTransform->GetRight();
	In_arg.vCamFront = DirectX::XMLoadFloat3(&front);
	In_arg.vCamSide = DirectX::XMLoadFloat3(&side);

	// キー入力で移動
	DirectX::XMVECTOR vCamMove = DirectX::XMVectorZero();
	if (Input::IsKeyPress('W')) vCamMove = DirectX::XMVectorAdd(vCamMove, In_arg.vCamFront);
	if (Input::IsKeyPress('S')) vCamMove = DirectX::XMVectorSubtract(vCamMove, In_arg.vCamFront);
	if (Input::IsKeyPress('A')) vCamMove = DirectX::XMVectorSubtract(vCamMove, In_arg.vCamSide);
	if (Input::IsKeyPress('D')) vCamMove = DirectX::XMVectorAdd(vCamMove, In_arg.vCamSide);
	if (Input::IsKeyPress('Q')) vCamMove = DirectX::XMVectorAdd(vCamMove, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	if (Input::IsKeyPress('E')) vCamMove = DirectX::XMVectorAdd(vCamMove, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	vCamMove = DirectX::XMVectorScale(vCamMove, m_pComponent->GetFar() * 0.0001f);
	vCamMove = DirectX::XMVectorScale(vCamMove, In_arg.speed);

	// 更新
	DirectX::XMVECTOR vCamPos = DirectX::XMVectorAdd(In_arg.vCamPos, vCamMove);
	auto Pos = m_pTransform->GetPosition();
	DirectX::XMStoreFloat3(&Pos, vCamPos);
	m_pTransform->SetPosition(Pos);
}
