/*+===================================================================
	File: SceneBase.hpp
	Summary: シーンのベースクラス
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 04:54:50 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/GameObject.hpp"
#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/System/ModelRenderer.hpp"
// ==============================
//  undefine
// ==============================
#undef GetObject

class SceneObjectBase
{
public:
	virtual ~SceneObjectBase() {};
	void *m_pObject = nullptr;
	bool m_bIsGameObject = false;	// GameObjectかどうか
};

template<class T>
class SceneObject : public SceneObjectBase
{
public:
	SceneObject(T *ptr)
	{
		m_pObject = ptr;
		// TがGameObjectの派生クラスかどうかを判定
		m_bIsGameObject = std::is_base_of<GameObject, T>();
	}
	virtual ~SceneObject() { delete static_cast<T *>(m_pObject); }
};

/// <summary>
/// SceneBaseクラス
/// </summary>
class SceneBase
{
private:
	using Objects = std::map<std::string, SceneObjectBase *>;
	using Items = std::list<std::string>;
public:
	static void Initialize() noexcept;

	SceneBase(_In_ const std::string &In_Name);
	virtual ~SceneBase();

	void RootUpdate() noexcept;
	void RootDraw() noexcept;

	// サブシーン操作
	template <class T = SceneBase>
	T* AddSubScene();
	void RemoveSubScene();

	// オブジェクト操作
	template <class T>
	T *CreateObject(_In_ const std::string &In_Name);

	template<>
	GameObject *CreateObject(_In_ const std::string &In_Name);
	
	template <class T>
	T *GetObject(_In_ const std::string_view &In_Name);

	void DestroyObj(const std::string &In_Name);

	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	void Setup(const char **In_ShaderFiles, int In_ShaderNum, int In_ModelNum);

private:
	static Objects m_Objects;
	std::string m_Name;

protected:
	SceneBase *m_pParent;		// 親シーン
	SceneBase *m_pSubScene;		// サブシーンへのポインタ
	Items m_Items;
};

/// <summary>
/// サブシーンの追加
/// </summary>
/// <typeparam name="T">サブシーンの型</typeparam>
/// <returns>生成したサブシーン</returns>
template<class T>
T *SceneBase::AddSubScene()
{
	RemoveSubScene();
	T *pScene = new T;
	m_pSubScene = pScene;
	pScene->m_pParent = this;
	pScene->Init();
	return pScene;
}

/// <summary>
/// オブジェクトの生成
/// </summary>
/// <typeparam name="T">オブジェクトの型</typeparam>
/// <param name="[In_szName]">オブジェクトの名称</param>
/// <returns>生成したオブジェクト</returns>
template<class T>
T *SceneBase::CreateObject(_In_ const std::string &In_Name)
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator it = m_Objects.find(In_Name);
	if (it != m_Objects.end())
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
	m_Objects.insert(std::pair<std::string, SceneObjectBase *>(In_Name, new SceneObject<T>(ptr)));
	m_Items.push_back(In_Name);
	return ptr;
}

template<class T>
T *SceneBase::GetObject(_In_ const std::string_view &In_Name)
{
	// オブジェクトの探索
	Objects::iterator it = m_Objects.find(In_Name.data());
	if (it == m_Objects.end()) return nullptr;

	// 型変換
	T *ptr = reinterpret_cast<T *>(it->second->m_pObject);
	return ptr;
}
