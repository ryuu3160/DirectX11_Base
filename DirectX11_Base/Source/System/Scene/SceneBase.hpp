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
// ==============================
//  undefine
// ==============================
#undef GetObject

class SceneObjectBase
{
public:
	~SceneObjectBase() = default;
	void *m_pObject;
	bool m_bIsGameObject;	// GameObjectかどうか
};

template<class T>
class SceneObject : public SceneObjectBase
{
public:
	SceneObject(T *ptr)
	{
		m_pObj = ptr;
		// TがGameObjectの派生クラスかどうかを判定
		isGameObject = std::is_base_of<GameObject, T>();
	}
	virtual ~SceneObject() { delete static_cast<T *>(m_pObj); }
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
	SceneBase(_In_ const std::string_view &In_Name);
	virtual ~SceneBase();

	static void Initialize() noexcept;

	void RootUpdate() noexcept;
	void RootDraw() noexcept;

	// サブシーン操作
	template <class T = SceneBase>
	T* AddSubScene();
	void RemoveSubScene();

	// オブジェクト操作
	template <class T = GameObject>
	T *CreateObject(_In_ const std::string_view &In_Name);

	template<>
	GameObject *CreateObject(_In_ const std::string_view &In_Name);
	
	template <class T = GameObject>
	T *GetObject(_In_ const std::string_view &In_Name);

	void DestroyObj(_In_ const std::string_view &In_Name);


	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	void Setup(_In_ const std::string_view *In_ShaderFiles, _In_ int In_ShaderNum, _In_ int In_ModelNum);

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
inline T *SceneBase::AddSubScene()
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
inline T *SceneBase::CreateObject(_In_ const std::string_view &In_Name)
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator it = m_objects.find(In_Name);
	if (it != m_objects.end())
	{
		std::string buf = "Failed to create object.";
		buf += In_Name;
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
inline T *SceneBase::GetObject(_In_ const std::string_view &In_Name)
{
	// オブジェクトの探索
	Objects::iterator it = m_Objects.find(In_Name);
	if (it == m_Objects.end()) return nullptr;

	// 型変換
	T *ptr = reinterpret_cast<T *>(it->second->m_pObject);
	return ptr;
}
