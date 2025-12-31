/*+===================================================================
	File: Transform.cpp
	Summary: Transformクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/23 Tue PM 06:37:47 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Transform.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

Transform::Transform()
	: Component("Transform")
	, m_Pos{ 0.0f, 0.0f, 0.0f }, m_Scale{ 1.0f, 1.0f, 1.0f }, m_Quat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_Euler{ 0.0f, 0.0f, 0.0f }
	, m_IsSyncEuler(false), m_AccumEuler{ 0.0f, 0.0f, 0.0f }
{
}

Transform::~Transform()
{
}

void Transform::Init() noexcept
{
}

void Transform::SaveLoad(_In_ DataAccessor *In_Data)
{
	In_Data->AccessValue<DirectX::XMFLOAT3>("Position", &m_Pos);
	In_Data->AccessValue<DirectX::XMFLOAT3>("Scale", &m_Scale);
	In_Data->AccessValue<DirectX::XMFLOAT4>("Quat", &m_Quat);
	In_Data->AccessValue<DirectX::XMFLOAT3>("Euler", &m_Euler);
}

void Transform::Rotate(_In_ float In_PitchDeg, _In_ float In_YawDeg, _In_ float In_RollDeg) noexcept
{
	// 増分をクォータニオンに変換
	DirectX::XMVECTOR deltaQuat = DirectX::XMQuaternionRotationRollPitchYaw(
		ToRad(In_PitchDeg),
		ToRad(In_YawDeg),
		ToRad(In_RollDeg)
	);
	// 現在の回転と合成（クォータニオンの積）
	DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&m_Quat);
	DirectX::XMVECTOR newQuat = DirectX::XMQuaternionMultiply(currentQuat, deltaQuat);

	// 正規化して保存
	newQuat = DirectX::XMQuaternionNormalize(newQuat);
	DirectX::XMStoreFloat4(&m_Quat, newQuat);
	
	// オイラー角の値も累積
	m_Euler.x += In_PitchDeg;
	m_Euler.y += In_YawDeg;
	m_Euler.z += In_RollDeg;
	m_AccumEuler.x += In_PitchDeg;
	m_AccumEuler.y += In_YawDeg;
	m_AccumEuler.z += In_RollDeg;

	// オイラー角の増減値を-180～180度の範囲に収める
	if(m_AccumEuler.x > 180.0f)
	{
		m_AccumEuler.x -= 360.0f;
		m_Euler.x -= 360.0f;
	}
	else if(m_AccumEuler.x < -180.0f)
	{
		m_AccumEuler.x += 360.0f;
		m_Euler.x += 360.0f;
	}
	if(m_AccumEuler.y > 180.0f)
	{
		m_AccumEuler.y -= 360.0f;
		m_Euler.y -= 360.0f;
	}
	else if(m_AccumEuler.y < -180.0f)
	{
		m_AccumEuler.y += 360.0f;
		m_Euler.y += 360.0f;
	}
	if(m_AccumEuler.z > 180.0f)
	{
		m_AccumEuler.z -= 360.0f;
		m_Euler.z -= 360.0f;
	}
	else if(m_AccumEuler.z < -180.0f)
	{
		m_AccumEuler.z += 360.0f;
		m_Euler.z += 360.0f;
	}
}

DirectX::XMFLOAT3 Transform::GetRotation(_In_ bool In_IsDegree) noexcept
{
	// クォータニオンからオイラー角を取得
	m_Euler = SyncEulerFromQuat();

	if(!In_IsDegree)
		return ToRad(m_Euler);
	return m_Euler;
}

DirectX::XMFLOAT3 Transform::GetFront(_In_ const bool &Is_Normalize) const noexcept
{
	// 前方ベクトルを取得
	DirectX::XMVECTOR vFront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// クォータニオンを使って回転
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	vFront = DirectX::XMVector3Rotate(vFront, qRotate);
	DirectX::XMFLOAT3 dir;
	// 正規化してXMFLOAT3に変換
	if(Is_Normalize)
		DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vFront));
	else
		DirectX::XMStoreFloat3(&dir, vFront);
	return dir;
}

DirectX::XMFLOAT3 Transform::GetRight() const noexcept
{
	// 右方向ベクトルを取得
	DirectX::XMVECTOR vRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	// クォータニオンを使って回転
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	vRight = DirectX::XMVector3Rotate(vRight, qRotate);
	DirectX::XMFLOAT3 dir;
	// 正規化してXMFLOAT3に変換
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vRight));
	return dir;
}

DirectX::XMFLOAT3 Transform::GetUp() const noexcept
{
	// 上方向ベクトルを取得
	DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	// クォータニオンを使って回転
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	vUp = DirectX::XMVector3Rotate(vUp, qRotate);
	DirectX::XMFLOAT3 dir;
	// 正規化してXMFLOAT3に変換
	DirectX::XMStoreFloat3(&dir, vUp);
	return dir;
}

void Transform::SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	m_Pos = In_Pos;
}

void Transform::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	// 回転を設定
	auto Rot = ToRad(In_Rotation);
	// クォータニオンに変換
	DirectX::XMStoreFloat4(&m_Quat, DirectX::XMQuaternionRotationRollPitchYaw(Rot.x, Rot.y, Rot.z));
	m_IsSyncEuler = true;
}

void Transform::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	// 拡縮を設定
	m_Scale = In_Scale;
}

void Transform::SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	// クォータニオンを設定
	m_Quat = In_Quat;
	m_IsSyncEuler = true;
}

void Transform::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	// トランスフォームグループの作成
	ItemGroup *group = In_pWindow->CreateItem<ItemGroup>("Transform");
	group->CreateGroupItem<ItemBind>("Pos", DebugItem::Kind::Vector, &m_Pos);
	group->CreateGroupItem<ItemCallback>("Rotation", DebugItem::Kind::Vector,
		[this](bool IsWrite, void *arg)
		{
			DirectX::XMFLOAT3 *pVec = static_cast<DirectX::XMFLOAT3 *>(arg);
			if(IsWrite)
			{
				m_Euler = *pVec;
				DirectX::XMStoreFloat4(&m_Quat,
					DirectX::XMQuaternionRotationRollPitchYaw( // zxy
						ToRad(m_Euler.x), // pitch
						ToRad(m_Euler.y), // yaw
						ToRad(m_Euler.z))); // roll

				// オイラー角の累積はリセット
				m_AccumEuler = { 0.0f, 0.0f, 0.0f };
				m_IsSyncEuler = false;
			}
			else
			{
				if(m_IsSyncEuler)
				{
					m_Euler = SyncEulerFromQuat();
					m_IsSyncEuler = false;
				}
				pVec->x = m_Euler.x;
				pVec->y = m_Euler.y;
				pVec->z = m_Euler.z;
			}
		});
	group->CreateGroupItem<ItemBind>("Scale", DebugItem::Kind::Vector, &m_Scale);
}

DirectX::XMFLOAT3 Transform::SyncEulerFromQuat() noexcept
{
	std::vector<DirectX::XMFLOAT3> candidates = GetPracticalEulerCandidates();

	// 現在のオイラー角を-180～180度の範囲に収める
	m_Euler = NormalizeEulerDegAngles(m_Euler);

	return ChoiceBestEuler(candidates);
}

DirectX::XMFLOAT3 Transform::ChoiceBestEuler(_In_ const std::vector<DirectX::XMFLOAT3> In_Condidates) const noexcept
{
	// 前フレームとの差が最小のものを選ぶ
	float MinDist = FLT_MAX;
	DirectX::XMFLOAT3 Best = In_Condidates[0];
	auto RadEular = ToRad(m_Euler);

	for(auto &itr : In_Condidates)
	{
		float dist = std::abs(itr.x - RadEular.x)
			+ std::abs(itr.y - RadEular.y)
			+ std::abs(itr.z - RadEular.z);

		if(dist < MinDist)
		{
			MinDist = dist;
			Best = itr;
		}
	}

	return ToDeg(Best);
}

std::vector<DirectX::XMFLOAT3> Transform::GetPracticalEulerCandidates() const noexcept
{
	std::vector<DirectX::XMFLOAT3> candidates;
	auto BaseEuler = DX11Math::QuaternionToRollPitchYaw(m_Quat);

	float pitch = BaseEuler.x;
	float yaw = BaseEuler.y;
	float roll = BaseEuler.z;

	// ジンバルロック判定
	const float gimbalThreshold = DirectX::XM_PIDIV2 - 0.01f;
	bool isGimbalLock = (std::abs(pitch) > gimbalThreshold);

	if(isGimbalLock)
	{
		// ジンバルロック時 → 多数の候補を生成
		return GenerateGimbalLockCandidates(pitch, yaw, roll);
	}
	else
	{
		// 1. 基本解
		candidates.push_back(DirectX::XMFLOAT3(pitch, yaw, roll));

		// 2. π 反転パターン
		candidates.push_back(DirectX::XMFLOAT3(
			PI - pitch,
			yaw + PI,
			roll + PI
		));

		// 3. 負の Pitch パターン
		candidates.push_back(DirectX::XMFLOAT3(
			-pitch,
			yaw + PI,
			roll
		));

		// 4. 2π 周期性（各軸）
		candidates.push_back(DirectX::XMFLOAT3(pitch + TWO_PI, yaw, roll));
		candidates.push_back(DirectX::XMFLOAT3(pitch - TWO_PI, yaw, roll));
		candidates.push_back(DirectX::XMFLOAT3(pitch, yaw + TWO_PI, roll));
		candidates.push_back(DirectX::XMFLOAT3(pitch, yaw - TWO_PI, roll));
		candidates.push_back(DirectX::XMFLOAT3(pitch, yaw, roll + TWO_PI));
		candidates.push_back(DirectX::XMFLOAT3(pitch, yaw, roll - TWO_PI));

		// 5. 複合パターン（π 反転 + 2π）
		candidates.push_back(DirectX::XMFLOAT3(
			PI - pitch,
			yaw + PI + TWO_PI,
			roll + PI
		));
		candidates.push_back(DirectX::XMFLOAT3(
			PI - pitch,
			yaw + PI - TWO_PI,
			roll + PI
		));
		candidates.push_back(DirectX::XMFLOAT3(
			PI - pitch,
			yaw + PI,
			roll + PI + TWO_PI
		));
		candidates.push_back(DirectX::XMFLOAT3(
			PI - pitch,
			yaw + PI,
			roll + PI - TWO_PI
		));

		candidates.push_back(DirectX::XMFLOAT3(
			-pitch,
			yaw + PI + TWO_PI,
			roll
		));
		candidates.push_back(DirectX::XMFLOAT3(
			-pitch,
			yaw + PI - TWO_PI,
			roll
		));

		return candidates;
	}
}

std::vector<DirectX::XMFLOAT3> Transform::GenerateGimbalLockCandidates(_In_ float In_Pitch, _In_ float In_Yaw, _In_ float In_Roll, _In_ float In_StepDeg) const noexcept
{
	std::vector<DirectX::XMFLOAT3> Candidates;

	const float GimbalThreshold = DirectX::XM_PIDIV2 - 0.01f;
	bool IsNorthPole = (In_Pitch > GimbalThreshold);
	bool IsSouthPole = (In_Pitch < -GimbalThreshold);

	if(!IsNorthPole && !IsSouthPole)
	{
		Candidates.push_back(DirectX::XMFLOAT3(In_Pitch, In_Yaw, In_Roll));
		return Candidates;
	}

	float PitchLocked = IsNorthPole ? DirectX::XM_PIDIV2 : -DirectX::XM_PIDIV2;
	float CombinedAngle = IsNorthPole ? (In_Yaw - In_Roll) : (In_Yaw + In_Roll);

	// 角度の候補リスト(Degree)
	std::vector<float> YawDegrees;

	// 1.  主要な角度を追加
	YawDegrees.push_back(0.0f);                                          // 0度
	YawDegrees.push_back(DirectX::XMConvertToDegrees(In_Yaw));           // 元の Yaw

	// 2. 角度刻みで候補を生成
	for(float deg = -180.0f; deg < 180.0f; deg += In_StepDeg)
	{
		YawDegrees.push_back(deg);
	}

	// 3. 重複を除去
	std::sort(YawDegrees.begin(), YawDegrees.end());
	YawDegrees.erase(std::unique(YawDegrees.begin(), YawDegrees.end(),
		[](float a, float b) { return std::abs(a - b) < 0.01f; }),
		YawDegrees.end());

	// 4. 候補を生成
	for(float yawDeg : YawDegrees)
	{
		float TestYaw = DirectX::XMConvertToRadians(yawDeg);

		// -π ~ πに正規化
		TestYaw = NormalizeRadAngle(TestYaw);

		float TestRoll;
		if(IsNorthPole)
		{
			TestRoll = TestYaw - CombinedAngle;
		}
		else
		{
			TestRoll = CombinedAngle - TestYaw;
		}

		// Rollを正規化
		TestRoll = NormalizeRadAngle(TestRoll);

		Candidates.push_back(DirectX::XMFLOAT3(PitchLocked, TestYaw, TestRoll));
	}

	return Candidates;
}
