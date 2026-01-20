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
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Component/Transform.hpp"
#include "Core/DirectX11/Resource/ShaderResource/Shader.hpp"
#include "Core/DirectX11/Renderer/ModelRenderer.hpp"
#include "Core/DirectX11/Renderer/SpriteRenderer.hpp"
#include "Core/System/Managers/DebugManager/DebugItem.hpp"
#include "ryuu_lib/CRC31_HashGenerator/HashGenerator.hpp"
#include "Core/System/CPON/cpon.hpp"
#include "Core/System/Utility/Utility.hpp"
// ==============================
//  undefine
// ==============================
#undef GetObject

// ==============================
//  前方宣言
// ==============================
class SceneManager;
class ItemHierarchy;

/// <summary>
/// SceneBaseクラス
/// </summary>
class SceneBase
{
private:
	friend class SceneManager; // シーンマネージャーをフレンドクラスに登録
	using Objects = std::unordered_map<std::string, GameObject *, HashGenerator,StringEq>;
	using Items = std::list<std::string>;
public:

	/// <summary>
	/// シーンの名前を指定して、SceneBase オブジェクトを初期化します
	/// </summary>
	/// <param name="[In_Name]">シーンの名前を表す文字列</param>
	SceneBase(_In_ std::string_view In_Name) noexcept;
	virtual ~SceneBase();

	// オブジェクト操作

	template <typename T>
	requires std::derived_from<T, GameObject>
	T *CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent = nullptr) noexcept;

	template<typename T, typename ...Args>
	requires std::derived_from<T, GameObject>
	T *CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent = nullptr, Args&&... args) noexcept;

	template <typename T, typename ...Args>
	requires std::derived_from<T, GameObject>
	T *CreateObject_NotAddHierarchy(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent = nullptr, Args&&... args) noexcept;

	/// <summary>
	/// 指定された名前でGameObjectを作成します
	/// </summary>
	/// <param name="[In_Name]">作成するGameObjectの名前</param>
	/// <returns>新しく作成されたGameObjectへのポインタ</returns>
	template<>
	GameObject *CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent) noexcept;

	/// <summary>
	/// 指定された名前に対応するオブジェクトへのポインタを取得します
	/// </summary>
	/// <typeparam name="[T]">取得するオブジェクトの型</typeparam>
	/// <param name="[In_Name]">検索するオブジェクトの名前を表す文字列ビュー</param>
	/// <returns>見つかった場合はオブジェクトへのポインタ、見つからない場合はnullptrを返します</returns>
	template <class T>
	T *GetObject(_In_ std::string_view In_Name) noexcept;

	/// <summary>
	/// オブジェクト名を変更します
	/// </summary>
	/// <param name="[In_OldName]">古い名前</param>
	/// <param name="[In_NewName]">新しい名前</param>
	void RenameObj(_In_ std::string_view In_OldName, _In_ std::string_view In_NewName);

	/// <summary>
	/// 指定された名前のオブジェクトを破棄します
	/// </summary>
	/// <param name="[In_Name]">破棄するオブジェクトの名前</param>
	void DestroyObj(_In_ std::string In_Name) noexcept;

	/// <summary>
	/// シーンを取得する関数
	/// </summary>
	/// <returns>このシーンのポインタを返します</returns>
	inline SceneBase *GetScene() noexcept { return this; }

	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update(_In_ float In_DeltaTime) = 0;
	virtual void LateUpdate(_In_ float In_DeltaTime) {};
	virtual void FixedUpdate(_In_ double In_FixedDeltaTime) {};
	virtual void Draw() = 0;

protected:
	/// <summary>
	/// モデル数を指定してセットアップを行います
	/// </summary>
	/// <param name="[In_ModelNum]">モデルの数への参照</param>
	void Setup(_In_ const int &In_ModelNum) noexcept;

private:

	/// <summary>
	/// メインカメラの作成など、シーン全体の初期化処理を行います
	/// </summary>
	void CommonProcessScene() noexcept;

	/// <summary>
	/// シーン内の、まだ初期化されていないオブジェクトの初期化を行います
	/// </summary>
	void _ObjectsInit() noexcept;

	/// <summary>
	/// ルートのメイン更新処理を実行します
	/// </summary>
	/// <param name="[In_DeltaTime]">経過時間（デルタタイム）を示す入力値</param>
	void _RootUpdateMain(_In_ float In_DeltaTime) noexcept;
	/// <summary>
	/// ルートオブジェクトの遅延更新処理を実行します
	/// </summary>
	/// <param name="[In_DeltaTime]">経過時間（デルタタイム）を示す入力値</param>
	void _RootUpdateLate(_In_ float In_DeltaTime) noexcept;
	/// <summary>
	/// ルートの固定更新処理を実行します
	/// </summary>
	/// <param name="[In_FixedDeltaTime]">固定タイックの時間（固定更新間隔）を示す入力値固定ステップの継続時間</param>
	void _RootFixedUpdate(_In_ double In_FixedDeltaTime) noexcept;

	/// <summary>
	/// オブジェクトのコンポーネント削除予約リストを処理する
	/// </summary>
	void _ExecuteDestroyObjectsComponents() noexcept;

	/// <summary>
	/// シーンの描画を行います
	/// </summary>
	void _RootDraw() noexcept;

	/// <summary>
	/// 削除予約リストに追加されたオブジェクトを破棄します
	/// </summary>
	void _DestroyObjects() noexcept;

	void DataSave();
	void DataLoad();

private:
	static Objects m_Objects;
#ifdef _DEBUG
	static std::vector<GameObject *> m_ShowHierarchyObjects;
	static ItemHierarchy *m_Hierarchy;
#endif
	std::vector<GameObject *> m_InitObjects; // Initializeを呼び出すオブジェクトリスト
	std::string m_Name;
	cpon* m_Data; // シーンデータ

public:
#ifdef _DEBUG
	/// <summary>
	/// デバッグ用: ヒエラルキーに表示するオブジェクトリストを取得します
	/// </summary>
	/// <returns>ヒエラルキーに表示するオブジェクトリストへの参照を返します</returns>
	static std::vector<GameObject *> &GetHierarchyObjects() noexcept { return m_ShowHierarchyObjects; }
#endif

protected:
	Items m_Items;
	Items m_DeadItems; // 破棄するオブジェクトの名前リスト
	std::unordered_set<GameObject *> m_SceneObjects; // シーン内のオブジェクトリスト
	SceneManager &m_SceneManager;
};

template <typename T>
requires std::derived_from<T, GameObject>
T *SceneBase::CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent) noexcept
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator itr = m_Objects.find(In_Name.data());
	if (itr != m_Objects.end())
	{
		std::string buf = "Failed to create object.";
		buf += In_Name.data();
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}
#endif

	// オブジェクト生成
	T *ptr = new T(In_Name.data());
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name.data())); // CPONデータ読み込み
	ptr->ExecuteAwake(); // Awake呼び出し

	if(In_pParent)
		ptr->GetTransform()->SetParent(In_pParent); // 親設定

	m_Objects.insert(std::pair<std::string, T*>(In_Name.data(), ptr));
	m_Items.push_back(In_Name.data());
	m_SceneObjects.emplace(ptr);
	m_InitObjects.push_back(ptr);

#ifdef _DEBUG
	// ヒエラルキーに追加
	m_ShowHierarchyObjects.push_back(ptr);
#endif

	return ptr;
}

template<typename T, typename ...Args>
requires std::derived_from<T, GameObject>
inline T *SceneBase::CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent, Args && ...args) noexcept
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator itr = m_Objects.find(In_Name.data());
	if(itr != m_Objects.end())
	{
		std::string buf = "Failed to create object.";
		buf += In_Name.data();
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}
#endif

	// オブジェクト生成
	T *ptr = new T(In_Name.data(), args...);
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name.data())); // CPONデータ読み込み
	ptr->ExecuteAwake(); // Awake呼び出し

	if(In_pParent)
		ptr->GetTransform()->SetParent(In_pParent); // 親設定
	m_Objects.insert(std::pair<std::string, T *>(In_Name.data(), ptr));
	m_Items.push_back(In_Name.data());
	m_SceneObjects.emplace(ptr);
	m_InitObjects.push_back(ptr);

#ifdef _DEBUG
	// ヒエラルキーに追加
	m_ShowHierarchyObjects.push_back(ptr);
#endif

	return ptr;
}

template<typename T, typename ...Args>
requires std::derived_from<T, GameObject>
inline T *SceneBase::CreateObject_NotAddHierarchy(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent, Args && ...args) noexcept
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator itr = m_Objects.find(In_Name.data());
	if(itr != m_Objects.end())
	{
		std::string buf = "Failed to create object.";
		buf += In_Name.data();
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}

#endif // _DEBUG

	// オブジェクト生成
	T *ptr = new T(In_Name.data(), args...);
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name.data())); // CPONデータ読み込み
	ptr->ExecuteAwake(); // Awake呼び出し

	if(In_pParent)
		ptr->GetTransform()->SetParent(In_pParent); // 親設定

	m_Objects.insert(std::pair<std::string, T *>(In_Name.data(), ptr));
	m_Items.push_back(In_Name.data());
	m_SceneObjects.emplace(ptr);
	m_InitObjects.push_back(ptr);
	return ptr;
}

/// <summary>
/// 指定された名前のオブジェクトを検索し、指定した型へのポインタとして返します
/// </summary>
/// <typeparam name="[T]">取得するオブジェクトの型</typeparam>
/// <param name="[In_Name]">検索するオブジェクトの名前</param>
/// <returns>見つかった場合は指定した型Tのポインタ、見つからなかった場合はnullptrを返します</returns>
template<class T>
T *SceneBase::GetObject(_In_ std::string_view In_Name) noexcept
{
	// オブジェクトの探索
	Objects::iterator itr = m_Objects.find(In_Name.data());
	if (itr == m_Objects.end())
		return nullptr;

	// 型変換
	T *ptr = dynamic_cast<T *>(itr->second);

	if(NullCheck(ptr, NullpCheckMode::OUTPUT, "Failed to get object." + std::string(In_Name)))
		return nullptr;

	// 破棄予約されていた場合は取得不可
	if(static_cast<GameObject *>(ptr)->IsDestroySelf())
		return nullptr;

	return ptr;
}
