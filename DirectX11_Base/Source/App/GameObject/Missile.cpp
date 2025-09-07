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

Missile::Missile()
	: GameObject("Missile")
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
}
