/*+===================================================================
	File: Enemy.cpp
	Summary: 敵キャラクター
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 02:08:22 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Enemy.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr DirectX::XMFLOAT3 cx_EnemyScale = { 0.01f, 0.01f, 0.01f };
	const inline constexpr DirectX::XMFLOAT4 cx_EnemyStartQuat = { 0.0f, 0.0f, 0.0f, 1.0f };
	const inline constexpr float cx_PlayerStartSpeed = 100.0f;

	// 移動関連
	const inline constexpr float MOVE_SPEED_SCALE = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_Speed = 300.0f; // 移動速度
}

Enemy::Enemy(_In_ std::string_view In_Name)
	: Component(In_Name)
	, m_pCamera(nullptr)
	, m_IsDestroyed(false)
{
}

Enemy::~Enemy()
{
}

void Enemy::Awake() noexcept
{
	m_pGameObject->SetQuat(cx_EnemyStartQuat);
}

void Enemy::Init() noexcept
{
}

void Enemy::Update(_In_ float In_DeltaTime) noexcept
{
	//if(GetComponent<SphereCollider>()->IsCollision())
	//{
	//	m_IsDestroyed = true;
	//	DestroySelf(); // 自身を破棄
	//	return;
	//}

	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = m_pGameObject->GetFront();

	// 移動
	m_pGameObject->GetTransform()->Translate((front * MOVE_SPEED_SCALE) * cx_Speed);
}

void Enemy::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	m_pCamera = In_Camera;
}
