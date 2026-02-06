/*+===================================================================
	File: Enemy.cpp
	Summary: 敵キャラクター
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 02:08:22 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "EnemyObj.hpp"

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

EnemyObj::EnemyObj(_In_ std::string In_Name)
	: GameObject(In_Name)
	, m_pCamera(nullptr)
	, m_IsDestroyed(false)
{
}

EnemyObj::~EnemyObj()
{
}

void EnemyObj::Awake() noexcept
{
	auto Model = AddComponent<ModelRenderer>();
	Model->SetAssetPath("Assets/Model/Character/Su25/SU25.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する

	//// コライダー追加
	//auto collider = AddComponent<SphereCollider>();
	//collider->SetRadius(2.0f); // 半径を設定

	auto icon = GetScene()->CreateObject<GameObject>(m_Name + "_Icon"); // アイコン用の子オブジェクトを追加
	icon->SetPosition(GetUp() * 4.0f); // アイコンの位置を設定
	icon->SetScale({ 200.0f, 200.0f, 200.0f }); // アイコンのスケールを設定)
	auto sprite = icon->AddComponent<SpriteRenderer>();
	sprite->SetAssetPath("Assets/Texture/EnemyIcon.png");
	sprite->Set3D(true); // 3Dスプライトに設定
	sprite->SetBillBoard(true); // ビルボードに設定
	sprite->Load();
	sprite->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 色を設定

	SetScale(cx_EnemyScale);
	SetQuat(cx_EnemyStartQuat);
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
	DirectX::XMFLOAT3 front = GetFront();

	// 移動
	m_pTransform->Translate((front * MOVE_SPEED_SCALE) * cx_Speed);
}

void Enemy::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	m_pCamera = In_Camera;
}
