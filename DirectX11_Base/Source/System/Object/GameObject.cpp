/*+===================================================================
	File: GameObject.cpp
	Summary: GameObjectの基底クラス 佐々木先生のGameObjectを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:24:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GameObject.hpp"
#include "System/Component/Transform.hpp"
#include "System/Component/Collider/ColliderBase.hpp"

GameObject::GameObject(_In_ std::string In_Name)
	: m_Name(In_Name), m_ChildNameSaffix("(" + m_Name + "_Child)")
	, m_bIsChild(false)
	, m_pScene(nullptr), m_pParent(nullptr)
	, m_Data(nullptr)
	, m_pTransform(nullptr)
	//, m_Pos{}, m_Quat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_Scale{ 1.0f, 1.0f, 1.0f }
{
	m_Data = std::make_shared<cpon_object>();
	m_Data->SetObjectName(m_Name);
}

GameObject::~GameObject()
{
	m_Data = nullptr;
	auto itr = m_Components.begin();
	// コンポーネントの削除
	for (itr = m_Components.begin(); itr != m_Components.end();itr++)
	{
		delete (*itr);
		(*itr) = nullptr;
	}

	for (auto &child : m_ChildObjects)
	{
		child = nullptr;
	}
}

void GameObject::OnEnable() noexcept
{
	// コンポーネントの有効化処理
	for (auto &itr : m_Components)
	{
		itr->OnEnable();
	}
}

void GameObject::OnDisable() noexcept
{
	// コンポーネントの無効化処理
	for (auto &itr : m_Components)
	{
		itr->OnDisable();
		itr->SetActive(false);
	}
}

void GameObject::ExecuteInit() noexcept
{
	// 継承先オブジェクトの初期化
	Init();

	// コンポーネントの初期化
	InitializeComponents();

	m_IsInitialized = true;
}

void GameObject::ExecuteAwake() noexcept
{
	// Transformコンポーネント追加
	m_pTransform = AddComponent<Transform>();

	// 継承先オブジェクトのAwake処理
	Awake();
	// コンポーネントのAwake処理
	for (auto &itr : m_Components)
	{
		itr->Awake();
	}
}

void GameObject::ExecuteUpdate(_In_ float In_Tick) noexcept
{
	// 初期化が呼ばれていないコンポーネントの初期化
	InitializeComponents();

	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		if(itr->m_IsActive)
			itr->Update(In_Tick);
	}
	// 継承先オブジェクトの処理
	Update(In_Tick);
}

void GameObject::ExecuteLateUpdate(_In_ float In_Tick) noexcept
{
	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		if(itr->m_IsActive)
			itr->LateUpdate(In_Tick);
	}
	// 継承先オブジェクトの遅延処理
	LateUpdate(In_Tick);
}

void GameObject::ExecuteFixedUpdate(_In_ double In_FixedTick) noexcept
{
	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		if(itr->m_IsActive)
			itr->FixedUpdate(In_FixedTick);
	}
	// 継承先オブジェクトの固定間隔更新処理
	FixedUpdate(In_FixedTick);
}

void GameObject::OnDrawGizmos(_In_ Gizmos *In_Gizmos) noexcept
{
	// 継承先オブジェクトのギズモ描画処理
	DrawGizmos(In_Gizmos);
	// コンポーネントのギズモ描画処理
	for (auto &itr : m_Components)
	{
		itr->DrawGizmos(In_Gizmos);
	}
}

void GameObject::CallOnCollisionEnter(_In_ ColliderBase *In_Other) noexcept
{
	OnCollisionEnter(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnCollisionEnter(In_Other);
	}
}
void GameObject::CallOnCollisionStay(_In_ ColliderBase *In_Other) noexcept
{
	OnCollisionStay(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnCollisionStay(In_Other);
	}
}
void GameObject::CallOnCollisionExit(_In_ ColliderBase *In_Other) noexcept
{
	OnCollisionExit(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnCollisionExit(In_Other);
	}
}
void GameObject::CallOnTriggerEnter(_In_ ColliderBase *In_Other) noexcept
{
	OnTriggerEnter(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnTriggerEnter(In_Other);
	}
}
void GameObject::CallOnTriggerStay(_In_ ColliderBase *In_Other) noexcept
{
	OnTriggerStay(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnTriggerStay(In_Other);
	}
}
void GameObject::CallOnTriggerExit(_In_ ColliderBase *In_Other) noexcept
{
	OnTriggerExit(In_Other);
	for (auto &itr : m_Components)
	{
		itr->OnTriggerExit(In_Other);
	}
}

void GameObject::RemoveComponent(_In_ std::string In_Name)
{
	for (auto itr = m_Components.begin(); itr != m_Components.end(); ++itr)
	{
		if ((*itr)->m_Name == In_Name)
		{
			for(auto deadCmp : m_DeadComponents)
			{
				if (deadCmp == *itr)
					return;
			}
			m_DeadComponents.push_back(*itr);
		}
	}
}

std::vector<GameObject *> GameObject::GetChildObjects() const noexcept
{
	return m_ChildObjects;
}

void GameObject::DestroyAllChildObjects() noexcept
{
	for (auto &child : m_ChildObjects)
	{
		child->DestroySelf();
	}
	m_ChildObjects.clear();
}

void GameObject::DestroySelf() noexcept
{
	Object::DestroySelf();

	_destroySelf();

	for (const auto &itr : m_ChildObjects)
		itr->DestroySelf();
}

DirectX::XMFLOAT3 GameObject::GetRotation(_In_ bool In_IsDegree) const noexcept
{
	return m_pTransform->GetRotation(In_IsDegree);
}

DirectX::XMFLOAT3 GameObject::GetFront(_In_ const bool &Is_Normalize) const noexcept
{
	return m_pTransform->GetFront(Is_Normalize);
}

DirectX::XMFLOAT3 GameObject::GetRight() const noexcept
{
	return m_pTransform->GetRight();
}

DirectX::XMFLOAT3 GameObject::GetUp() const noexcept
{
	return m_pTransform->GetUp();
}

DirectX::XMFLOAT4X4 GameObject::GetWorld(_In_ bool In_IsTranspose) const noexcept
{
	auto Pos = m_pTransform->GetPosition();
	auto Quat = m_pTransform->GetQuat();
	auto Scale = m_pTransform->GetScale();

	// 各要素の行列を取得
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(
		DirectX::XMVectorSet(Quat.x, Quat.y, Quat.z, Quat.w)
	);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	// 行列の合算
	DirectX::XMMATRIX M = S * R * T;

	if (m_bIsChild && m_pParent)
	{
		auto ParPos = m_pParent->m_pTransform->GetPosition();
		auto ParQuat = m_pParent->m_pTransform->GetQuat();
		auto ParScale = m_pParent->m_pTransform->GetScale();
		T = DirectX::XMMatrixTranslation(ParPos.x, ParPos.y, ParPos.z);
		R = DirectX::XMMatrixRotationQuaternion(
			DirectX::XMVectorSet(ParQuat.x, ParQuat.y, ParQuat.z, ParQuat.w)
		);
		S = DirectX::XMMatrixScaling(ParScale.x, ParScale.y, ParScale.z);

		M = M * (S * R * T);
	}

	// 転置
	if (In_IsTranspose)
		M = DirectX::XMMatrixTranspose(M);
	// XMMATRIXからXMFLOATへ変換
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, M);

	return fMat;
}

void GameObject::SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	m_pTransform->SetPosition(In_Pos);
}

void GameObject::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	m_pTransform->SetRotation(In_Rotation);
}

void GameObject::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	m_pTransform->SetScale(In_Scale);
}

void GameObject::SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	m_pTransform->SetQuat(In_Quat);
}

DirectX::XMFLOAT3 GameObject::GetLeftTopFrontPosition() const noexcept
{
	auto Pos = m_pTransform->GetPosition();
	auto Scale = m_pTransform->GetScale();

	float Left = Pos.x - (Scale.x / 2.0f);
	float Top = Pos.y + (Scale.y / 2.0f);
	float Front = Pos.z - (Scale.z / 2.0f);
	return DirectX::XMFLOAT3(Left, Top, Front);
}

DirectX::XMFLOAT3 GameObject::GetRightBottomBackPosition() const noexcept
{
	auto Pos = m_pTransform->GetPosition();
	auto Scale = m_pTransform->GetScale();
	float Right = Pos.x + (Scale.x / 2.0f);
	float Bottom = Pos.y - (Scale.y / 2.0f);
	float Back = Pos.z + (Scale.z / 2.0f);
	return DirectX::XMFLOAT3(Right, Bottom, Back);
}

void GameObject::OnTriggerEnter(_In_ ColliderBase *In_Other) noexcept
{
	std::string msg = m_Name + " TriggerEnter with " + In_Other->GetGameObject()->GetName();
	DebugManager::GetInstance().DebugLog(msg.c_str());
}

void GameObject::OnTriggerStay(_In_ ColliderBase *In_Other) noexcept
{
	std::string  msg = m_Name + " TriggerStay with " + In_Other->GetGameObject()->GetName();
	DebugManager::GetInstance().DebugLog(msg.c_str());
}

void GameObject::OnTriggerExit(_In_ ColliderBase *In_Other) noexcept
{
	std::string msg = m_Name + " TriggerExit with " + In_Other->GetGameObject()->GetName();
	DebugManager::GetInstance().DebugLog(msg.c_str());
}


void GameObject::InitializeComponents() noexcept
{
	for (auto &itr : m_InitComponents)
	{
		itr->Init();
		itr->m_IsInitialized = true;
	}
	m_InitComponents.clear();
}

void GameObject::_addComponent(_In_ Component *In_pComponent)
{
	// 所持オブジェクトの登録
	In_pComponent->m_pGameObject = this;

	if(!m_Data)
		return;

	// 保存されている情報を設定
	In_pComponent->DataRead(m_Data);
	// Awake処理を実行
	In_pComponent->Awake();
}

void GameObject::_destroySelf() noexcept
{
	// 自身が所属しているシーンの削除予約リストに追加
	if (m_pScene)
		m_pScene->DestroyObj(m_Name); // シーンから自身を削除
}

void GameObject::ExecuteDestroyComponents() noexcept
{
	for (auto &itr : m_DeadComponents)
	{
		auto compItr = std::find(m_Components.begin(), m_Components.end(), itr);
		if (compItr != m_Components.end())
		{
			delete *compItr;
			m_Components.erase(compItr);
		}
	}
	m_DeadComponents.clear();
}

void GameObject::DataWrite(_In_ cpon *In_pCpon)
{
	// 保存データのクリア
	m_Data->ClearData();

	auto block = m_Data->CreateDataBlock();
	/*block->CreateArray<float>("Position", { m_Pos.x, m_Pos.y, m_Pos.z });
	block->CreateArray<float>("Quaternion", { m_Quat.x, m_Quat.y, m_Quat.z, m_Quat.w });
	block->CreateArray<float>("Scale", { m_Scale.x, m_Scale.y, m_Scale.z });*/

	for (auto &itr : m_Components)
	{
		itr->DataWrite(block);
	}

	In_pCpon->AddObject(m_Data);
}

void GameObject::DataRead(_In_ std::shared_ptr<cpon_object> In_pCponObj)
{
	if(!In_pCponObj || In_pCponObj->IsEmpty())
		return;

	*m_Data = *In_pCponObj;

	/*auto block = (*In_pCponObj)[0];
	auto PosArray = block->GetArrayPtr<float>("Position");
	auto QuatArray = block->GetArrayPtr<float>("Quaternion");
	auto ScaleArray = block->GetArrayPtr<float>("Scale");

	if (!PosArray || !QuatArray || !ScaleArray)
		return;

	m_Pos = DirectX::XMFLOAT3((*PosArray)[0], (*PosArray)[1], (*PosArray)[2]);
	m_Quat = DirectX::XMFLOAT4((*QuatArray)[0], (*QuatArray)[1], (*QuatArray)[2], (*QuatArray)[3]);
	m_Scale = DirectX::XMFLOAT3((*ScaleArray)[0], (*ScaleArray)[1], (*ScaleArray)[2]);*/
}

#ifdef _DEBUG
void GameObject::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	//// トランスフォームグループの作成
	//ItemGroup *group = In_pWindow->CreateItem<ItemGroup>("Transform");
	//group->CreateGroupItem<ItemBind>("Pos", DebugItem::Kind::Vector, &m_Pos);
	//group->CreateGroupItem<ItemCallback>("Rotation", DebugItem::Kind::Vector,
	//	[this](bool IsWrite, void *arg) {
	//		DirectX::XMFLOAT3 *pVec = static_cast<DirectX::XMFLOAT3 *>(arg);
	//		if (IsWrite)
	//		{
	//			DirectX::XMStoreFloat4(&m_Quat,
	//				DirectX::XMQuaternionRotationRollPitchYaw( // zxy
	//					DirectX::XMConvertToRadians(pVec->x), // pitch
	//					DirectX::XMConvertToRadians(pVec->y), // yaw
	//					DirectX::XMConvertToRadians(pVec->z))); // roll
	//		}
	//		else
	//		{
	//			DirectX::XMFLOAT3 rot = DX11Math::QuaternionToRollPitchYaw(m_Quat);
	//			pVec->x = DirectX::XMConvertToDegrees(rot.x);
	//			pVec->y = DirectX::XMConvertToDegrees(rot.y);
	//			pVec->z = DirectX::XMConvertToDegrees(rot.z);
	//		}
	//	});
	//group->CreateGroupItem<ItemBind>("Scale", DebugItem::Kind::Vector, &m_Scale);
	In_pWindow->CreateItem<ItemBind>("IsActive", DebugItem::Kind::Bool, &m_IsActive);

	// 継承先オブジェクトのインスペクター登録
	RegisterObjectDebugInspector(In_pWindow);

	// コンポーネントのインスペクター登録
	for (auto &itr : m_Components)
	{
		itr->RegisterDebugInspector(In_pWindow);
	}
}
#endif
