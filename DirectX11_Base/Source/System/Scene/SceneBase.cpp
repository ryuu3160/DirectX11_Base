/*+===================================================================
	File: SceneBase.cpp
	Summary: シーンのベースクラス
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 04:55:01 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneBase.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/GameObject.hpp"
#include "System/Scene/SceneManager.hpp"
#include "System/SpriteManager/SpriteManager.hpp"

// ==============================
//  前方宣言
// ==============================
SceneBase::Objects SceneBase::m_Objects;
#ifdef _DEBUG
ItemList *SceneBase::m_Hierarchy = nullptr;
#endif // DEBUG

SceneBase::SceneBase(_In_ const std::string &In_Name) noexcept
	: m_Name(In_Name)
	, m_SceneManager(SceneManager::GetInstance())
	, m_Data(nullptr)
{
	DataLoad();
}

SceneBase::~SceneBase()
{
	DataSave();

	// 全てのオブジェクトを削除予約に追加
	for (auto &itr : m_Items)
	{
		DestroyObj(itr.c_str());
	}
	// 破棄処理
	_DestroyObjects();

	if (m_Data)
	{
		delete m_Data;
		m_Data = nullptr;
	}
}

void SceneBase::CommonProcessScene() noexcept
{
#ifdef _DEBUG
	auto &DebugM = DebugManager::GetInstance();
	DebugWindow *window = DebugM.GetDebugWindow("System", "Hierarchy");
	m_Hierarchy = window->CreateItem<ItemList>("Name", [](const void *arg)
		{
			const char *name = reinterpret_cast<const char *>(arg);
			auto itr = m_Objects.find(name);
			if (itr == m_Objects.end()) return;
			auto *window = DebugManager::GetInstance().GetDebugWindow("System", "Inspector");
			window->ClearItems();
			window->CreateItem<ItemValue>(itr->first, DebugItem::Label);
			itr->second->RegisterDebugInspector(window);
		});
#endif
	// データの読み込み
	DataLoad();

	// メインカメラの作成
#ifdef _DEBUG
	auto CamObj = CreateObject<CameraDCC>("EditorCamera");
	auto CamCmp = CamObj->GetComponent<Camera>();
	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(CamCmp);
	SpriteManager::GetInstance().SetCameraObject(CamObj);
#else
	auto CamObj = CreateObject<CameraBaseObj>("MainCamera");
	auto CamCmp = CamObj->GetComponent<Camera>();
	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(CamCmp);
	SpriteManager::GetInstance().SetCameraObject(CamObj);
#endif
}

template<> GameObject
*SceneBase::CreateObject(_In_ const std::string &In_Name) noexcept
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
	m_Hierarchy->AddListItem(In_Name.data());
#endif // _DEBUG

	GameObject *ptr = new GameObject(In_Name);
	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name)); // CPONデータ読み込み
	m_Objects.insert(std::pair<std::string, GameObject *>(In_Name, ptr));
	m_Items.push_back(In_Name);
	m_SceneObjects.emplace(ptr);
	m_InitObjects.push_back(ptr);
	return ptr;
}

void SceneBase::DestroyObj(_In_ std::string In_Name) noexcept
{
	// 既に破棄予定リストに登録されている場合は何もしない
	for (auto &itr : m_DeadItems)
	{
		if (itr == In_Name)
			return;
	}
	m_DeadItems.push_back(In_Name);
}

void SceneBase::Setup(_In_ int const &In_ModelNum) noexcept
{
	// 表示オブジェクト作成
	for (int i = 0; i < In_ModelNum; ++i)
	{
		std::string IndexStr;
		IndexStr = ToString(i);
		std::string name = m_Name + "Model" + IndexStr;
		GameObject *obj = CreateObject<GameObject>(name.c_str());
		ModelRenderer *renderer = obj->AddComponent<ModelRenderer>();
	}
}

void SceneBase::_ObjectsInit() noexcept
{
	for (auto itr = m_InitObjects.begin(); itr != m_InitObjects.end();)
	{
		if ((*itr)->m_IsActive)
		{
			(*itr)->ExecuteInit();
			itr = m_InitObjects.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void SceneBase::_RootUpdateMain(_In_ float In_Tick) noexcept
{
	// シーンが所持しているオブジェクトの更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized)
		{
			itr->ExecuteUpdate(In_Tick);
		}
	}

	// シーン自体の更新(クリア判定など
	Update(In_Tick);
}

void SceneBase::_RootUpdateLate(_In_ float In_Tick) noexcept
{
	// シーンが所持しているオブジェクトの遅延更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized)
		{
			itr->ExecuteLateUpdate(In_Tick);
		}
	}

	// シーン自体の遅延更新
	LateUpdate(In_Tick);
}

void SceneBase::_RootFixedUpdate(_In_ double In_FixedTick) noexcept
{
	// シーンが所持しているオブジェクトの固定更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized)
		{
			itr->ExecuteFixedUpdate(In_FixedTick);
		}
	}

	// シーン自体の固定更新
	FixedUpdate(In_FixedTick);
}

void SceneBase::_ExecuteDestroyObjectsComponents() noexcept
{
	// シーンが所持しているオブジェクトのコンポーネント破棄処理を実行
	for(auto &itr : m_SceneObjects)
	{
		// 型チェック
		if (itr && itr->m_IsActive && itr->m_IsInitialized)
		{
			itr->ExecuteDestroyComponents();
		}
	}
}

void SceneBase::_RootDraw() noexcept
{
	// シーン自体の描画
	Draw();
}

void SceneBase::_DestroyObjects() noexcept
{
	// 破棄予定のオブジェクトを削除
	for (auto &name : m_DeadItems)
	{
		auto obj = m_Objects.find(name);
		if (obj == m_Objects.end()) continue;

		// シーンのオブジェクト保持リストから削除
		m_SceneObjects.erase(obj->second);
		// オブジェクトの削除
		delete obj->second;

		// 各リストから削除
		m_Objects.erase(obj);
		m_Items.remove(name);
	}
	// 破棄予定リストをクリア
	m_DeadItems.clear();
}

void SceneBase::DataSave()
{
	m_Data->ClearObjectsData();

	// 自シーンが持つオブジェクトのデータを書き込み
	for(const auto &itr : m_SceneObjects)
	{
		if (itr)
		{
			itr->DataWrite(m_Data);
		}
	}

	// ディレクトリが存在しなければ作成
	if(!std::filesystem::exists("Assets/Data/Scene/"))
		std::filesystem::create_directories("Assets/Data/Scene/");

	// ファイルに書き込み
	m_Data->WriteToFile("Assets/Data/Scene/" + m_Name + ".cpon");
}

void SceneBase::DataLoad()
{
	// 既にデータが存在する場合は削除
	if (m_Data)
	{
		delete m_Data;
		m_Data = nullptr;
	}

	// CPONデータの作成
	m_Data = new cpon();

	// ファイルから読み込み
	m_Data->LoadFromFile("Assets/Data/Scene/" + m_Name + ".cpon");
}
