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
	: m_Name(In_Name)
	, m_Pos{}, m_Quat{ 0.0f, 0.0f, 0.0f, 1.0f }, m_Scale{ 1.0f, 1.0f, 1.0f }
	, m_Rotation{ 0.0f, 0.0f, 0.0f }
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
		std::memcpy(&m_Pos, ptr, sizeof(m_Pos));
		std::memcpy(&m_Quat, ptr += sizeof(m_Pos), sizeof(m_Quat));
		std::memcpy(&m_Scale, ptr += sizeof(m_Quat), sizeof(m_Scale));
		ptr += sizeof(m_Scale);
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
}

void GameObject::Execute()
{
	// コンポーネントの処理
	for (auto &itr : m_Components)
	{
		itr->Execute();
	}
	// 継承先オブジェクトの処理
	Update();
}

std::map<std::string,GameObject *> GameObject::GetChildObjects() const noexcept
{
	return m_ChildObjects;
}

DirectX::XMFLOAT3 GameObject::GetFront() const noexcept
{
	// 前方ベクトルを取得
	DirectX::XMVECTOR vFront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// クォータニオンを使って回転
	DirectX::XMVECTOR qRotate = DirectX::XMLoadFloat4(&m_Quat);
	vFront = DirectX::XMVector3Rotate(vFront, qRotate);
	DirectX::XMFLOAT3 dir;
	// 正規化してXMFLOAT3に変換
	DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(vFront));
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
