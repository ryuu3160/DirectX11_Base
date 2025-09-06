/*+===================================================================
	File: Player.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 04:04:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Player.hpp"
#include "DirectX11/System/ModelRenderer.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float MOVE_SPEED_SCALE = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_SpeedAdd = 0.5f;		// 速度増減量
	const inline constexpr float cx_SpeedMinus = 0.2f;	// 速度増減量
	const inline constexpr float cx_PitchSpeed = 0.2f;		// ピッチ速度
	const inline constexpr float cx_BrakeSpeed = 0.5f;		// 急ブレーキ速度
	const inline constexpr float cx_SharpPitch = 2.5f;		// 鋭いピッチ時のオフセット値
	const inline constexpr float cx_YawSpeed = 0.01f;		// ヨー速度
	const inline constexpr float cx_RollSpeed = 0.9f;		// ロール速度

	// キー定義
	const inline constexpr char LeftYawKey = 'Q';		// 左ヨー
	const inline constexpr char RightYawKey = 'E';	// 右ヨー
	const inline constexpr char LeftRollKey = 'A';	// 左ロール
	const inline constexpr char RightRollKey = 'D';	// 右ロール
	const inline constexpr char PitchUpKey = 'W';	// ピッチUP
	const inline constexpr char PitchDownKey = 'S';	// ピッチDown
	const inline constexpr char SpeedUpKey = 'I';	// 速度UP
	const inline constexpr char SpeedDownKey = 'K';	// 速度Down
	const inline constexpr char BrakeKey = 'U';	// 急ブレーキ
	const inline constexpr char ShootKey = VK_SPACE;	// 射撃
}

Player::Player()
	: GameObject("Player")
	, m_fSpeed(1.0f)
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Character/F15E.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する

	SetPos({ 0.0f, 0.0f, 0.0f });
	SetScale({ 0.005f, 0.005f, 0.005f });
}

Player::~Player()
{
}

void Player::Update()
{
	UpdateMovement();

	

	GameObject::Update();
}

void Player::UpdateMovement()
{
	// ----- 旋回処理 -----
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	DirectX::XMVECTOR quat;

	// ヨー
	if (Input::IsKeyPress(LeftYawKey))
	{
		// 左ヨー
		DirectX::XMFLOAT3 up = GetUp();
		quat = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), -cx_YawSpeed);
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress(RightYawKey))
	{
		// 右ヨー
		DirectX::XMFLOAT3 up = GetUp();
		quat = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), cx_YawSpeed);
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ロール
	if (Input::IsKeyPress(LeftRollKey))
	{
		// 左ロール
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(cx_RollSpeed));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress(RightRollKey))
	{
		// 右ロール
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(-cx_RollSpeed));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ピッチ
	if (Input::IsKeyPress(PitchUpKey))
	{
		// 鋭いピッチ時のオフセット値取得
		float offset = GetSharpPitch();

		// ピッチUP
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(-cx_PitchSpeed * offset));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress(PitchDownKey))
	{
		// 鋭いピッチ時のオフセット値取得
		float offset = GetSharpPitch();

		// ピッチDown
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(cx_PitchSpeed * offset));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ----- 速度処理 -----

	if (Input::IsKeyPress(BrakeKey))
	{
		// 急ブレーキ
		m_fSpeed -= cx_BrakeSpeed;
		if (m_fSpeed < 0.0f)
			m_fSpeed = 0.0f;
	}
	else if (Input::IsKeyPress(SpeedUpKey)) // 急ブレーキ中は速度UPしない
	{
		m_fSpeed += cx_SpeedAdd;
	}

	if (Input::IsKeyPress(SpeedDownKey))
	{
		m_fSpeed -= cx_SpeedMinus;

		if (m_fSpeed < 0.0f)
			m_fSpeed = 0.0f;
	}

	// 回転の更新
	DirectX::XMStoreFloat4(&m_Quat, qRotate);

	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = GetFront();

	// 移動
	m_Pos += ((front * MOVE_SPEED_SCALE) * m_fSpeed);
}

void Player::UpdateShoot()
{
}

float Player::GetSharpPitch()
{
	if (Input::IsKeyPress(BrakeKey))
		return cx_SharpPitch;
	return 1.0f;
}
