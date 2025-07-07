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
	: m_pParent(nullptr)
	, m_pSubScene(nullptr)
	, m_Name(In_Name)
{
}

SceneBase::~SceneBase()
{
	// サブシーンを削除
	RemoveSubScene();

	// 削除
	while (!m_Items.empty())
	{
		DestroyObj(m_Items.begin()->c_str());
	}
	m_Items.clear();

	// 親の参照を削除
	if (m_pParent)
		m_pParent->m_pSubScene = nullptr;
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

void SceneBase::RootUpdate() noexcept
{
	// シーンが所持しているオブジェクトの更新
	for (auto &itr :m_Items)
	{
		auto objIt = m_Objects.find(itr);
		// 型チェック
		if (objIt != m_Objects.end() && objIt->second->m_bIsGameObject)
		{
			GameObject *obj = reinterpret_cast<GameObject *>(objIt->second->m_pObject);
			obj->Execute();
		}
	}

	// シーン自体の更新(クリア判定など
	Update();

	// サブシーンの更新
	if (m_pSubScene)
		m_pSubScene->RootUpdate();
}

void SceneBase::RootDraw() noexcept
{
	Draw();
	if (m_pSubScene)
		m_pSubScene->Draw();
}

void SceneBase::RemoveSubScene() noexcept
{
	// 削除するサブシーンが存在するか
	if (!m_pSubScene) return;

	// 階層内のサブシーンを優先して削除
	m_pSubScene->RemoveSubScene();

	// 直下のサブシーンを削除
	m_pSubScene->Uninit();

	delete m_pSubScene;
	m_pSubScene = nullptr;
}

template<> GameObject
*SceneBase::CreateObject(_In_ const std::string &In_Name) noexcept
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
	//hierarchy->AddListItem(In_Name.data());
#endif // _DEBUG

	GameObject *ptr = new GameObject(In_Name);
	m_Objects.insert(std::pair<std::string, SceneObjectBase *>(In_Name, new SceneObject<GameObject>(ptr)));
	m_Items.push_back(In_Name);
	return ptr;
}

void SceneBase::DestroyObj(_In_ const std::string &In_Name) noexcept
{
	auto obj = m_Objects.find(In_Name);
	if (obj == m_Objects.end()) return;

	delete obj->second;
	m_Objects.erase(obj);

	m_Items.remove(In_Name);
}

void SceneBase::Setup(_In_ const char **In_ShaderFiles, _In_ int const &In_ShaderNum, _In_ int const &In_ModelNum) noexcept
{
	for (int i = 0; i < In_ShaderNum; ++i)
	{
		Shader *shader = nullptr;
		if (strstr(In_ShaderFiles[i], "PS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<PixelShader>(In_ShaderFiles[i]);
		}
		else if (strstr(In_ShaderFiles[i], "VS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<VertexShader>(In_ShaderFiles[i]);
		}
		else if (strstr(In_ShaderFiles[i], "GS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<GeometryShader>(In_ShaderFiles[i]);
		}
		else if (strstr(In_ShaderFiles[i], "HS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<HullShader>(In_ShaderFiles[i]);
		}
		else if (strstr(In_ShaderFiles[i], "DS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<DomainShader>(In_ShaderFiles[i]);
		}
		else if (strstr(In_ShaderFiles[i], "CS_") == In_ShaderFiles[i])
		{
			shader = CreateObject<ComputeShader>(In_ShaderFiles[i]);
		}
		else
		{
			MessageBoxA(NULL, In_ShaderFiles[i], "Shader name [VS_ / PS_]", MB_OK);
		}
		std::string path = "Assets/Shader/";
		path += In_ShaderFiles[i];
		path += ".cso";
		if (shader && FAILED(shader->Load(path.c_str())))
		{
			MessageBoxA(NULL, In_ShaderFiles[i], "Shader Error", MB_OK);
		}
	}

	// 表示オブジェクト作成
	for (int i = 0; i < In_ModelNum; ++i)
	{
		std::string name = m_Name + "Model" + std::to_string(i);
		GameObject *obj = CreateObject<GameObject>(name.c_str());
		ModelRenderer *renderer = obj->AddComponent<ModelRenderer>();
	}
}
