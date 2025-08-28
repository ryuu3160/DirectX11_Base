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
}

Player::Player()
	: GameObject("Player")
	, m_fSpeed(1.0f)
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/F15E.fbx");
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
	if (Input::IsKeyPress('Q'))
	{
		// 左ヨー
		DirectX::XMFLOAT3 up = GetUp();
		quat = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), -0.01f);
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress('E'))
	{
		// 右ヨー
		DirectX::XMFLOAT3 up = GetUp();
		quat = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), 0.01f);
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ロール
	if (Input::IsKeyPress('A'))
	{
		// 左ロール
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(1.0f));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress('D'))
	{
		// 右ロール
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(-1.0f));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ピッチ
	if (Input::IsKeyPress(VK_SPACE))
	{
		// ピッチUP
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(-1.0f));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress(VK_LSHIFT))
	{
		// ピッチDown
		DirectX::XMVECTOR vAxis = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vAxis = DirectX::XMVector3Rotate(vAxis, qRotate);
		quat = DirectX::XMQuaternionRotationAxis(vAxis, DirectX::XMConvertToRadians(1.0f));
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}

	// ----- 速度処理 -----

	if (Input::IsKeyPress('W'))
	{
		m_fSpeed += 0.1f;
	}
	if (Input::IsKeyPress('S'))
	{
		m_fSpeed -= 0.1f;

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
