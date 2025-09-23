/*+===================================================================
	File: GameObject.cpp
	Summary: GameObjectの基底クラス 佐々木先生のGameObjectを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:24:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "cstring"
#include "GameObject.hpp"

GameObject::GameObject(_In_ std::string In_Name)
	: m_Name(In_Name)
	, m_Pos{}, m_Quat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_Scale{ 1.0f, 1.0f, 1.0f }
	, m_Rotation{ 0.0f, 0.0f, 0.0f }
	, m_bIsChild(false), m_ParentPos{ 0.0f, 0.0f, 0.0f }, m_ParentQuat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_ParentScale{ 0.0f,0.0f,0.0f }
	, m_ParentRotation{ 0.0f, 0.0f, 0.0f }
	, m_PrevRotation{ 0.0f, 0.0f, 0.0f }
	, m_ChildPos{ 0.0f, 0.0f, 0.0f }, m_ChildRotation{ 0.0f, 0.0f, 0.0f }, m_ChildQuat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_ChildScale{ 0.0f, 0.0f, 0.0f }
	, m_pScene(nullptr), m_IsDestroySelf(false)
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

	// 子オブジェクトの削除
	for (auto &child : m_ChildObjects)
	{
		delete child.second; // 子オブジェクトの削除
	}
}

void GameObject::ExecuteUpdate() noexcept
{
	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		itr->ExecuteUpdate();
	}
	// 継承先オブジェクトの処理
	Update();

	// 子オブジェクトの処理
	for (auto &itr : m_ChildObjects)
	{
		itr.second->m_ParentPos = m_Pos; // 親の座標を保存
		itr.second->m_ParentQuat = m_Quat; // 親の回転を保存
		itr.second->m_ParentScale = m_Scale; // 親の拡縮を保存
		itr.second->UpdateChildTransform(); // 子オブジェクトの変換情報を更新
		itr.second->ExecuteUpdate();
	}

	// 角度データの同期
	AngleSynchronization();

	// 自身の破棄
	if (m_IsDestroySelf)
		_destroySelf();
}

void GameObject::ExecuteLateUpdate() noexcept
{
	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		itr->ExecuteLateUpdate();
	}
	// 継承先オブジェクトの遅延処理
	LateUpdate();

	// 子オブジェクトの処理
	for (auto &itr : m_ChildObjects)
	{
		itr.second->m_ParentPos = m_Pos; // 親の座標を保存
		itr.second->m_ParentQuat = m_Quat; // 親の回転を保存
		itr.second->m_ParentScale = m_Scale; // 親の拡縮を保存
		itr.second->UpdateChildTransform(); // 子オブジェクトの変換情報を更新
		itr.second->ExecuteLateUpdate();
	}

	// 角度データの同期
	AngleSynchronization();

	// 自身の破棄
	if (m_IsDestroySelf)
		_destroySelf();
}

void GameObject::ExecuteDraw() noexcept
{
	// コンポーネントの描画
	for (auto &itr : m_Components)
	{
		itr->ExecuteDraw();
	}
	// 継承先オブジェクトの描画
	Draw();

	// 子オブジェクトの描画
	for (auto &itr : m_ChildObjects)
	{
		itr.second->ExecuteDraw();
	}
}

std::map<std::string,GameObject *> GameObject::GetChildObjects() const noexcept
{
	return m_ChildObjects;
}

void GameObject::DestroyAllChildObjects() noexcept
{
	for (auto &child : m_ChildObjects)
	{
		delete child.second; // 子オブジェクトの削除
	}
	m_ChildObjects.clear();
}

void GameObject::DestroySelf() noexcept
{
	m_IsDestroySelf = true;
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
	// 転置
	if (In_IsTranspose)
		M = DirectX::XMMatrixTranspose(M);
	// XMMATRIXからXMFLOATへ変換
	DirectX::XMFLOAT4X4 fMat;
	DirectX::XMStoreFloat4x4(&fMat, M);

	return fMat;
}

void GameObject::SetPos(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	if(m_bIsChild)
		m_ChildPos = In_Pos; // 子オブジェクトの座標を保存(親からの相対座標)
	else
		m_Pos = In_Pos; // 座標を設定
}

void GameObject::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept
{
	if (m_bIsChild)
	{
		// 子オブジェクトの場合、親の回転を基準にする
		m_ChildRotation = ToRad(In_Rotation);
		// クォータニオンに変換
		DirectX::XMStoreFloat4(&m_ChildQuat, DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z));
	}
	else
	{
		// 回転を設定
		m_Rotation = ToRad(In_Rotation);
		m_PrevRotation = m_Rotation; // 前回の値を更新
		// クォータニオンに変換
		DirectX::XMStoreFloat4(&m_Quat,DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z));
	}
}

void GameObject::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	if (m_bIsChild)
	{
		// 子オブジェクトの場合、親の拡縮を基準にする
		m_ChildScale = In_Scale;
	}
	else
	{
		// 拡縮を設定
		m_Scale = In_Scale;
	}
}

void GameObject::SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept
{
	if (m_bIsChild)
	{
		// 子オブジェクトの場合、親のクォータニオンを基準にする
		m_ChildQuat = In_Quat;
		// オイラー角に変換
		m_ChildRotation = QuaternionToRollPitchYaw(m_Quat);
	}
	else
	{
		// クォータニオンを設定
		m_Quat = In_Quat;
		// オイラー角に変換
		m_Rotation = QuaternionToRollPitchYaw(m_Quat);
		m_PrevRotation = m_Rotation; // 前回の値を更新
	}
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

void GameObject::AngleSynchronization()
{
	// クォータニオンを正データとして扱う

	// オイラー角が前回の値から変化しているか
	if (m_PrevRotation != m_Rotation)
	{
		// 変化している場合は、クォータニオンに変換
		DirectX::XMStoreFloat4(&m_Quat, DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z));
		m_PrevRotation = m_Rotation; // 前回の値を更新
		return;
	}

	//// クォータニオンからオイラー角に変換
	//DirectX::XMFLOAT3 rot = QuaternionToRollPitchYaw(m_Quat);

	//if(m_Rotation != rot)
	//{
	//	// オイラー角が変化している場合、オイラー角を正データとして扱う
	//	m_Rotation = rot;
	//	m_PrevRotation = m_Rotation; // 前回の値を更新
	//}
}

void GameObject::UpdateChildTransform()
{
	// 親の変換情報を基準に子オブジェクトの変換情報を更新
	if (m_bIsChild)
	{
		// 座標
		m_Pos = m_ParentPos + m_ChildPos;
		// 回転
		m_Quat = QuaternionMultiply(m_ParentQuat, m_ChildQuat);
		m_Rotation = QuaternionToRollPitchYaw(m_Quat);
		m_PrevRotation = m_ChildRotation = m_Rotation; // 前回の値を更新
		// 拡縮
		m_Scale = m_ParentScale * m_ChildScale;
	}
}

void GameObject::_destroySelf() noexcept
{
	// 自身が所属しているシーンの削除予約リストに追加
	if (m_pScene)
		m_pScene->DestroyObj(m_Name); // シーンから自身を削除
}
