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
#include "System/Object/GameObject.hpp"

// ==============================
//  前方宣言
// ==============================
SceneBase::Objects SceneBase::m_Objects;

SceneBase::SceneBase(_In_ const std::string &In_Name) noexcept
	: m_Name(In_Name)
{
}

SceneBase::~SceneBase()
{
	// 削除
	for (auto &itr : m_Items)
	{
		DestroyObj(itr.c_str());
	}
	// 破棄予定のオブジェクトを削除
	for (auto &name : m_DeadItems)
	{
		m_Items.remove(name);
	}
	m_DeadItems.clear();
	m_Items.clear();
}

void SceneBase::Initialize() noexcept
{
#ifdef _DEBUG
	/*debug::Menu::Create("Inspector");
	debug::Window &window = debug::Menu::Create("Hierarchy");
	hierarchy = debug::Item::CreateList("Name", [](const void *arg)
		{
			const char *name = reinterpret_cast<const char *>(arg);
			auto it = m_objects.find(name);
			if (it == m_objects.end()) return;
			auto &window = debug::Menu::Get("Inspector");
			window.Clear();
			window.AddItem(debug::Item::CreateValue(it->first.c_str(), debug::Item::Label));
			if (!it->second->isGameObject) return;

			static_cast<GameObject *>(it->second->m_pObj)->Debug(&window);
		});
	window.AddItem(hierarchy);*/
#endif
}

//void SceneBase::RemoveSubScene() noexcept
//{
//	// 削除するサブシーンが存在するか
//	if (!m_pSubScene) return;
//
//	// 階層内のサブシーンを優先して削除
//	m_pSubScene->RemoveSubScene();
//
//	// 直下のサブシーンを削除
//	m_pSubScene->Uninit();
//
//	m_pSubScene.reset();
//	m_pSubScene = nullptr;
//}

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
	//hierarchy->AddListItem(In_Name.data());
#endif // _DEBUG

	GameObject *ptr = new GameObject(In_Name);
	ptr->m_pScene = this; // 所属シーンを設定
	m_Objects.insert(std::pair<std::string, SceneObjectBase *>(In_Name, new SceneObject<GameObject>(ptr)));
	m_Items.push_back(In_Name);
	return ptr;
}

void SceneBase::DestroyObj(_In_ std::string In_Name) noexcept
{
	auto obj = m_Objects.find(In_Name);
	if (obj == m_Objects.end()) return;

	delete obj->second;
	m_Objects.erase(obj);

	m_DeadItems.push_back(In_Name);
}

void SceneBase::Setup(_In_ int const &In_ModelNum) noexcept
{
	// 表示オブジェクト作成
	for (int i = 0; i < In_ModelNum; ++i)
	{
		std::string name = m_Name + "Model" + std::to_string(i);
		GameObject *obj = CreateObject<GameObject>(name.c_str());
		ModelRenderer *renderer = obj->AddComponent<ModelRenderer>();
	}
}

void SceneBase::_RootUpdateMain() noexcept
{
	// シーンが所持しているオブジェクトの更新
	for (auto &itr : m_Items)
	{
		auto objIt = m_Objects.find(itr);
		// 型チェック
		if (objIt != m_Objects.end() && objIt->second->m_bIsGameObject)
		{
			GameObject *obj = reinterpret_cast<GameObject *>(objIt->second->m_pObject);
			obj->ExecuteUpdate();
		}
	}

	// シーン自体の更新(クリア判定など
	Update();

	// 破棄されたオブジェクトの削除
	for (auto &name : m_DeadItems)
	{
		m_Items.remove(name);
	}
	m_DeadItems.clear();
}

void SceneBase::_RootUpdateLate() noexcept
{
	// シーンが所持しているオブジェクトの遅延更新
	for (auto &itr : m_Items)
	{
		auto objItr = m_Objects.find(itr);
		// 型チェック
		if (objItr != m_Objects.end() && objItr->second->m_bIsGameObject)
		{
			GameObject *obj = reinterpret_cast<GameObject *>(objItr->second->m_pObject);
			obj->ExecuteLateUpdate();
		}
	}

	// シーン自体の遅延更新
	LateUpdate();

	// 破棄予定のオブジェクトを削除
	for (auto &name : m_DeadItems)
	{
		m_Items.remove(name);
	}
	m_DeadItems.clear();
}

void SceneBase::_RootDraw() noexcept
{
	// シーンが所持しているオブジェクトの描画
	for (auto &itr : m_Items)
	{
		auto objIt = m_Objects.find(itr);
		// 型チェック
		if (objIt != m_Objects.end() && objIt->second->m_bIsGameObject)
		{
			GameObject *obj = reinterpret_cast<GameObject *>(objIt->second->m_pObject);
			obj->ExecuteDraw();
		}
	}

	// シーン自体の描画
	Draw();
}
