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
#include "Core/System/Object/CameraObj.hpp"
#include "Core/System/Component/Camera/CameraDCC.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Managers/SceneManager.hpp"
#include "Core/System/Object/GridObject.hpp"
#include "Core/System/Object/SkyBoxObj.hpp"
#include "Core/System/Managers/DebugManager/SystemItem.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

SceneBase::SceneBase(_In_ std::string_view In_Name) noexcept
	: m_Name(In_Name)
	, m_SceneManager(SceneManager::GetInstance())
	, m_Data(nullptr)
{
	m_Hierarchy = nullptr;
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
	// エディタ用のカメラ作成
	 auto pCamObj = CreateObject_NotAddHierarchy<CameraObj>("EditorCamera");
	 pCamObj->AddComponent<CameraDCC>();

	auto &DebugM = DebugManager::GetInstance();
	DebugWindow *window = DebugM.GetDebugWindow("System", "Hierarchy");
	m_Hierarchy = window->CreateItem<ItemHierarchy>("Objects", this,
		[](GameObject *obj)
		{
			if(obj)
				obj->ReloadingInspector();
		});
#endif

	 // ゲーム用のカメラ作成
	 auto pGameCamObj = CreateObject<CameraObj>("GameCamera");

	 // デフォルトのスカイボックスを作成
	 auto Skybox = CreateObject_NotAddHierarchy<SkyBoxObj>("SkyBox",pCamObj->GetTransform());

#ifdef _DEBUG
	// グリッドオブジェクトの作成
	auto grid = CreateObject_NotAddHierarchy<GridObject>("GridObject");
#endif
}

GameObject *SceneBase::CreateObject(_In_ std::string_view In_Name, _In_opt_ Transform *In_pParent) noexcept
{
	GameObject *ptr = new GameObject(In_Name.data());

	// 所属シーン名を頭に付ける
	std::string SceneSpecificName = m_Name + "_";
	SceneSpecificName += In_Name.data();

	auto res = m_Objects.try_emplace(SceneSpecificName, ptr);

	// 名前が被っていた場合
	if(res.second == false)
	{
		// 名前の後ろに連番を付けて再度登録を試みる
		int suffix = 1;
		while(true)
		{
			std::string NewName = SceneSpecificName + "(" + ToString(suffix) + ")";
			res = m_Objects.try_emplace(NewName, ptr);
			if(res.second == true)
			{
				NewName = In_Name.data();
				NewName += "(" + ToString(suffix) + ")";
				ptr->m_Name = NewName; // オブジェクト名も変更
				ptr->m_Data->SetObjectName(NewName); // CPON上の名前も変更
				break;
			}
			++suffix;
			if(suffix > 1000) // 1000回試みてもダメなら諦める
			{
				DebugManager::GetInstance().DebugLogError("CreateObject: Failed to create object '{}'. Name conflict could not be resolved.", In_Name);
				delete ptr;
				return nullptr;
			}
		}
	}

	ptr->m_pScene = this; // 所属シーンを設定
	ptr->DataRead(m_Data->GetObjectPtr(In_Name.data())); // CPONデータ読み込み
	ptr->ExecuteAwake(); // Awake呼び出し

	if(In_pParent)
		ptr->GetTransform()->SetParent(In_pParent); // 親設定

	m_Items.push_back(In_Name.data());
	m_SceneObjects.emplace(ptr);
	m_InitObjects.push_back(ptr);

#ifdef _DEBUG
	// ヒエラルキーに追加
	m_ShowHierarchyObjects.push_back(ptr);
#endif

	return ptr;
}

GameObject * SceneBase::GetObject(_In_ std::string_view In_Name) const noexcept
{
	// オブジェクトの探索
	std::string ObjName = m_Name + "_";
	ObjName += In_Name.data();
	Objects::iterator itr = m_Objects.find(ObjName);
	if(itr == m_Objects.end())
		return nullptr;

	// 型変換
	GameObject *ptr = dynamic_cast<GameObject *>(itr->second);

	if(!ptr)
	{
		Debug::DebugLogError("GetObject: Failed to get object '{}'. Type mismatch.", In_Name);
		return nullptr;
	}

	// 破棄予約されていた場合は取得不可
	if(ptr->IsDestroySelf())
	{
		Debug::DebugLogWarning("GetObject: Object '{}' is marked for destruction.", In_Name);
		return nullptr;
	}

	return ptr;
}

void SceneBase::RenameObj(_In_ std::string_view In_OldName, _In_ std::string_view In_NewName) const
{
	std::string NewObjName = m_Name + "_";
	std::string OldObjName = m_Name + "_";
	NewObjName += In_NewName.data();
	OldObjName += In_OldName.data();
	if(m_Objects.find(NewObjName) != m_Objects.end())
	{
		DebugManager::GetInstance().DebugLogError("RenameObj: Failed to rename object. The name '{}' is already in use.", In_NewName);
		return;
	}

	auto obj = m_Objects.find(OldObjName);
	if(obj == m_Objects.end())
	{
		DebugManager::GetInstance().DebugLogError("RenameObj: Failed to rename object. The object '{}' was not found.", In_OldName);
		return;
	}

	// キー名を変更するため、一度オブジェクトを削除してから再登録
	GameObject *gameObj = obj->second;
	m_Objects.erase(obj);
	m_Objects.insert(std::pair<std::string, GameObject *>(NewObjName, gameObj));
}

void SceneBase::DestroyObj(_In_ std::string_view In_Name) noexcept
{
	// 既に破棄予定リストに登録されている場合は何もしない
	for (auto &itr : m_DeadItems)
	{
		if (itr == In_Name)
			return;
	}
	m_DeadItems.push_back(In_Name.data());
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
		if ((*itr)->m_IsActive && (*itr)->m_IsActiveParent)
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

void SceneBase::_RootUpdateMain(_In_ float In_DeltaTime) noexcept
{
	// シーンが所持しているオブジェクトの更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized && itr->m_IsActiveParent)
		{
			itr->ExecuteUpdate(In_DeltaTime);
		}
	}

	// シーン自体の更新(クリア判定など
	Update(In_DeltaTime);
}

void SceneBase::_RootUpdateLate(_In_ float In_DeltaTime) noexcept
{
	// シーンが所持しているオブジェクトの遅延更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized && itr->m_IsActiveParent)
		{
			itr->ExecuteLateUpdate(In_DeltaTime);
		}
	}

	// シーン自体の遅延更新
	LateUpdate(In_DeltaTime);
}

void SceneBase::_RootFixedUpdate(_In_ double In_FixedDeltaTime) noexcept
{
	// シーンが所持しているオブジェクトの固定更新
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら更新を実行
		if (itr && itr->m_IsActive && itr->m_IsInitialized && itr->m_IsActiveParent)
		{
			itr->ExecuteFixedUpdate(In_FixedDeltaTime);
		}
	}

	// シーン自体の固定更新
	FixedUpdate(In_FixedDeltaTime);
}

void SceneBase::_ExecuteChangeObjectsComponents() noexcept
{
	// シーンが所持しているオブジェクトのコンポーネントの変更処理
	for(auto &itr : m_SceneObjects)
	{
		// アクティブかつ初期化済みなら実行
		if(itr && itr->m_IsActive && itr->m_IsInitialized && itr->m_IsActiveParent)
		{
			itr->ExecuteChangeOrderComponents();	// 順序変更処理
			itr->ExecuteDestroyComponents();		// 削除処理
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
		std::string ObjName = m_Name + "_";
		ObjName += name;
		auto obj = m_Objects.find(ObjName);
		if (obj == m_Objects.end())
			continue;

		// シーンのオブジェクト保持リストから削除
		m_SceneObjects.erase(obj->second);
		// オブジェクトの削除
		delete obj->second;
#ifdef _DEBUG
		// ヒエラルキーから削除
		auto itr = std::find(m_ShowHierarchyObjects.begin(), m_ShowHierarchyObjects.end(), obj->second);
		if(itr != m_ShowHierarchyObjects.end())
		{
			m_ShowHierarchyObjects.erase(itr);
		}
#endif
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
	if(!std::filesystem::exists("Engine/Assets/Data/Scene/"))
		std::filesystem::create_directories("Engine/Assets/Data/Scene/");

	// ファイルに書き込み
	m_Data->WriteToFile("Engine/Assets/Data/Scene/" + m_Name + ".cpon");
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
	m_Data->LoadFromFile("Engine/Assets/Data/Scene/" + m_Name + ".cpon");
}
