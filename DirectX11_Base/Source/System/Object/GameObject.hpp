/*+===================================================================
	File: GameObject.hpp
	Summary: GameObjectの基底クラス 佐々木先生のGameObjectを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:24:28 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//  前方宣言
// ==============================
class Component;

/// <summary>
/// GameObjectクラス
/// </summary>
class GameObject
{
private:
	// 子オブジェクトリスト
	using ChildObjects = std::map<std::string, GameObject *>;
public:
	GameObject(_In_ std::string In_Name);
	virtual ~GameObject();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Execute();

	// コンポーネントの生成
	template<class T>
	T *AddComponent();
	// コンポーネントの取得
	template<class T>
	T *GetComponent();

	// 子オブジェクトを生成、追加する
	template<typename T,typename std::enable_if<std::is_base_of<GameObject,T>::value>>
	T *AddChildObject(_In_ const std::string &In_Name);

	// 指定した名前の子オブジェクトを取得する
	template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>>
	T *GetChildObject(_In_ const std::string &In_Name);

	/// <summary>
	/// 子オブジェクトのポインタのリストを取得します。
	/// </summary>
	/// <returns>子オブジェクトへのポインタを格納した std::map を返します。</returns>
	ChildObjects GetChildObjects() const noexcept;

	inline void SetPos(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept { m_Pos = In_Pos; }
	inline DirectX::XMFLOAT3 GetPos() const noexcept { return m_Pos; }
	DirectX::XMFLOAT3 GetFront() const noexcept;
	DirectX::XMFLOAT3 GetRight() const noexcept;
	DirectX::XMFLOAT3 GetUp() const noexcept;
	DirectX::XMFLOAT4X4 GetWorld(_In_ bool In_IsTranspose = true) const noexcept;

protected:
	// 継承先のクラスでオブジェクト別の処理を実装する場合、上書きすること。
	virtual void Update() {}

private:
	// コンポーネント追加時に型に関係なく呼び出す処理
	void _addComponent(_In_ Component *In_pComponent);

private:
	// コンポーネントリスト
	using Components = std::vector<Component *>;

	// データの保存
	struct SaveData
	{
		const char *name;	// 保存する値の名称
		char *value;		// 保存値
	};
	using Datas = std::vector<SaveData>;

private:
	Components			m_Components;	// コンポーネントの一覧
	ChildObjects		m_ChildObjects;	// 子オブジェクトの一覧
	Datas				m_Datas;		// 保存データ
	std::string			m_Name;			// オブジェクト名
	DirectX::XMFLOAT3	m_Rotation;		// 回転
protected:
	DirectX::XMFLOAT3	m_Pos;		// 座標
	DirectX::XMFLOAT4	m_Quat;		// 回転
	DirectX::XMFLOAT3	m_Scale;	// 拡縮
};

template<class T>
inline T *GameObject::AddComponent()
{
#ifdef _DEBUG
	// デバッグ時のみ、指定された型がComponentを継承しているか確認
	static_assert(std::is_base_of<Component, T>(),
		"[GameObject::GetComponent] template T does not inherit from 'Component'");
#endif
	// コンポーネント生成
	T *ptr = new T;
	// 型に関係ない初期化処理を実施
	_addComponent(ptr);
	// 管理リストに追加
	m_Components.push_back(ptr);

	return ptr;
}

/// <summary>
/// 指定した型のコンポーネントを取得します。
/// </summary>
/// <typeparam name="[T]">取得したいコンポーネントの型。</typeparam>
/// <returns>指定した型Tのコンポーネントへのポインタ。該当するコンポーネントが存在しない場合はnullptrを返します。</returns>
template<class T>
inline T *GameObject::GetComponent()
{
	T *ptr = nullptr;
	for (auto itr = m_Components.begin(); itr != m_Components.end();itr++)
	{
		// 型チェック
		if (typeid(T) == typeid(**itr))
		{
			ptr = reinterpret_cast<T *>(*itr);
			break;
		}
	}
	return ptr;
}

/// <summary>
/// <para>指定した型の子オブジェクトを生成し、追加します。</para>
/// <para>指定する型は、必ずGameObjectクラスを継承していなければなりません。</para>
/// </summary>
/// <param name="[In_Name]">オブジェクト名</param>
/// <returns>生成、追加したオブジェクトへのポインタを返す</returns>
template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>>
inline T *GameObject::AddChildObject(const std::string &In_Name)
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	ChildObjects::iterator itr = m_ChildObjects.find(In_Name);
	if (itr != m_ChildObjects.end())
	{
		std::string buf = "Failed to create object." + In_Name;
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}

	// ヒエラルキーに追加
	//hierarchy->AddListItem(name);

#endif // _DEBUG

	// オブジェクト生成
	T *ptr = new T();
	m_ChildObjects.insert(std::pair<std::string, GameObject *>(In_Name, ptr));
	return ptr;
}

/// <summary>
/// <para>指定した名前の子オブジェクトを取得します。</para>
/// <para>指定する型は、必ずGameObjectクラスを継承していなければなりません。</para>
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="enable_if"></typeparam>
/// <param name="In_Name"></param>
/// <returns></returns>
template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>>
inline T *GameObject::GetChildObject(const std::string &In_Name)
{
	for (auto &itr : m_ChildObjects)
	{
		if (itr.first == In_Name)
		{
			// 型チェック
			if (typeid(T) != typeid(*itr.second))
			{
				std::string buf = "Failed to get object." + In_Name;
				MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
				return nullptr;
			}
			return reinterpret_cast<T *>(itr.second);
		}
	}

	return nullptr;
}
