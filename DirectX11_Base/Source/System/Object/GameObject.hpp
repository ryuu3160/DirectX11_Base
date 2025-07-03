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
