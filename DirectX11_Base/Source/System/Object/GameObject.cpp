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
{
	// オブジェクト名に応じて、保存ファイルの読み込み
    std::string pathStr = "Assets/GameObject/" + m_Name + ".dat";
    FilePath path = pathStr;
	std::fstream file;
	file.open(path.data(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		// ファイルが開けた場合、データを読み込む
		file.seekg(0, std::ios::end);
		long fileSize = static_cast<long>(file.tellg());
		file.seekg(0, std::ios::beg);
		char *ptr = new char[fileSize];
		file.read(ptr, fileSize); // ファイルの内容を一括で読み込む
		m_Datas.push_back({ "data", ptr }); // 一括で読み込んだデータを保存
		file.close();

		// ゲームオブジェクト内のデータの読み込み
        // ファイルサイズが十分かどうかをチェックしてからmemcpyを実行
		if (fileSize >= sizeof(m_Pos) + sizeof(m_Quat) + sizeof(m_Scale))
		{
			std::memcpy(&m_Pos, ptr, sizeof(m_Pos));
			std::memcpy(&m_Quat, ptr + sizeof(m_Pos), sizeof(m_Quat));
			std::memcpy(&m_Scale, ptr + sizeof(m_Pos) + sizeof(m_Quat), sizeof(m_Scale));
			ptr += sizeof(m_Pos) + sizeof(m_Quat) + sizeof(m_Scale);
		}
		size_t size;
		// データのキーと値が保存されている個所へのポインタを取得
		for (;ptr - m_Datas[0].value < fileSize;ptr += size)
		{
			char *data[2]; // キー,値
			for (int i = 0; i < 2; ++i)
			{
				// データサイズ
				size = *reinterpret_cast<size_t *>(ptr);
				ptr += sizeof(size);
				// データ
				data[i] = ptr;
				ptr += size;
			}
			m_Datas.push_back({ data[0], data[1] });
		}
	}
}

GameObject::~GameObject()
{
	// 保存データの削除
	if (!m_Datas.empty())
		delete[] m_Datas[0].value;

	auto itr = m_Components.begin();
#ifdef _DEBUG
	// データの保存
	std::string pathStr = "Assets/GameObject/" + m_Name + ".dat";
    FilePath path = pathStr;
	std::fstream file;
	file.open(path.data(), std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		// ゲームオブジェクトのデータを保存
		file.write(reinterpret_cast<const char *>(&m_Pos), sizeof(m_Pos));
		file.write(reinterpret_cast<const char *>(&m_Quat), sizeof(m_Quat));
		file.write(reinterpret_cast<const char *>(&m_Scale), sizeof(m_Scale));

		// コンポーネントのデータを保存
		for (itr = m_Components.begin();itr != m_Components.end();itr++)
		{
			const char *name = typeid(**itr).name();
			Component::DataAccessor accessor(nullptr);
			(*itr)->ReadWrite(&accessor);
			// データのキーを保存
			size_t size = strlen(name);
			file.write(reinterpret_cast<const char *>(&size), sizeof(size));
			file.write(name, size);
			// データの保存
			size = accessor.GetWriteSize();
			file.write(reinterpret_cast<const char *>(&size), sizeof(size));
			file.write(accessor.GetData(), size);
		}
		file.close();
	}
#endif

	// コンポーネントの削除
	for (itr = m_Components.begin(); itr != m_Components.end();itr++)
	{
		delete (*itr);
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

	// 破棄予約されたコンポーネントの破棄処理
	ExecuteDestroyComponents();
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
	return QuaternionToRollPitchYaw(m_Quat);
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
	DirectX::XMMATRIX T;
	DirectX::XMMATRIX R;
	DirectX::XMMATRIX S;

	auto Pos = m_Pos;
	auto Quat = m_Quat;
	auto Scale = m_Scale;

	if (m_bIsChild && m_pParent)
	{
		Pos = Pos + m_pParent->m_Pos;
		Quat = QuaternionMultiply(Quat, m_pParent->m_Quat);
		Scale = Scale * m_pParent->m_Scale;
	}

	T = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	R = DirectX::XMMatrixRotationQuaternion(
		DirectX::XMVectorSet(Quat.x, Quat.y, Quat.z, Quat.w)
	);
	S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);

	// 行列の合算
	DirectX::XMMATRIX M = S * R * T;
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
	Component::DataAccessor accessor(it->value);
	In_pComponent->ReadWrite(&accessor);
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

void GameObject::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	// トランスフォームグループの作成
	ItemGroup *group = In_pWindow->CreateItem<ItemGroup>("Transform");
	group->CreateGroupItem<ItemBind>("Pos", DebugItem::Kind::Vector, &m_Pos);
	group->CreateGroupItem<ItemBind>("Rotation", DebugItem::Kind::Vector,
		[this](bool IsWrite, void *arg)
		{
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
				DirectX::XMFLOAT3 rot = QuaternionToRollPitchYaw(m_Quat);
				pVec->x = DirectX::XMConvertToDegrees(rot.x);
				pVec->y = DirectX::XMConvertToDegrees(rot.y);
				pVec->z = DirectX::XMConvertToDegrees(rot.z);
			}
		});
	group->CreateGroupItem<ItemBind>("Scale", DebugItem::Kind::Vector, &m_Scale);

	// コンポーネントのインスペクター登録
	/*auto it = m_components.begin();
	while (it != m_components.end())
	{
		(*it)->Debug(window);
		++it;
	}*/
}
