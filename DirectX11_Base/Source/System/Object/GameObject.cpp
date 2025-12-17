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

GameObject::GameObject(_In_ std::string In_Name)
	: m_Name(In_Name), m_ChildNameSaffix("(" + m_Name + "_Child)")
	, m_Pos{}, m_Quat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_Scale{ 1.0f, 1.0f, 1.0f }
	, m_bIsChild(false)
	, m_pScene(nullptr), m_IsDestroySelf(false), m_pParent(nullptr)
	, m_Data(nullptr)
{
	m_Data = std::make_shared<cpon_object>();
	m_Data->SetObjectName(m_Name);
	//// オブジェクト名に応じて、保存ファイルの読み込み
 //   std::string pathStr = "Assets/GameObject/" + m_Name + ".dat";
 //   FilePath path = pathStr;
	//std::fstream file;
	//file.open(path.data(), std::ios::in | std::ios::binary);
	//if (file.is_open())
	//{
	//	// ファイルが開けた場合、データを読み込む
	//	file.seekg(0, std::ios::end);
	//	long fileSize = static_cast<long>(file.tellg());
	//	file.seekg(0, std::ios::beg);
	//	char *ptr = new char[fileSize];
	//	file.read(ptr, fileSize); // ファイルの内容を一括で読み込む
	//	m_Datas.push_back({ "data", ptr }); // 一括で読み込んだデータを保存
	//	file.close();

	//	// ゲームオブジェクト内のデータの読み込み
 //       // ファイルサイズが十分かどうかをチェックしてからmemcpyを実行
	//	if (fileSize >= sizeof(m_Pos) + sizeof(m_Quat) + sizeof(m_Scale))
	//	{
	//		std::memcpy(&m_Pos, ptr, sizeof(m_Pos));
	//		std::memcpy(&m_Quat, ptr + sizeof(m_Pos), sizeof(m_Quat));
	//		std::memcpy(&m_Scale, ptr + sizeof(m_Pos) + sizeof(m_Quat), sizeof(m_Scale));
	//		ptr += sizeof(m_Pos) + sizeof(m_Quat) + sizeof(m_Scale);
	//	}
	//	size_t size = 0;
	//	// データのキーと値が保存されている個所へのポインタを取得
	//	for (;ptr - m_Datas[0].value < fileSize;ptr += size)
	//	{
	//		char *data[2]; // キー,値
	//		for (int i = 0; i < 2; ++i)
	//		{
	//			// データサイズ
	//			size = *reinterpret_cast<size_t *>(ptr);
	//			ptr += sizeof(size);
	//			// データ
	//			data[i] = ptr;
	//			ptr += size;
	//		}
	//		m_Datas.push_back({ data[0], data[1] });
	//	}
	//}
}

GameObject::~GameObject()
{
	m_Data = nullptr;
	auto itr = m_Components.begin();
//	// 保存データの削除
//	if (!m_Datas.empty())
//		delete[] m_Datas[0].value;
//
//#ifdef _DEBUG
//	// データの保存
//	std::string pathStr = "Assets/GameObject/" + m_Name + ".dat";
//    FilePath path = pathStr;
//	std::fstream file;
//	file.open(path.data(), std::ios::out | std::ios::binary);
//	if (file.is_open())
//	{
//		// ゲームオブジェクトのデータを保存
//		file.write(reinterpret_cast<const char *>(&m_Pos), sizeof(m_Pos));
//		file.write(reinterpret_cast<const char *>(&m_Quat), sizeof(m_Quat));
//		file.write(reinterpret_cast<const char *>(&m_Scale), sizeof(m_Scale));
//
//		// コンポーネントのデータを保存
//		for (itr = m_Components.begin();itr != m_Components.end();itr++)
//		{
//			const char *name = typeid(**itr).name();
//			Component::DataAccessor accessor(nullptr);
//			(*itr)->ReadWrite(&accessor);
//			// データのキーを保存
//			size_t size = strlen(name);
//			file.write(reinterpret_cast<const char *>(&size), sizeof(size));
//			file.write(name, size);
//			// データの保存
//			size = accessor.GetWriteSize();
//			file.write(reinterpret_cast<const char *>(&size), sizeof(size));
//			file.write(accessor.GetData(), size);
//		}
//		file.close();
//	}
//#endif

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
	m_IsDestroySelf = true;

	_destroySelf();

	for (const auto &itr : m_ChildObjects)
		itr->DestroySelf();
}

DirectX::XMFLOAT3 GameObject::GetRotation() const noexcept
{
	return DX11Math::QuaternionToRollPitchYaw(m_Quat);
}

DirectX::XMFLOAT3 GameObject::GetFront(_In_ const bool &Is_Normalize) const noexcept
{
	// 前方ベクトルを取得
	DirectX::XMVECTOR vFront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// クォータニオンを使って回転
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	vFront = DirectX::XMVector3Rotate(vFront, qRotate);
	DirectX::XMFLOAT3 dir;
	// 正規化してXMFLOAT3に変換
	if (Is_Normalize)
		DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vFront));
	else
		DirectX::XMStoreFloat3(&dir, vFront);
	return dir;
}

DirectX::XMFLOAT3 GameObject::GetRight() const noexcept
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

DirectX::XMFLOAT3 GameObject::GetUp() const noexcept
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

DirectX::XMFLOAT4X4 GameObject::GetWorld(_In_ bool In_IsTranspose) const noexcept
{
	// 各要素の行列を取得
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(
		DirectX::XMVectorSet(m_Quat.x, m_Quat.y, m_Quat.z, m_Quat.w)
	);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	// 行列の合算
	DirectX::XMMATRIX M = S * R * T;

	if (m_bIsChild && m_pParent)
	{
		T = DirectX::XMMatrixTranslation(m_pParent->m_Pos.x, m_pParent->m_Pos.y, m_pParent->m_Pos.z);
		R = DirectX::XMMatrixRotationQuaternion(
			DirectX::XMVectorSet(m_pParent->m_Quat.x, m_pParent->m_Quat.y, m_pParent->m_Quat.z, m_pParent->m_Quat.w)
		);
		S = DirectX::XMMatrixScaling(m_pParent->m_Scale.x, m_pParent->m_Scale.y, m_pParent->m_Scale.z);

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
	m_Pos = In_Pos;
}

void GameObject::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	// 回転を設定
	auto Rot = ToRad(In_Rotation);
	// クォータニオンに変換
	DirectX::XMStoreFloat4(&m_Quat,DirectX::XMQuaternionRotationRollPitchYaw(Rot.x, Rot.y, Rot.z));
}

void GameObject::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	// 拡縮を設定
	m_Scale = In_Scale;
}

void GameObject::SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	// クォータニオンを設定
	m_Quat = In_Quat;
}

DirectX::XMFLOAT3 GameObject::GetLeftTopFrontPosition() const noexcept
{
	float Left = m_Pos.x - (m_Scale.x / 2.0f);
	float Top = m_Pos.y + (m_Scale.y / 2.0f);
	float Front = m_Pos.z - (m_Scale.z / 2.0f);
	return DirectX::XMFLOAT3(Left, Top, Front);
}

DirectX::XMFLOAT3 GameObject::GetRightBottomBackPosition() const noexcept
{
	float Right = m_Pos.x + (m_Scale.x / 2.0f);
	float Bottom = m_Pos.y - (m_Scale.y / 2.0f);
	float Back = m_Pos.z + (m_Scale.z / 2.0f);
	return DirectX::XMFLOAT3(Right, Bottom, Back);
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
	In_pComponent->m_pTransform = this;

	// 保存データに一致するコンポーネントがあるか探索
	const char *name = typeid(*In_pComponent).name();
	auto it = std::find_if(m_Datas.begin(), m_Datas.end(), [&name](SaveData &data)
		{
			// コンポーネントの名前と保存データのキーが一致するか確認
			return strstr(data.name, name) == data.name;
		});
	if (it == m_Datas.end()) return;

	// 保存されている情報を設定
	In_pComponent->DataRead(m_Data);
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
	block->CreateArray<float>("Position", { m_Pos.x, m_Pos.y, m_Pos.z });
	block->CreateArray<float>("Quaternion", { m_Quat.x, m_Quat.y, m_Quat.z, m_Quat.w });
	block->CreateArray<float>("Scale", { m_Scale.x, m_Scale.y, m_Scale.z });

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

	auto block = (*In_pCponObj)[0];
	auto PosArray = block->GetArrayPtr<float>("Position");
	auto QuatArray = block->GetArrayPtr<float>("Quaternion");
	auto ScaleArray = block->GetArrayPtr<float>("Scale");

	if (!PosArray || !QuatArray || !ScaleArray)
		return;

	m_Pos = DirectX::XMFLOAT3((*PosArray)[0], (*PosArray)[1], (*PosArray)[2]);
	m_Quat = DirectX::XMFLOAT4((*QuatArray)[0], (*QuatArray)[1], (*QuatArray)[2], (*QuatArray)[3]);
	m_Scale = DirectX::XMFLOAT3((*ScaleArray)[0], (*ScaleArray)[1], (*ScaleArray)[2]);
}

#ifdef _DEBUG
void GameObject::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	// トランスフォームグループの作成
	ItemGroup *group = In_pWindow->CreateItem<ItemGroup>("Transform");
	group->CreateGroupItem<ItemBind>("Pos", DebugItem::Kind::Vector, &m_Pos);
	group->CreateGroupItem<ItemCallback>("Rotation", DebugItem::Kind::Vector,
		[this](bool IsWrite, void *arg) {
			DirectX::XMFLOAT3 *pVec = static_cast<DirectX::XMFLOAT3 *>(arg);
			if (IsWrite)
			{
				DirectX::XMStoreFloat4(&m_Quat,
					DirectX::XMQuaternionRotationRollPitchYaw( // zxy
						DirectX::XMConvertToRadians(pVec->x), // pitch
						DirectX::XMConvertToRadians(pVec->y), // yaw
						DirectX::XMConvertToRadians(pVec->z))); // roll
			}
			else
			{
				DirectX::XMFLOAT3 rot = DX11Math::QuaternionToRollPitchYaw(m_Quat);
				pVec->x = DirectX::XMConvertToDegrees(rot.x);
				pVec->y = DirectX::XMConvertToDegrees(rot.y);
				pVec->z = DirectX::XMConvertToDegrees(rot.z);
			}
		});
	group->CreateGroupItem<ItemBind>("Scale", DebugItem::Kind::Vector, &m_Scale);
	In_pWindow->CreateItem<ItemBind>("IsActive", DebugItem::Kind::Bool, &m_IsActive);

	// コンポーネントのインスペクター登録
	for (auto &itr : m_Components)
	{
		itr->RegisterDebugInspector(In_pWindow);
	}
}
#endif
