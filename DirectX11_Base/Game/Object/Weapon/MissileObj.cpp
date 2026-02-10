/*+===================================================================
	File: MissileObj.cpp
	Summary: ミサイルオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/8 Mon AM 12:44:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MissileObj.hpp"
#include "Game/Object/Character/EnemyObj.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float cx_MoveSpeedScale = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_MissileRange = 18000.0f; // ミサイルの射程距離(180km)

	const inline constexpr float cx_SpeedDuration = 60.0f; // 速度変化にかかる時間
	const inline constexpr float cx_StartDuration = 15.0f; // 開始時のイージング時間
}

MissileObj::MissileObj(_In_ std::string In_Name, _In_ const bool &In_IsAddCollider)
	: GameObject(In_Name), m_Speed(0.0f), m_EaseData{}, m_StartPos{}, m_pTarget(nullptr)
	, m_IsAddCollider(In_IsAddCollider)
{
}

MissileObj::~MissileObj()
{
}

void MissileObj::Awake() noexcept
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Weapon/AIM-120.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する
	SetScale({ MissileScale,MissileScale,MissileScale });

	//if (m_IsAddCollider)
	//{
	//	// コライダー追加
	//	auto collider = AddComponent<SphereCollider>();
	//	collider->SetRadius(1.0f); // 半径を設定
	//}
}

void MissileObj::SetSpeed(_In_ const float &In_Speed) noexcept
{
	m_Speed = In_Speed;
	m_EaseData.fStart = 0.0f;
	m_EaseData.fEnd = In_Speed;
	m_EaseData.fNowTime = cx_StartDuration;
	m_EaseData.fDuration = cx_SpeedDuration; // 目標速度に到達するまでの時間
}
