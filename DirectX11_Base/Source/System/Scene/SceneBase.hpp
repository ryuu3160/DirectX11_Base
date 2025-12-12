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
#include "DirectX11/Resource/Shaders/Shader.hpp"
#include "DirectX11/Renderer/ModelRenderer.hpp"
#include "DirectX11/Renderer/SpriteRenderer.hpp"
#include "System/DebugManager/DebugItem.hpp"

// ==============================
//  undefine
// ==============================
#undef GetObject

// ==============================
//  前方宣言
// ==============================
class SceneManager;

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
	/// シーンの名前を指定して、SceneBase オブジェクトを初期化します。
	/// </summary>
	/// <param name="[In_Name]">シーンの名前を表す文字列。</param>
	SceneBase(_In_ const std::string &In_Name) noexcept;
	virtual ~SceneBase();

	// オブジェクト操作

	template <typename T>
	requires std::derived_from<T, GameObject>
	T *CreateObject(_In_ const std::string &In_Name) noexcept;

	template <typename T, typename ...Args>
	requires std::derived_from<T, GameObject>
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
	void DestroyObj(_In_ std::string In_Name) noexcept;

	/// <summary>
	/// シーンを取得する関数
	/// </summary>
	/// <returns>このシーンのポインタを返します。</returns>
	inline SceneBase *GetScene() noexcept { return this; }

	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update(_In_ float In_Tick) = 0;
	virtual void LateUpdate(_In_ float In_Tick) {};
	virtual void FixedUpdate(_In_ double In_FixedTick) {};
	virtual void Draw() = 0;

protected:
	/// <summary>
	/// モデル数を指定してセットアップを行います。
	/// </summary>
	/// <param name="[In_ModelNum]">モデルの数への参照。</param>
	void Setup(_In_ const int &In_ModelNum) noexcept;

private:

	/// <summary>
	/// メインカメラの作成など、シーン全体の初期化処理を行います。
	/// </summary>
	void CommonProcessScene() noexcept;

	/// <summary>
	/// シーン内の、まだ初期化されていないオブジェクトの初期化を行います。
	/// </summary>
	void _ObjectsInit() noexcept;

	/// <summary>
	/// ルートのメイン更新処理を実行します。
	/// </summary>
	/// <param name="[In_Tick]">経過時間（デルタタイム）を示す入力値。</param>
	void _RootUpdateMain(_In_ float In_Tick) noexcept;
	/// <summary>
	/// ルートオブジェクトの遅延更新処理を実行します。
	/// </summary>
	/// <param name="[In_Tick]">経過時間（デルタタイム）を示す入力値。</param>
	void _RootUpdateLate(_In_ float In_Tick) noexcept;
	/// <summary>
	/// ルートの固定更新処理を実行します。
	/// </summary>
	/// <param name="[In_FixedTick]">固定タイックの時間（固定更新間隔）を示す入力値。固定ステップの継続時間</param>
	void _RootFixedUpdate(_In_ double In_FixedTick) noexcept;

	/// <summary>
	/// オブジェクトのコンポーネント削除予約リストを処理する
	/// </summary>
	void _ExecuteDestroyObjectsComponents() noexcept;

	/// <summary>
	/// シーンの描画を行います。
	/// </summary>
	void _RootDraw() noexcept;

	/// <summary>
	/// 削除予約リストに追加されたオブジェクトを破棄します。
	/// </summary>
	void _DestroyObjects() noexcept;

	void DataSave();
	void DataLoad();

private:
	static Objects m_Objects;
#ifdef _DEBUG
	static ItemList *m_Hierarchy;
#endif // _DEBUG
	std::vector<GameObject *> m_InitObjects; // Initializeを呼び出すオブジェクトリスト
	std::string m_Name;
	cpon* m_Data; // シーンデータ

protected:
	Items m_Items;
	Items m_DeadItems; // 破棄するオブジェクトの名前リスト
	SceneManager &m_SceneManager;
};

template <typename T>
requires std::derived_from<T, GameObject>
T *SceneBase::CreateObject(_In_ const std::string &In_Name) noexcept
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator itr = m_Objects.find(In_Name);
	if (itr != m_Objects.end())
	{
		std::string buf = "Failed to create object." + In_Name;
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}

	// ヒエラルキーに追加
	m_Hierarchy->AddListItem(In_Name);

#endif // _DEBUG

	// オブジェクト生成
	T *ptr = new T();
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name)); // CPONデータ読み込み
	m_Objects.insert(std::pair<std::string, T*>(In_Name, ptr));
	m_Items.push_back(In_Name);
	m_InitObjects.push_back(ptr);
	return ptr;
}

template <typename T, typename ...Args>
requires std::derived_from<T, GameObject>
T *SceneBase::CreateObject(_In_ const std::string &In_Name, Args && ...args) noexcept
{
#ifdef _DEBUG
	// デバッグ中のみ、名称ダブりがないかチェック
	Objects::iterator itr = m_Objects.find(In_Name);
	if (itr != m_Objects.end())
	{
		std::string buf = "Failed to create object." + In_Name;
		MessageBoxA(NULL, buf.c_str(), "Error", MB_OK);
		return nullptr;
	}

	// ヒエラルキーに追加
	m_Hierarchy->AddListItem(In_Name);

#endif // _DEBUG

	// オブジェクト生成
	T *ptr = new T(args...);
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name)); // CPONデータ読み込み
	m_Objects.insert(std::pair<std::string, T *>(In_Name, ptr));
	m_Items.push_back(In_Name);
	m_InitObjects.push_back(ptr);
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
	Objects::iterator itr = m_Objects.find(In_Name.data());
	if (itr == m_Objects.end()) return nullptr;

	// 型変換
	T *ptr = dynamic_cast<T *>(itr->second);

	if(NullCheck(ptr, NullpCheckMode::OUTPUT, "Failed to get object." + std::string(In_Name)))
		return nullptr;

	return ptr;
}
