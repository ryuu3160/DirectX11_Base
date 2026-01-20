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
#include "Core/DirectX11/System/DX11_Math.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include "Core/System/Object/GameObject.hpp"

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
	, m_pParent(nullptr)
	, m_WorldMatrixDirty(true)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransposeWorldMatrix, DirectX::XMMatrixIdentity());
}

Transform::~Transform()
{
	DetachChildren();
	if(m_pParent)
		m_pParent->RemoveChild(this);
	m_pParent = nullptr;
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
	In_Data->AccessValue<std::string>("ParentName", &m_ParentName);
	MarkDirty();
}

void Transform::SetParent(_In_opt_ Transform *In_Parent, _In_ bool In_IsWorldPositionStays)
{
	// 親が同じ場合は何もしない
	if(m_pParent == In_Parent)
		return;

	// 自分自身を親にすることはできない
	if(In_Parent == this)
	{
		DebugManager::GetInstance().DebugLogError("Cannot set self as parent!");
		return;
	}

	// 循環参照チェック(In_Parentが自分の子孫でないか)
	if(In_Parent)
	{
		Transform *ancestor = In_Parent;
		while(ancestor != nullptr)
		{
			if(ancestor == this)
			{
				DebugManager::GetInstance().DebugLogError("Circular parent reference detected!");
				return;
			}
			ancestor = ancestor->m_pParent;
		}
	}

	// 保存されている親の名前と一致した場合はワールド座標を維持しない
	bool MatchParentName = false;
	if(In_Parent)
	{
		std::string ParentName = In_Parent->GetGameObject()->GetName();
		if(ParentName == m_ParentName)
		{
			MatchParentName = true;
		}
	}

	// ワールド座標を保存
	DirectX::XMFLOAT3 worldPos;
	DirectX::XMFLOAT4 worldRot;
	DirectX::XMFLOAT3 worldScale;

	if(In_IsWorldPositionStays && !MatchParentName)
	{
		worldPos = GetPosition();
		worldRot = GetQuat();
		worldScale = GetScale();
	}

	// 既存の親から削除
	if(m_pParent)
	{
		m_pParent->RemoveChild(this);
		m_ParentName.clear();
	}

	// 新しい親を設定
	m_pParent = In_Parent;

	if(m_pParent)
	{
		// 親の名前を保存
		m_ParentName = m_pParent->GetGameObject()->GetName();

		// 新しい親に追加
		m_pParent->AddChild(this);
	}

	// ワールド座標を復元
	if(In_IsWorldPositionStays && !MatchParentName)
	{
		SetPosition(worldPos);
		SetQuat(worldRot);
		SetScale(worldScale);
	}
}

Transform *Transform::GetRoot()
{
	return GetRootRecursive(this);
}

Transform *Transform::GetChild(_In_ int In_Index) const
{
	if(In_Index < 0 || In_Index >= static_cast<int>(m_Children.size()))
	{
		DebugManager::GetInstance().DebugLogError("Child index out of range!");
		return nullptr;
	}

	return m_Children[In_Index];
}

Transform *Transform::FindChild(_In_ std::string_view In_Name)
{
	return FindRecursive(this, In_Name);
}

const std::vector<Transform *> &Transform::GetChildren() const
{
	return m_Children;
}

bool Transform::IsChildOf(_In_ Transform *In_Transform) const noexcept
{
	if(!In_Transform)
		return false;

	Transform *current = m_pParent;
	while(current != nullptr)
	{
		if(current == In_Transform)
			return true;
		current = current->m_pParent;
	}

	return false;
}

void Transform::DetachChildren() noexcept
{
	// すべての子の親を nullptr に設定
	for(Transform *child : m_Children)
	{
		child->m_pParent = nullptr;
	}
	m_Children.clear();
}

void Transform::Translate(_In_ const DirectX::XMFLOAT3 &In_Translate) noexcept
{
	m_Pos += In_Translate;
	MarkDirty();
}

void Transform::Translate(_In_ float In_X, _In_ float In_Y, _In_ float In_Z) noexcept
{
	m_Pos.x += In_X;
	m_Pos.y += In_Y;
	m_Pos.z += In_Z;
	MarkDirty();
}

void Transform::Rotate(_In_ const DirectX::XMFLOAT3 &In_Angle) noexcept
{
	Rotate(In_Angle.x, In_Angle.y, In_Angle.z);
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

	MarkDirty();
}

void Transform::RotateAround(_In_ const DirectX::XMFLOAT3 &In_Point, _In_ const DirectX::XMFLOAT3 &In_Axis, _In_ float In_Angle)
{
	DirectX::XMVECTOR PointVec = DirectX::XMLoadFloat3(&In_Point);
	DirectX::XMVECTOR AxisVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&In_Axis));
	float Radians = DirectX::XMConvertToRadians(In_Angle);

	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(AxisVec, Radians);

	// 現在位置をIn_Point基準に変換
	auto CurrentPos = GetPosition();
	DirectX::XMVECTOR VecCurrentPos = DirectX::XMLoadFloat3(&CurrentPos);
	DirectX::XMVECTOR VecRelativePos = DirectX::XMVectorSubtract(VecCurrentPos, PointVec);

	// 回転
	DirectX::XMVECTOR NewRelativePos = DirectX::XMVector3TransformCoord(VecRelativePos, R);
	DirectX::XMVECTOR NewPos = DirectX::XMVectorAdd(NewRelativePos, PointVec);

	DirectX::XMFLOAT3 Pos;
	DirectX::XMStoreFloat3(&Pos, NewPos);
	SetPosition(Pos);

	// 自分自身も回転
	Rotate(In_Axis.x * In_Angle, In_Axis.y * In_Angle, In_Axis.z * In_Angle);
}

void Transform::LookAt(_In_ const DirectX::XMFLOAT3 &In_Target, _In_ const DirectX::XMFLOAT3 &In_Up)
{
	auto Pos = GetPosition();
	DirectX::XMVECTOR VecPos = DirectX::XMLoadFloat3(&Pos);
	DirectX::XMVECTOR VecTargetVec = DirectX::XMLoadFloat3(&In_Target);
	DirectX::XMVECTOR VecUpVec = DirectX::XMLoadFloat3(&In_Up);

	// LookAt 行列を作成
	DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtLH(VecPos, VecTargetVec, VecUpVec);

	// 回転部分を抽出してクォータニオンに変換
	DirectX::XMVECTOR S, R, T;
	DirectX::XMMatrixDecompose(&S, &R, &T, lookAtMatrix);

	// 逆回転（LookAtLH は視点行列なので）
	R = DirectX::XMQuaternionInverse(R);

	DirectX::XMFLOAT4 NewQuat;
	DirectX::XMStoreFloat4(&NewQuat, R);

	SetQuat(NewQuat);
}

DirectX::XMFLOAT4X4 Transform::GetLocalMatrix(_In_ bool In_IsTranspose) const noexcept
{
	using namespace DirectX;

	// S * R * T
	XMMATRIX T = DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_Quat));
	XMMATRIX S = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMMATRIX M = S * R * T;

	if(In_IsTranspose)
		M = DirectX::XMMatrixTranspose(M);

	XMFLOAT4X4 fMat;
	XMStoreFloat4x4(&fMat, M);
	return fMat;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix(_In_ bool In_IsTranspose) const noexcept
{
	UpdateWorldMatrix();
	if(In_IsTranspose)
	{
		return m_TransposeWorldMatrix;
	}
	return m_WorldMatrix;
}

DirectX::XMFLOAT3 Transform::GetLocalRotation(_In_ bool In_IsDegree) noexcept
{
	// クォータニオンからオイラー角を取得
	m_Euler = SyncEulerFromQuat(m_Quat);

	if(!In_IsDegree)
		return ToRad(m_Euler);
	return m_Euler;
}

DirectX::XMFLOAT3 Transform::GetRotation(_In_ bool In_IsDegree) noexcept
{
	// クォータニオンからオイラー角を取得
	m_Euler = SyncEulerFromQuat(GetQuat());

	if(!In_IsDegree)
		return ToRad(m_Euler);
	return m_Euler;
}

DirectX::XMFLOAT3 Transform::GetPosition() const noexcept
{
	if(!m_pParent)
		return m_Pos;
	else
		return LocalToWorldPosition(m_Pos);
}

DirectX::XMFLOAT4 Transform::GetQuat() const noexcept
{
	if(!m_pParent)
		return m_Quat;
	else
		return LocalToWorldRotation(m_Quat);
}

DirectX::XMFLOAT3 Transform::GetScale() const noexcept
{
	if(!m_pParent)
		return m_Scale;
	else
		return m_Scale * m_pParent->GetScale();
}

DirectX::XMFLOAT3 Transform::GetFront(_In_ bool In_IsNormalize) const noexcept
{
	const DirectX::XMFLOAT4X4 &world = GetWorldMatrix(false);

	// 行列のZ軸(3列目)が前方ベクトル
	DirectX::XMFLOAT3 front(world._31, world._32, world._33);

	if(In_IsNormalize)
	{
		// スケールの影響を除去
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&front);
		v = DirectX::XMVector3Normalize(v);
		DirectX::XMStoreFloat3(&front, v);
	}

	return front;
}

DirectX::XMFLOAT3 Transform::GetRight(_In_ bool In_IsNormalize) const noexcept
{
	const DirectX::XMFLOAT4X4 &world = GetWorldMatrix(false);
	DirectX::XMFLOAT3 right(world._11, world._12, world._13);

	if(In_IsNormalize)
	{
		// スケールの影響を除去
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&right);
		v = DirectX::XMVector3Normalize(v);
		DirectX::XMStoreFloat3(&right, v);
	}

	return right;
}

DirectX::XMFLOAT3 Transform::GetUp(_In_ bool In_IsNormalize) const noexcept
{
	const DirectX::XMFLOAT4X4 &world = GetWorldMatrix(false);
	DirectX::XMFLOAT3 up(world._21, world._22, world._23);

	if(In_IsNormalize)
	{
		// スケールの影響を除去
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&up);
		v = DirectX::XMVector3Normalize(v);
		DirectX::XMStoreFloat3(&up, v);
	}

	return up;
}

void Transform::GetDirectionVectors(_Out_opt_ DirectX::XMFLOAT3 *Out_Front, _Out_opt_ DirectX::XMFLOAT3 *Out_Right, _Out_opt_ DirectX::XMFLOAT3 *Out_Up, _In_ bool In_IsNormalize) const noexcept
{
	const DirectX::XMFLOAT4X4 &world = GetWorldMatrix();

	if(Out_Front)
	{
		*Out_Front = DirectX::XMFLOAT3(world._31, world._32, world._33);
		if(In_IsNormalize)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(Out_Front);
			v = DirectX::XMVector3Normalize(v);
			DirectX::XMStoreFloat3(Out_Front, v);
		}
	}

	if(Out_Right)
	{
		*Out_Right = DirectX::XMFLOAT3(world._11, world._12, world._13);
		if(In_IsNormalize)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(Out_Right);
			v = DirectX::XMVector3Normalize(v);
			DirectX::XMStoreFloat3(Out_Right, v);
		}
	}

	if(Out_Up)
	{
		*Out_Up = DirectX::XMFLOAT3(world._21, world._22, world._23);
		if(In_IsNormalize)
		{
			DirectX::XMVECTOR v = DirectX::XMLoadFloat3(Out_Up);
			v = DirectX::XMVector3Normalize(v);
			DirectX::XMStoreFloat3(Out_Up, v);
		}
	}
}

void Transform::SetLocalPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	m_Pos = In_Pos;
	MarkDirty();
}

void Transform::SetLocalRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	// 回転を設定
	auto Rot = ToRad(In_Rotation);
	// クォータニオンに変換
	DirectX::XMStoreFloat4(&m_Quat, DirectX::XMQuaternionRotationRollPitchYaw(Rot.x, Rot.y, Rot.z));
	m_IsSyncEuler = true;
	MarkDirty();
}

void Transform::SetLocalScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	// 拡縮を設定
	m_Scale = In_Scale;
	MarkDirty();
}

void Transform::SetLocalQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	// クォータニオンを設定
	m_Quat = In_Quat;
	m_IsSyncEuler = true;
	MarkDirty();
}

void Transform::SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	if(!m_pParent)
		m_Pos = In_Pos;
	else
		m_Pos = WorldToLocalPosition(In_Pos);

	MarkDirty();
}

void Transform::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	// 回転を設定
	auto Rot = ToRad(In_Rotation);
	// クォータニオンに変換
	DirectX::XMFLOAT4 Quat;
	DirectX::XMStoreFloat4(&Quat, DirectX::XMQuaternionRotationRollPitchYaw(Rot.x, Rot.y, Rot.z));
	if(!m_pParent)
		m_Quat = Quat;
	else
		m_Quat = WorldToLocalRotation(Quat);

	m_IsSyncEuler = true;
	MarkDirty();
}

void Transform::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	if(!m_pParent)
		m_Scale = In_Scale;
	else
		m_Scale = In_Scale / m_pParent->GetScale();

	MarkDirty();
}

void Transform::SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	if(!m_pParent)
		m_Quat = In_Quat;
	else
		m_Quat = WorldToLocalRotation(In_Quat);

	m_IsSyncEuler = true;
	MarkDirty();
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
				MarkDirty();
			}
			else
			{
				if(m_IsSyncEuler)
				{
					m_Euler = SyncEulerFromQuat(m_Quat);
					m_IsSyncEuler = false;
				}
				pVec->x = m_Euler.x;
				pVec->y = m_Euler.y;
				pVec->z = m_Euler.z;
			}
		});
	group->CreateGroupItem<ItemBind>("Scale", DebugItem::Kind::Vector, &m_Scale);
}

DirectX::XMFLOAT3 Transform::SyncEulerFromQuat(_In_ DirectX::XMFLOAT4 In_Quat) noexcept
{
	std::vector<DirectX::XMFLOAT3> candidates = GetPracticalEulerCandidates(In_Quat);

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

std::vector<DirectX::XMFLOAT3> Transform::GetPracticalEulerCandidates(_In_ DirectX::XMFLOAT4 In_Quat) const noexcept
{
	std::vector<DirectX::XMFLOAT3> candidates;
	auto BaseEuler = DX11Math::QuaternionToRollPitchYaw(In_Quat);

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
	YawDegrees.push_back(DirectX::XMConvertToDegrees(In_Yaw));           // 元のYaw

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

void Transform::AddChild(_In_ Transform *In_Child)
{
	if(In_Child == nullptr)
		return;

	// 既に子リストにある場合は追加しない
	auto itr = std::find(m_Children.begin(), m_Children.end(), In_Child);
	if(itr == m_Children.end())
	{
		m_Children.push_back(In_Child);
	}
}

void Transform::RemoveChild(_In_ Transform *In_Child)
{
	if(In_Child == nullptr)
		return;

	auto itr = std::find(m_Children.begin(), m_Children.end(), In_Child);
	if(itr != m_Children.end())
	{
		m_Children.erase(itr);
	}
}

Transform *Transform::FindRecursive(_In_ Transform *In_Current, _In_ std::string_view In_Name) const
{
	if(In_Current == nullptr)
		return nullptr;

	// 自分の名前をチェック
	if(In_Current->GetGameObject()->GetName() == In_Name)
	{
		return In_Current;
	}

	// 子を再帰的に検索
	for(Transform *child : In_Current->m_Children)
	{
		Transform *found = FindRecursive(child, In_Name);
		if(found != nullptr)
		{
			return found;
		}
	}

	return nullptr;
}

Transform *Transform::GetRootRecursive(_In_ Transform *In_Current) const
{
	if(In_Current->m_pParent == nullptr)
		return In_Current;

	return GetRootRecursive(In_Current->m_pParent);
}

DirectX::XMFLOAT3 Transform::WorldToLocalPosition(_In_ const DirectX::XMFLOAT3 &In_WorldPos) const
{
	if(!m_pParent)
		return In_WorldPos;

	// 親のワールド行列の逆行列を取得
	DirectX::XMFLOAT4X4 fParentMat = m_pParent->GetWorldMatrix(false);
	DirectX::XMMATRIX ParentMat = DirectX::XMLoadFloat4x4(&fParentMat);
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX InvParentMat = DirectX::XMMatrixInverse(&det, ParentMat);

	// ワールド座標を親の逆行列で変換
	DirectX::XMVECTOR WorldPosVec = DirectX::XMLoadFloat3(&In_WorldPos);
	DirectX::XMVECTOR LocalPosVec = DirectX::XMVector3TransformCoord(WorldPosVec, InvParentMat);

	DirectX::XMFLOAT3 Pos;
	DirectX::XMStoreFloat3(&Pos, LocalPosVec);
	return Pos;
}

DirectX::XMFLOAT3 Transform::LocalToWorldPosition(_In_ const DirectX::XMFLOAT3 &In_LocalPos) const
{
	if(!m_pParent)
		return In_LocalPos;

	// 親のワールド行列を取得
	DirectX::XMFLOAT4X4 fParentMat = m_pParent->GetWorldMatrix(false);
	DirectX::XMMATRIX ParentMat = DirectX::XMLoadFloat4x4(&fParentMat);

	// ローカル座標を親のワールド行列で変換
	DirectX::XMVECTOR LocalPosVec = DirectX::XMLoadFloat3(&In_LocalPos);
	DirectX::XMVECTOR WorldPosVec = DirectX::XMVector3TransformCoord(LocalPosVec, ParentMat);

	DirectX::XMFLOAT3 Pos;
	DirectX::XMStoreFloat3(&Pos, WorldPosVec);
	return Pos;
}

DirectX::XMFLOAT4 Transform::WorldToLocalRotation(_In_ const DirectX::XMFLOAT4 &In_WorldQuat) const
{
	if(!m_pParent)
		return In_WorldQuat;

	// 親のワールド回転の逆クォータニオン
	DirectX::XMFLOAT4 ParentQuat = m_pParent->GetQuat();
	DirectX::XMVECTOR ParentWorldQuat = DirectX::XMLoadFloat4(&ParentQuat);
	DirectX::XMVECTOR InvParentQuat= DirectX::XMQuaternionInverse(ParentWorldQuat);

	// ワールド回転を親の逆回転で変換
	DirectX::XMVECTOR WorldQuatVec = DirectX::XMLoadFloat4(&In_WorldQuat);
	DirectX::XMVECTOR LocalQuatVec = DirectX::XMQuaternionMultiply(InvParentQuat, WorldQuatVec);

	DirectX::XMFLOAT4 Quat;
	DirectX::XMStoreFloat4(&Quat, LocalQuatVec);
	return Quat;
}

DirectX::XMFLOAT4 Transform::LocalToWorldRotation(_In_ const DirectX::XMFLOAT4 &In_LocalQuat) const
{
	if(!m_pParent)
		return In_LocalQuat;

	// 親のワールド回転
	DirectX::XMFLOAT4 ParentQuat = m_pParent->GetQuat();
	DirectX::XMVECTOR ParentWorldQuat = DirectX::XMLoadFloat4(&ParentQuat);

	// ローカル回転を親のワールド回転と合成
	DirectX::XMVECTOR LocalQuatVec = DirectX::XMLoadFloat4(&In_LocalQuat);
	DirectX::XMVECTOR WorldQuatVec = DirectX::XMQuaternionMultiply(ParentWorldQuat, LocalQuatVec);

	DirectX::XMFLOAT4 Quat;
	DirectX::XMStoreFloat4(&Quat, WorldQuatVec);
	return Quat;
}

void Transform::UpdateWorldMatrix() const noexcept
{
	// 各要素の行列を取得
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_Quat));
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	// 行列の合算
	DirectX::XMMATRIX M = S * R * T;

	if(m_pParent)
	{
		auto fParentMat = m_pParent->GetWorldMatrix(false);
		auto ParentM = DirectX::XMLoadFloat4x4(&fParentMat);
		M = M * ParentM;
	}

	// XMMATRIXからXMFLOATへ変換
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, M);
	// 転置したものも保存
	DirectX::XMStoreFloat4x4(&m_TransposeWorldMatrix, DirectX::XMMatrixTranspose(M));
}

void Transform::MarkDirty() noexcept
{
	if(m_WorldMatrixDirty)
		return;

	m_WorldMatrixDirty = true;

	// 子にも伝播
	PropagateTransformChanged();
}

void Transform::PropagateTransformChanged() noexcept
{
	for(Transform *child : m_Children)
	{
		child->MarkDirty();
	}
}
