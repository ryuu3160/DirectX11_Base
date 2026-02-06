/*+===================================================================
	File: Player.cpp
	Summary: プレイヤーオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 04:04:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "PlayerObj.hpp"
#include "Game/Script/Character/Player.hpp"
#include "Game/Script/Missile.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	// プレイヤーの初期位置
	const inline constexpr DirectX::XMFLOAT3 cx_PlayerStartPos = { 0.0f, 200.0f, 0.0f };
	const inline constexpr DirectX::XMFLOAT3 cx_PlayerScale = { 0.005f, 0.005f, 0.005f };
	const inline constexpr DirectX::XMFLOAT4 cx_PlayerStartQuat = { 0.0f, 0.0f, 0.0f, 1.0f };
	const inline constexpr float cx_PlayerStartSpeed = 100.0f;

	// 移動関連
	const inline constexpr float MOVE_SPEED_SCALE = 0.01f; // 移動速度の倍率
	const inline constexpr float cx_SpeedAdd = 0.5f;		// 速度増減量
	const inline constexpr float cx_SpeedMinus = 0.2f;	// 速度増減量
	const inline constexpr float cx_PitchSpeed = 0.2f;		// ピッチ速度
	const inline constexpr float cx_BrakeSpeed = 0.5f;		// 急ブレーキ速度
	const inline constexpr float cx_SharpPitch = 2.5f;		// 鋭いピッチ時のオフセット値
	const inline constexpr float cx_YawSpeed = 0.01f;		// ヨー速度
	const inline constexpr float cx_RollSpeed = 0.9f;		// ロール速度
	const inline constexpr float cx_MaxSpeed_ms = 737.71f; // 最大速度（m/s）=マッハ2.5(60fps換算)
	const inline constexpr float cx_MinSpeed_ms = 49.05f;  // 最小速度（m/s）=マッハ0.167(60fps換算)

	// 機体スペック関連
	const inline constexpr float cx_MaximumAscentAltitude = 15000.0f; // 最大上昇高度
}

PlayerObj::PlayerObj(_In_ std::string_view In_Name)
	: GameObject(In_Name.data())
{
}

PlayerObj::~PlayerObj()
{
}

void PlayerObj::Awake() noexcept
{
	auto Model = AddComponent<ModelRenderer>();
	auto PlayerScript = AddComponent<Player>();
	Model->SetAssetPath("Assets/Model/Character/F15E/F15E.fbx");
	Model->SetVertexShader(ShaderManager::GetInstance().GetShader("VS_Object"));
	Model->SetPixelShader(ShaderManager::GetInstance().GetShader("PS_TexColor"));
	Model->IsUseMaterialShader(true); // マテリアルシェーダーを使用する


	SetPosition(cx_PlayerStartPos);
	SetScale(cx_PlayerScale);
	SetQuat(cx_PlayerStartQuat);
}

void PlayerObj::Init() noexcept
{
}