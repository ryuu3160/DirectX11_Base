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

Player::Player()
	: GameObject("Player")
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/F15E.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する

	SetPos({ 2.0f, 0.0f, 0.0f });
	SetScale({ 0.005f, 0.005f, 0.005f });
}

Player::~Player()
{
}

void Player::Update()
{
	if(GetAsyncKeyState('A') & 0x8000)
	{
		m_Rotation.y += 0.01f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_Rotation.y -= 0.01f;
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_Rotation.z += 0.01f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_Rotation.z -= 0.01f;
	}
	GameObject::Update();
}
