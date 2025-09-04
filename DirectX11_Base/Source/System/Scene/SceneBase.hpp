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
#include "DirectX11/System/SpriteRenderer.hpp"

// ==============================
//  前方宣言
// ==============================
class RenderManager;

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

	/// <summary>
	/// シーンの名前を指定して、SceneBase オブジェクトを初期化します。
	/// </summary>
	/// <param name="[In_Name]">シーンの名前を表す文字列。</param>
	SceneBase(_In_ const std::string &In_Name) noexcept;
	virtual ~SceneBase();

	/// <summary>
	/// 大本のシーンの更新と描画を行います。
	/// </summary>
	void RootUpdate() noexcept;
	void RootDraw() noexcept;

	// サブシーン操作

	/// <summary>
	/// サブシーンを追加し、そのポインタを返します。
	/// </summary>
	/// <typeparam name="[T]">追加するサブシーンの型。デフォルトは SceneBase です。</typeparam>
	/// <returns>追加されたサブシーンのポインタ。</returns>
	template <class T = SceneBase>
	T* AddSubScene() noexcept;
	/// <summary>
	/// サブシーンを削除します。
	/// </summary>
	void RemoveSubScene() noexcept;

	/// <summary>
	/// サブシーンへのポインタを取得します。
	/// </summary>
	/// <returns>サブシーンを指す SceneBase 型のポインタ。</returns>
	SceneBase *GetSubScene() noexcept { return m_pSubScene; }

	// オブジェクト操作

	/// <summary>
	/// 指定された名前でオブジェクトを作成します。
	/// </summary>
	/// <typeparam name="[T]">作成するオブジェクトの型。</typeparam>
	/// <param name="[In_Name]">作成するオブジェクトの名前を表す文字列。</param>
	/// <returns>作成されたオブジェクトへのポインタ。</returns>
	template <class T>
	T *CreateObject(_In_ const std::string &In_Name) noexcept;

	template <typename T, typename ...Args>
	T *CreateObject(_In_ const std::string &In_Name, Args&&... args) noexcept;

	/// <summary>
	/// 指定された名前でGameObjectを作成します。
	/// </summary>
	/// <param name="[In_Name]">作成するGameObjectの名前。</param>
	/// <returns>新しく作成されたGameObjectへのポインタ。</returns>
	template<>
	GameObject *CreateObject(_In_ const std::string &In_Name) noexcept;
	
	/// <summary>
	/// 指定された名前に対応するオブジェクトへのポインタを取得します。
	/// </summary>
	/// <typeparam name="[T]">取得するオブジェクトの型。</typeparam>
	/// <param name="[In_Name]">検索するオブジェクトの名前を表す文字列ビュー。</param>
	/// <returns>見つかった場合はオブジェクトへのポインタ、見つからない場合はnullptrを返します。</returns>
	template <class T>
	T *GetObject(_In_ const std::string_view &In_Name) noexcept;

	/// <summary>
	/// 指定された名前のオブジェクトを破棄します。
	/// </summary>
	/// <param name="[In_Name]">破棄するオブジェクトの名前。</param>
	void DestroyObj(_In_ const std::string &In_Name) noexcept;

	/// <summary>
	/// シーンを取得する関数
	/// </summary>
	/// <returns>このシーンのポインタを返します。</returns>
	inline SceneBase *GetScene() noexcept { return this; }

	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() {};
	virtual void Draw() = 0;

protected:
	/// <summary>
	/// モデル数を指定してセットアップを行います。
	/// </summary>
	/// <param name="[In_ModelNum]">モデルの数への参照。</param>
	void Setup(_In_ const int &In_ModelNum) noexcept;

private:
	/// <summary>
	/// ルートのメイン更新処理を実行します。
	/// </summary>
	void _RootUpdateMain() noexcept;
	/// <summary>
	/// ルートオブジェクトの遅延更新処理を実行します。
	/// </summary>
	void _RootUpdateLate() noexcept;

private:
	static Objects m_Objects;
	std::string m_Name;

	RenderManager &m_RenderManager; // レンダリングマネージャーのインスタンス

protected:
	SceneBase *m_pParent;		// 親シーン
	SceneBase *m_pSubScene;		// サブシーンへのポインタ
	Items m_Items;
};

/// <summary>
/// サブシーンの追加
/// </summary>
/// <typeparam name="[T]">サブシーンの型</typeparam>
/// <returns>生成したサブシーン</returns>
template<class T>
T *SceneBase::AddSubScene() noexcept
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
/// <typeparam name="[T]">オブジェクトの型</typeparam>
/// <param name="[In_szName]">オブジェクトの名称</param>
/// <returns>生成したオブジェクト</returns>
template<class T>
T *SceneBase::CreateObject(_In_ const std::string &In_Name) noexcept
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

template<typename T, typename ...Args>
inline T *SceneBase::CreateObject(const std::string &In_Name, Args && ...args) noexcept
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
	T *ptr = new T(args...);
	m_Objects.insert(std::pair<std::string, SceneObjectBase *>(In_Name, new SceneObject<T>(ptr)));
	m_Items.push_back(In_Name);
	return ptr;
}

/// <summary>
/// 指定された名前のオブジェクトを検索し、指定した型へのポインタとして返します。
/// </summary>
/// <typeparam name="[T]">取得するオブジェクトの型。</typeparam>
/// <param name="[In_Name]">検索するオブジェクトの名前。</param>
/// <returns>見つかった場合は指定した型Tのポインタ、見つからなかった場合はnullptrを返します。</returns>
template<class T>
T *SceneBase::GetObject(_In_ const std::string_view &In_Name) noexcept
{
	// オブジェクトの探索
	Objects::iterator it = m_Objects.find(In_Name.data());
	if (it == m_Objects.end()) return nullptr;

	// 型変換
	T *ptr = reinterpret_cast<T *>(it->second->m_pObject);
	return ptr;
}
