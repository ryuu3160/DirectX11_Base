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
#include "System/Object/Object.hpp"
#include "System/Scene/SceneBase.hpp"
// ==============================
//  前方宣言
// ==============================
class Component;
class DebugWindow;

/// <summary>
/// GameObjectクラス
/// </summary>
class GameObject : public Object
{
	friend class SceneBase;
private:
	// 子オブジェクトリスト
	using ChildObjects = std::vector<GameObject *>;
public:
	GameObject(_In_ std::string In_Name);
	virtual ~GameObject();

	virtual void OnEnable() noexcept override;
	virtual void OnDisable() noexcept override;

	void ExecuteInit() noexcept;

	/// <summary>
	/// 更新処理
	/// </summary>
	void ExecuteUpdate(_In_ float In_Tick) noexcept;

	/// <summary>
	/// 遅延更新処理
	/// </summary>
	void ExecuteLateUpdate(_In_ float In_Tick) noexcept;

	/// <summary>
	/// 固定間隔更新処理
	/// </summary>
	/// <param name="In_FixedTick"></param>
	void ExecuteFixedUpdate(_In_ double In_FixedTick) noexcept;

	/// <summary>
	/// ギズモを描画するために呼び出されます
	/// </summary>
	void OnDrawGizmos(_In_ Gizmos* In_Gizmos) noexcept;

	// 当たり判定用
	void CallOnCollisionEnter(_In_ ColliderBase *In_Other) noexcept;
	void CallOnCollisionStay(_In_ ColliderBase *In_Other) noexcept;
	void CallOnCollisionExit(_In_ ColliderBase *In_Other) noexcept;
	void CallOnTriggerEnter(_In_ ColliderBase *In_Other) noexcept;
	void CallOnTriggerStay(_In_ ColliderBase *In_Other) noexcept;
	void CallOnTriggerExit(_In_ ColliderBase *In_Other) noexcept;

	// コンポーネントの生成
	template<typename T>
	requires std::derived_from<T, Component>
	T *AddComponent();
	// コンポーネントの生成(引数付き)
	template<typename T, typename ...Args>
	requires std::derived_from<T, Component>
	T *AddComponent(_In_ Args&&... args);

	// コンポーネントの取得
	template<typename T>
	requires std::derived_from<T, Component>
	T *GetComponent();

	template <typename T>
	requires std::derived_from<T, Component>
	void RemoveComponent();

	void RemoveComponent(_In_ std::string In_Name);

	// 子オブジェクトを生成、追加する
	template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type* = nullptr>
	T *AddChildObject(_In_ const std::string &In_Name);

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type * = nullptr>
	T *AddChildObject(_In_ const std::string &In_Name, Args&&... args);

	template<>
	GameObject *AddChildObject(_In_ const std::string &In_Name);

	// 指定した名前の子オブジェクトを取得する
	template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type* = nullptr>
	T *GetChildObject(_In_ const std::string &In_Name);

	/// <summary>
	/// 子オブジェクトのポインタのリストを取得します。
	/// </summary>
	/// <returns>子オブジェクトへのポインタを格納した std::map を返します。</returns>
	ChildObjects GetChildObjects() const noexcept;

	template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type * = nullptr>
	void DestroyChildObject(_In_ const std::string &In_Name);

	/// <summary>
	/// すべての子オブジェクトを破棄します。
	/// </summary>
	void DestroyAllChildObjects() noexcept;

	/// <summary>
	/// 自身を所属しているシーンから破棄します。
	/// </summary>
	void DestroySelf() noexcept override final;

	/// <summary>
	/// 自身が所属しているシーンを取得します。
	/// </summary>
	/// <returns>所属しているシーンへの静的ポインタ。</returns>
	inline SceneBase *GetScene() const noexcept { return m_pScene; }

	inline const std::string &GetName() const noexcept { return m_Name; }
	DirectX::XMFLOAT3 GetRotation() const noexcept;

	inline DirectX::XMFLOAT3 GetPosition() const noexcept { return m_Pos; }
	inline DirectX::XMFLOAT4 GetQuat() const noexcept { return m_Quat; }
	inline DirectX::XMFLOAT3 GetScale() const noexcept { return m_Scale; }
	inline const bool &IsChild() const noexcept { return m_bIsChild; }

	DirectX::XMFLOAT3 GetFront(_In_ const bool &Is_Normalize = true) const noexcept;
	DirectX::XMFLOAT3 GetRight() const noexcept;
	DirectX::XMFLOAT3 GetUp() const noexcept;
	DirectX::XMFLOAT4X4 GetWorld(_In_ bool In_IsTranspose = true) const noexcept;

	void SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;
	void SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept;
	void SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept;
	void SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept;

	/// <summary>
	/// オブジェクトの左下前方の位置を返します
	/// </summary>
	/// <returns>左下前方の位置を表すDirectX::XMFLOAT3構造体。</returns>
	DirectX::XMFLOAT3 GetLeftTopFrontPosition() const noexcept;
	/// <summary>
	/// オブジェクトの右上奥の位置を返します
	/// </summary>
	/// <returns>右上奥の位置を表す DirectX::XMFLOAT3。</returns>
	DirectX::XMFLOAT3 GetRightBottomBackPosition() const noexcept;

protected:
	// 継承先で使用する関数
	virtual void Init() noexcept override {}
	virtual void Update(_In_ float In_Tick) noexcept override {}
	virtual void LateUpdate(_In_ float In_Tick) noexcept override {}
	virtual void FixedUpdate(_In_ double In_FixedTick) noexcept override {}

	virtual void OnCollisionEnter(_In_ ColliderBase *In_Other) noexcept override {}
	virtual void OnCollisionStay(_In_ ColliderBase *In_Other) noexcept override {}
	virtual void OnCollisionExit(_In_ ColliderBase *In_Other) noexcept override {}
	virtual void OnTriggerEnter(_In_ ColliderBase *In_Other) noexcept override;
	virtual void OnTriggerStay(_In_ ColliderBase *In_Other) noexcept override;
	virtual void OnTriggerExit(_In_ ColliderBase *In_Other) noexcept override;

#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override final;
	virtual void RegisterObjectDebugInspector(_In_ DebugWindow *In_pWindow) {};
#endif // _DEBUG

private:
	void InitializeComponents() noexcept;

	// コンポーネント追加時に型に関係なく呼び出す処理
	void _addComponent(_In_ Component *In_pComponent);

	// 自身をシーンから破棄
	void _destroySelf() noexcept;

	void ExecuteDestroyComponents() noexcept;

	void DataWrite(_In_ cpon *In_pCpon);
	void DataRead(_In_ std::shared_ptr<cpon_object> In_pCponObj);

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
	Components			m_Components;		// コンポーネントの一覧
	Components			m_InitComponents;	// 初期化を呼び出すコンポーネントリスト
	Components			m_DeadComponents;	// 破棄予定のコンポーネントリスト
	ChildObjects		m_ChildObjects;		// 子オブジェクトの一覧
	std::shared_ptr<cpon_object> 		m_Data;				// 保存データ
	Datas				m_Datas;			// 保存データ
	std::string			m_Name;				// オブジェクト名
	std::string			m_ChildNameSaffix;	// 子オブジェクト名のサフィックス
	SceneBase			*m_pScene;			// 所属しているシーンへのポインタ
	GameObject			*m_pParent;			// 親オブジェクトへのポインタ
protected:
	DirectX::XMFLOAT3	m_Pos;		// 座標
	DirectX::XMFLOAT3	m_Scale;	// 拡縮
	DirectX::XMFLOAT4	m_Quat;		// 回転(クォータニオン)
	bool				m_bIsChild; // 子オブジェクトかどうか
};

template<typename T>
requires std::derived_from<T, Component>
inline T *GameObject::AddComponent()
{
	// コンポーネント生成
	T *ptr = new T;
	// 型に関係ない初期化処理を実施
	_addComponent(ptr);
	// 管理リストに追加
	m_Components.push_back(ptr);
	m_InitComponents.push_back(ptr);

	return ptr;
}

template<typename T, typename ...Args>
requires std::derived_from<T, Component>
inline T *GameObject::AddComponent(_In_ Args && ...args)
{
	// コンポーネント生成
	T *ptr = new T(args...);
	// 型に関係ない初期化処理を実施
	_addComponent(ptr);
	// 管理リストに追加
	m_Components.push_back(ptr);
	m_InitComponents.push_back(ptr);

	return ptr;
}

/// <summary>
/// 指定した型のコンポーネントを取得します。
/// </summary>
/// <typeparam name="[T]">取得したいコンポーネントの型。</typeparam>
/// <returns>指定した型Tのコンポーネントへのポインタ。該当するコンポーネントが存在しない場合はnullptrを返します。</returns>
template<typename T>
requires std::derived_from<T, Component>
inline T *GameObject::GetComponent()
{
	for (auto itr = m_Components.begin(); itr != m_Components.end();++itr)
	{
		// 型チェック
		if (typeid(T) == typeid(**itr))
		{
			return dynamic_cast<T *>(*itr);
		}
	}
	return nullptr;
}

template<typename T>
requires std::derived_from<T, Component>
inline void GameObject::RemoveComponent()
{
	for (auto itr = m_Components.begin(); itr != m_Components.end(); ++itr)
	{
		// 型チェック
		if (typeid(T) == typeid(**itr))
		{
			(*itr)->DestroySelf();
		}
	}
}

/// <summary>
/// <para>指定した型の子オブジェクトを生成し、追加します。</para>
/// <para>指定する型は、必ずGameObjectクラスを継承していなければなりません。</para>
/// </summary>
/// <param name="[In_Name]">オブジェクト名</param>
/// <returns>生成、追加したオブジェクトへのポインタを返す</returns>
template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type*>
inline T *GameObject::AddChildObject(_In_ const std::string &In_Name)
{
	std::string Name = In_Name + m_ChildNameSaffix;
	auto Child = m_pScene->CreateObject<T>(Name);

	if (!Child)
		return nullptr;

	dynamic_cast<GameObject *>(Child)->m_bIsChild = true; // 子オブジェクトフラグを立てる
	dynamic_cast<GameObject *>(Child)->m_pParent = this; // 親オブジェクトを設定
	m_ChildObjects.push_back(Child);
	return Child;
}

template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type *>
inline T *GameObject::AddChildObject(_In_ const std::string &In_Name, Args && ...args)
{
	std::string Name = In_Name + m_ChildNameSaffix;
	auto Child = m_pScene->CreateObject<T>(Name, args);

	if (!Child)
		return nullptr;

	dynamic_cast<GameObject *>(Child)->m_bIsChild = true; // 子オブジェクトフラグを立てる
	dynamic_cast<GameObject *>(Child)->m_pParent = this; // 親オブジェクトを設定
	m_ChildObjects.push_back(Child);
	return Child;
}

template<>
inline GameObject *GameObject::AddChildObject(_In_ const std::string &In_Name)
{
	std::string Name = In_Name + m_ChildNameSaffix;
	auto Child = m_pScene->CreateObject<GameObject>(Name);

	if (!Child)
		return nullptr;

	Child->m_bIsChild = true;
	Child->m_pParent = this;
	m_ChildObjects.push_back(Child);
	return Child;
}

/// <summary>
/// <para>指定した名前の子オブジェクトを取得します。</para>
/// <para>指定する型は、必ずGameObjectクラスを継承していなければなりません。</para>
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="enable_if"></typeparam>
/// <param name="In_Name"></param>
/// <returns></returns>
template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type*>
inline T *GameObject::GetChildObject(_In_ const std::string &In_Name)
{
	auto Name = In_Name + m_ChildNameSaffix;
	for (auto &itr : m_ChildObjects)
	{
		if (itr->m_Name == Name)
		{
			// 型チェック
			if (std::is_same_v<T,decltype(*itr)> == false)
			{
				std::string buf = "Failed to get object." + In_Name;
				MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
				return nullptr;
			}
			return dynamic_cast<T *>(itr);
		}
	}
	return nullptr;
}

template<typename T, typename std::enable_if<std::is_base_of<GameObject, T>::value>::type *>
inline void GameObject::DestroyChildObject(_In_ const std::string &In_Name)
{
	auto itr = GetChildObject<T>(In_Name);

	if (!itr)
		return;

	dynamic_cast<GameObject *>(itr)->DestroySelf();
	m_ChildObjects.erase(std::remove(m_ChildObjects.begin(), m_ChildObjects.end(), itr), m_ChildObjects.end());
}
