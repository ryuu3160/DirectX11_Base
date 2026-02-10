/*+===================================================================
	File: Player.cpp
	Summary: プレイヤーの処理
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 04:04:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Player.hpp"
#include "Game/Object/Weapon/MissileObj.hpp"
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

	// キー定義
	inline constexpr char LeftYawKey = 'Q';		// 左ヨー
	inline constexpr char RightYawKey = 'E';	// 右ヨー
	inline constexpr char LeftRollKey = 'A';	// 左ロール
	inline constexpr char RightRollKey = 'D';	// 右ロール
	inline constexpr char PitchUpKey = 'W';	// ピッチUP
	inline constexpr char PitchDownKey = 'S';	// ピッチDown
	inline constexpr char SpeedUpKey = 'I';	// 速度UP
	inline constexpr char SpeedDownKey = 'K';	// 速度Down
	inline constexpr char BrakeKey = 'U';	// 急ブレーキ
	inline constexpr char ShootKey = VK_SPACE;	// 射撃

	// ミサイル関連
	inline constexpr float cx_MissileScale = 1.4f; // ミサイルの相対スケール
	inline constexpr float cx_MissileReloadTime = 5.0f; // ミサイルのリロード時間
	inline constexpr float cx_MissileSpeed = 1180.4f; // ミサイルの速度
	inline constexpr int cx_MissileMax = 4; // ミサイルの最大数
}

Player::Player(_In_ std::string_view In_Name)
	: Component(In_Name.data())
	, m_fSpeed(1.0f), m_pCamera(nullptr), m_ShotMissileNum(0)
	, m_IsDestroyed(false)
{
}

Player::~Player()
{
}

void Player::Awake() noexcept
{
}

void Player::Init() noexcept
{
	// SE追加
	auto &SoundM = SoundManager::GetInstance();
	SoundM.Load("Missile", "Assets/Sound/SE/Missile.wav", true, false);
	SoundM.Load("Explosion", "Assets/Sound/SE/small_explosion.wav", true, false);
}

void Player::Update(_In_ float In_DeltaTime) noexcept
{
	UpdateMovement();
	UpdateReload();
	UpdateChildMissile();
	UpdateShoot();
	
	if (m_pGameObject->GetPosition().y < 0.0f && !m_IsDestroyed)
	{
		m_IsDestroyed = true;
		SoundManager::GetInstance().Play("Explosion");
		m_fSpeed = 0.0f;
		m_pGameObject->GetTransform()->Translate(0.0f, -2.0f, 0.0f); // 地面にめり込ませる
	}
}

void Player::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	m_pCamera = In_Camera;
}

void Player::UpdateMovement()
{
	// ----- 旋回処理 -----
	DirectX::XMFLOAT4 Quat = m_pGameObject->GetQuat();
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&Quat);
	DirectX::XMVECTOR quat;

	// ヨー
	if (Input::IsKeyPress(LeftYawKey))
	{
		// 左ヨー
		DirectX::XMFLOAT3 up = m_pGameObject->GetUp();
		quat = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&up), -cx_YawSpeed);
		qRotate = DirectX::XMQuaternionMultiply(qRotate, quat);
	}
	if (Input::IsKeyPress(RightYawKey))
	{
		// 右ヨー
		DirectX::XMFLOAT3 up = m_pGameObject->GetUp();
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
		if (m_fSpeed < cx_MinSpeed_ms)
			m_fSpeed = cx_MinSpeed_ms;
	}
	else if (Input::IsKeyPress(SpeedUpKey)) // 急ブレーキ中は速度UPしない
	{
		m_fSpeed += cx_SpeedAdd;
		if(m_fSpeed > cx_MaxSpeed_ms)
			m_fSpeed = cx_MaxSpeed_ms;
	}

	if (Input::IsKeyPress(SpeedDownKey))
	{
		m_fSpeed -= cx_SpeedMinus;

		if (m_fSpeed < cx_MinSpeed_ms)
			m_fSpeed = cx_MinSpeed_ms;
	}

	// 回転の更新
	DirectX::XMStoreFloat4(&Quat, qRotate);

	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = m_pGameObject->GetFront();

	// 移動
	m_pGameObject->GetTransform()->Translate((front * MOVE_SPEED_SCALE) * m_fSpeed);
}

void Player::UpdateShoot()
{
	// 撃てるミサイルが無ければ処理しない
	if (m_MissileIndices.empty())
		return;

	if (Input::IsKeyTrigger(ShootKey))
	{
		// ミサイル発射SE
		SoundManager::GetInstance().Play("Missile");
		std::string name = "Missile" + std::to_string(m_ShotMissileNum);
		auto obj = m_pGameObject->GetScene()->CreateObject<MissileObj>(name,nullptr,false);
		DirectX::XMFLOAT3 Pos = m_pGameObject->GetPosition();
		DirectX::XMVECTOR pos1 = DirectX::XMLoadFloat3(&Pos);
		auto targetPos = m_pTarget->GetPosition();
		DirectX::XMVECTOR pos2 = DirectX::XMLoadFloat3(&targetPos);
		DirectX::XMVECTOR sub = DirectX::XMVectorSubtract(pos1, pos2);
		float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(sub));
		// ターゲットが一定距離内にいる場合のみターゲットをセット
		if(length < 20000.0f)
			obj->SetTarget(m_pTarget);
		auto cmp = obj->GetComponent<ModelRenderer>();
		cmp->SetLayer(-1); // キャノピー越しに見えるようにする
		++m_ShotMissileNum; // 発射したミサイルの番号をインクリメント

		// ミサイルの初期位置を設定
		DirectX::XMFLOAT3 pos = m_pGameObject->GetTransform()->FindChild("Missile" + std::to_string(m_MissileIndices[0]))->GetPosition();
		obj->SetPosition(pos);
		obj->SetStartPosition(pos);
		obj->SetQuat(m_pGameObject->GetQuat());
		obj->SetSpeed(cx_MissileSpeed); // 自機の速度に+2.0fした速度で発射

		// 子オブジェクトを削除
		m_pGameObject->GetTransform()->FindChild("Missile" + std::to_string(m_MissileIndices[0]))->DestroySelf();
		
		// リロードタイマーをセット
		m_ReloadTimer.push_back({ m_MissileIndices[0], cx_MissileReloadTime });
		m_MissileIndices.erase(m_MissileIndices.begin()); // 先頭のミサイルインデックスを削除
	}
}

void Player::UpdateReload()
{
	// 1発でもミサイルが打たれたらリロード開始
	if (m_MissileIndices.size() < cx_MissileMax)
	{
		auto itr = m_ReloadTimer.begin();
		for (;itr != m_ReloadTimer.end();)
		{
			(*itr).second -= 1.0f / 60.0f; // 1フレーム分減少
			if ((*itr).second <= 0.0f)
			{
				m_MissileIndices.push_back((*itr).first); // ミサイルインデックスを追加
				std::string name = "Missile" + std::to_string((*itr).first);
				auto obj = m_pGameObject->GetScene()->CreateObject<MissileObj>(name, m_pGameObject->GetTransform(), false);
				obj->SetScale({ cx_MissileScale,cx_MissileScale,cx_MissileScale });
				// ミサイルの初期位置を設定
				DirectX::XMFLOAT3 pos = m_pGameObject->GetUp() * 1.03f;
				pos -= m_pGameObject->GetFront() * 0.5f; // 少し後方にオフセット
				switch ((*itr).first)
				{
				case 0:
					pos += m_pGameObject->GetRight() * 1.68f; // 右側
					break;
				case 1:
					pos += m_pGameObject->GetRight() * 1.26f; // 右側
					break;
				case 2:
					pos -= m_pGameObject->GetRight() * 1.26f; // 左側
					break;
				case 3:
					pos -= m_pGameObject->GetRight() * 1.68f; // 左側
					break;
				}
				obj->SetPosition(pos);
				itr = m_ReloadTimer.erase(itr);
			}
			else
				++itr;
		}
		
	}
}

void Player::UpdateChildMissile()
{
	auto missiles = m_pGameObject->GetTransform()->GetChildren();
	for (auto &itr : missiles)
	{
		// 位置更新
		DirectX::XMFLOAT3 pos = m_pGameObject->GetUp() * 1.03f;
		pos -= m_pGameObject->GetFront() * 0.5f; // 少し後方にオフセット
		int num = FromString<int>(itr->GetGameObject()->GetName().substr(7)); // "Missile"の7文字目以降を数値に変換
		switch (num)
		{
		case 0:
			pos += m_pGameObject->GetRight() * 1.68f; // 右側
			break;
		case 1:
			pos += m_pGameObject->GetRight() * 1.26f; // 右側
			break;
		case 2:
			pos -= m_pGameObject->GetRight() * 1.26f; // 左側
			break;
		case 3:
			pos -= m_pGameObject->GetRight() * 1.68f; // 左側
			break;
		}
		itr->SetPosition(pos);
	}
}

float Player::GetSharpPitch()
{
	if (Input::IsKeyPress(BrakeKey))
		return cx_SharpPitch;
	return 1.0f;
}
