/*+===================================================================
	File: Missile.cpp
	Summary: ミサイルオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/8 Mon AM 12:44:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Missile.hpp"
#include "DirectX11/System/ModelRenderer.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float cx_MoveSpeedScale = 0.01f; // 移動速度の倍率
}

Missile::Missile()
	: GameObject("Missile"), m_Speed(0.0f)
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Weapon/AIM-120.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する
	SetScale({ MissileScale,MissileScale,MissileScale });
}

Missile::~Missile()
{
}

void Missile::Update()
{
	//// 前方ベクトル取得
	//DirectX::XMFLOAT3 front = GetFront();
	//// 移動
	//m_Pos += ((front * cx_MoveSpeedScale) * m_Speed);
	//GameObject::Update();
}
