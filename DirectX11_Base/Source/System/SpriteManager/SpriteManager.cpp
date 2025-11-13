/*+===================================================================
	File: SpriteManager.cpp
	Summary: スプライトマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/14 Mon AM 09:55:21 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManager.hpp"
#include "SpriteManagerHierarchy.hpp"
#include "SpriteManagerInspector.hpp"
#include "SpriteManagerLoader.hpp"
#include "SpriteManagerSceneSelecter.hpp"

#include "System/Scene/SceneBase.hpp"

// ==============================
//  nlohmann/json
// ==============================
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "System/Component/Camera.hpp"

// ==============================
//  定数
// ==============================
namespace
{
	const inline constexpr int cx_nSpriteRadius = 32; // スプライトの半径
	const inline constexpr float cx_fMouseSpeed = 0.005f; // マウスの移動速度
	const inline constexpr float cx_fEpsilon = 1e-15f;
}

void SpriteManager::Init() noexcept
{
	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto &io = ImGui::GetIO();
	auto &Instance = DX11_Core::GetInstance();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Window::GetInstance().GetHwnd());
	ImGui_ImplDX11_Init(Instance.GetDevice(), Instance.GetDeviceContext());

	// フォントの設定
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\NotoSansJP-Medium.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	// ドッキングの有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // ビューポートの有効化

	InitManagerWindow();

	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		m_SpriteObjects[i].clear();
		m_SpriteNames[i].clear();
	}

	LoadAllScene();
	LoadSprites();
}

void SpriteManager::Update() noexcept
{
#ifdef _DEBUG
	if (MENU_OPEN_KEY)
		m_bIsOpen = !m_bIsOpen;
#endif // _DEBUG

	if (m_pCamera == nullptr || m_pCameraObj == nullptr)
		return;

	UpdateViewAndProjection(); // カメラのビュー行列と射影行列を更新

	// ウィンドウが開いていなければ処理しない
	if (!m_bIsOpen)
		return;

	// ウィンドウにカーソルが被っている場合は処理しない
	if (m_bIsHoveredWindow)
		return;

	SpriteController();
}

void SpriteManager::Draw() noexcept
{
	//for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	//{
	//	for (auto &itr : m_SpriteObjects[i])
	//	{
	//		itr->ExecuteDraw();
	//	}
	//}
}

std::list<GameObject *> SpriteManager::Get2DSprites() noexcept
{
	return m_SpriteObjects[_2D];
}

std::list<GameObject *> SpriteManager::Get3DSprites() noexcept
{
	return m_SpriteObjects[_3D];
}

GameObject *SpriteManager::GetSprite(_In_ const std::string_view &In_SpriteName) const noexcept
{
	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		for (auto &itr : m_SpriteObjects[i])
		{
			if (itr && itr->GetName() == In_SpriteName.data())
				return itr;
		}
	}

	//for(auto &itr : m_Sprites[_2D])
	//{
	//	for (auto &sprite : itr.second)
	//	{
	//		if (sprite && sprite->GetName() == In_SpriteName.data())
	//			return sprite;
	//	}
	//}
	//for (auto &itr : m_Sprites[_3D])
	//{
	//	for (auto &sprite : itr.second)
	//	{
	//		if (sprite && sprite->GetName() == In_SpriteName.data())
	//			return sprite;
	//	}
	//}

	return nullptr;
}

GameObject *SpriteManager::CreateSprite(_In_ const std::string_view &In_SpriteName, _In_ const FilePath &In_FilePath, _In_ const bool &In_Is3D, _In_ const bool &In_IsBillBoard, _In_ const int &In_Layer, _In_ const float &In_Scale) noexcept
{
	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		// すでに同名のスプライトオブジェクトが存在するか確認
		for (auto &itr : m_SpriteObjects[i])
		{
			if (itr && itr->GetName() == In_SpriteName.data())
			{
				// すでに同盟のスプライトオブジェクトが存在する場合はそのまま返す
				return itr;
			}
		}
	}

	// 新しいスプライトオブジェクトを作成
	auto scene = SceneManager::GetInstance().GetCurrentScene();
	if (scene == nullptr) return nullptr;
	// メインのシーンでスプライトオブジェクトを作成
	std::string name = In_SpriteName.data();
	name += "_SpriteManager";
	GameObject *obj = scene->CreateObject<GameObject>(name);

	// オブジェクトを作成できなかった場合はnullptrを返す
	if (obj == nullptr)
		return nullptr;

	auto cmp = obj->AddComponent<SpriteRenderer>();
	cmp->SetAssetPath(std::string(In_FilePath));
	cmp->Set3D(In_Is3D);
	cmp->SetBillBoard(In_IsBillBoard);
	cmp->SetLayerGroup(LayerGroup_UI);
	cmp->SetLayer(In_Layer);
	cmp->SetUsePixelPosition(!In_Is3D);
	cmp->Load();

	// スプライトオブジェクトをマネージャーに登録
	if (In_Is3D)
	{
		m_SpriteObjects[RenderMode::_3D].push_back(obj);
		m_SpriteNames[RenderMode::_3D].push_back(In_SpriteName.data());
	}
	else
	{
		m_SpriteObjects[RenderMode::_2D].push_back(obj);
		m_SpriteNames[RenderMode::_2D].push_back(In_SpriteName.data());
	}
	return obj;
}

void SpriteManager::DeleteSprite(_In_ const std::string_view &In_SpriteName) noexcept
{
	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		for (auto &itr : m_SpriteObjects[i])
		{
			// 指定された名前のスプライトオブジェクトを探す
			if (itr && itr->GetName() == In_SpriteName.data())
			{
				itr->DestroySelf();
				m_SpriteObjects[i].remove(itr);
				std::string name = In_SpriteName.data();
				name = name.substr(0, name.find_last_of("_SpriteManager") - 13);
				m_SpriteNames[i].remove(name);
				return;
			}
		}
	}
}

void SpriteManager::DeleteAll() noexcept
{
	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		for (auto &itr : m_SpriteObjects[i])
		{
			itr->DestroySelf();
		}
		m_SpriteObjects[i].clear();
		m_SpriteNames[i].clear();
	}
	m_2DIndex = 0;
	m_3DIndex = 0;
}

void SpriteManager::UpdateViewAndProjection() noexcept
{
	if (m_pCamera == nullptr) return;

	bool prev = m_pCamera->Is3D();
	m_View = m_pCamera->GetView();
	m_BillBoardView = m_pCamera->GetView(false);
	m_pCamera->Set3D(true);
	m_Projection3D = m_pCamera->GetProj();
	m_pCamera->Set3D(false);
	m_Projection2D = m_pCamera->GetProj();
	m_pCamera->Set3D(prev);
}

SpriteManager::SpriteManager()
	: m_View(), m_Projection3D(), m_Projection2D(), m_BillBoardView()
	, m_bIsOpen(false), m_bIsHoveredWindow(false), m_2DIndex(0), m_3DIndex(0)
	, m_CurrentSceneIndex(0), m_PrevSceneIndex(-1)
	, m_pCamera(nullptr), m_pCameraObj(nullptr) // カメラの初期化
	, m_Selected2DSpriteIndex(-1), m_ClickPointOffsetX_2D(0), m_ClickPointOffsetY_2D(0)
	, m_bIsLeftClickTrigger(false)
	, m_Selected3DSpriteIndex(-1), m_ClickPointOffsetX_3D(0), m_ClickPointOffsetY_3D(0)
	, m_bIs3KeyTrigger(false)
	, m_fRayLength(0.0f)
{
}

SpriteManager::~SpriteManager()
{
	SaveScene(); // シーンの保存
	SaveSprites();

	DeleteAll(); // デストラクタで全てのスプライトを削除

	for (auto &itr : m_vecWindow)
	{
		delete itr;
		itr = nullptr;
	}

	if (m_pCamera)
	{
		m_pCamera = nullptr;
	}
	if(m_pCameraObj)
	{
		m_pCameraObj = nullptr;
	}

	// ImGui終了処理
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void SpriteManager::InitManagerWindow() noexcept
{
	SpriteManagerWindow *window[] =
	{
		new SpriteManagerHierarchy(),
		new SpriteManagerInspector(),
		new SpriteManagerLoader(),
		new SpriteManagerSceneSelecter()
	};

	auto size = _countof(window);
	for (int i = 0; i < size; ++i)
		m_vecWindow.push_back(window[i]);
}

void SpriteManager::DrawImGui() noexcept
{
	// メインウィドウが有効になっていなければ非表示
	if (!m_bIsOpen)
		return;

	// ウィンドウにカーソルが被っているかどうかのフラグリセット
	m_bIsHoveredWindow = false;

	// Imguiの描画準備
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ウィンドウ内の表示項目を描画
	for (int i = 0; i < m_vecWindow.size();++i)
	{
		// ウィンドウ内の描画開始
		ImGui::Begin(m_vecWindow[i]->GetWindowName().c_str());

		// ウィンドウにカーソルが被っているかどうか
		// ※ドラッグ中は除外
		if (!m_bIsLeftClickTrigger && !m_bIs3KeyTrigger)
		{
			if (ImGui::IsWindowHovered())
				m_bIsHoveredWindow = true;
		}

		// 表示
		switch (m_vecWindow[i]->GetWindowType())
		{
		case Hierarchy:
		{
			SpriteManagerHierarchy *Hierarchy = static_cast<SpriteManagerHierarchy *>(m_vecWindow[i]);
			Hierarchy->Draw(m_SpriteNames[_2D], m_SpriteNames[_3D],&m_2DIndex,&m_3DIndex);
		}

			break;

		case Inspector:
		{
			SpriteManagerInspector *Inspector = static_cast<SpriteManagerInspector *>(m_vecWindow[i]);
			auto itr2D = m_SpriteObjects[_2D].begin();
			auto itr3D = m_SpriteObjects[_3D].begin();
			GameObject *Sprite2D = nullptr;
			GameObject *Sprite3D = nullptr;

			if (!m_SpriteObjects[_2D].empty())
			{
				std::advance(itr2D, m_2DIndex);
				Sprite2D = *itr2D;
			}
			if (!m_SpriteObjects[_3D].empty())
			{
				std::advance(itr3D, m_3DIndex);
				Sprite3D = *itr3D;
			}

			// インスペクターの描画
			Inspector->Draw(Sprite2D, Sprite3D);

			break;
		}

		case Loader:
		{
			SpriteManagerLoader *Loader = static_cast<SpriteManagerLoader *>(m_vecWindow[i]);

			// ローダーの描画
			Loader->Draw();

			break;
		}

		case SceneSelecter:
		{
			m_PrevSceneIndex = m_CurrentSceneIndex; // 前のシーンインデックスを保存
			SpriteManagerSceneSelecter *SceneSelecter = static_cast<SpriteManagerSceneSelecter *>(m_vecWindow[i]);
			SceneSelecter->Draw(m_SceneSaveData, &m_CurrentSceneIndex);

			break;
		}
		}

		// ウィンドウ内の描画終了
		ImGui::End();
	}

	// ImGuiの表示
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// マルチビューポートが有効な場合の処理
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// 現在のレンダーターゲットと深度ステンシルビューを取得
		auto &RTVManager = RenderTargetManager::GetInstance();
		auto rtv = RTVManager.GetDefaultRTV();
		auto dsv = RTVManager.GetDefaultDSV();

		// ImGuiのビューポートを更新
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		// レンダーターゲットを復元
		DX11_Core &Instance = DX11_Core::GetInstance();
		Instance.SetRenderTargets(1, &rtv, dsv);
	}
}

void SpriteManager::CreateScene(_In_ const std::string_view &In_SceneName) noexcept
{
	// シーンを作成
	if (m_SceneSaveData.find(In_SceneName.data()) == m_SceneSaveData.end())
	{
		std::string scenePathStr = "Assets\\SpriteManager\\" + std::string(In_SceneName) + "Sprites" + ".json";
		FilePath scenePath = scenePathStr.c_str();

		m_SceneSaveData[In_SceneName.data()] = scenePath;
	}
}

void SpriteManager::ChangeScene(_In_ const int &In_Index) noexcept
{
	// インデックスが指定された場合は、インデックスを変更
	if (In_Index != -1)
	{
		if(In_Index < 0 || In_Index >= m_SceneSaveData.size())
			return; // 範囲外なら何もしない

		if(In_Index == m_CurrentSceneIndex)
			return; // 同じシーンなら何もしない
		m_CurrentSceneIndex = In_Index;
	}

	if(m_CurrentSceneIndex == m_PrevSceneIndex)
		return; // 同じシーンなら何もしない

	SaveSprites(); // 現在のスプライトを保存
	// 現在のスプライトを削除
	DeleteAll();

	auto itr = m_SceneSaveData.begin();
	std::advance(itr, m_CurrentSceneIndex);
	m_CurrentSceneName = itr->first; // 現在のシーン名を更新

	LoadSprites(); // スプライトをロード
	m_PrevSceneIndex = m_CurrentSceneIndex; // 前のシーンインデックスを更新
}

void SpriteManager::ChangeScene(_In_ const std::string_view &In_SceneName) noexcept
{
	if(m_CurrentSceneName == In_SceneName.data())
		return; // 同じシーンなら何もしない

	// シーン名を検索
	auto itr = m_SceneSaveData.find(In_SceneName.data());
	if(itr == m_SceneSaveData.end())
		return; // 見つからなければ何もしない

	SaveSprites(); // 現在のスプライトを保存
	// 現在のスプライトを削除
	DeleteAll();

	m_CurrentSceneName = itr->first; // 現在のシーン名を更新
	m_CurrentSceneIndex = static_cast<int>(std::distance(m_SceneSaveData.begin(), itr)); // 現在のシーンインデックスを更新
	m_PrevSceneIndex = m_CurrentSceneIndex; // 前のシーンインデックスを更新
	LoadSprites(); // スプライトをロード
}

void SpriteManager::DeleteScene(_In_ const std::string &In_SceneName) noexcept
{
	auto itr = m_SceneSaveData.begin();
	if (In_SceneName.empty())
		itr = m_SceneSaveData.find(m_CurrentSceneName);
	else
		itr = m_SceneSaveData.find(In_SceneName);

	if(itr == m_SceneSaveData.end())
		return; // 見つからなければ何もしない

	// 現在のシーンを削除する場合は、スプライトを削除してからシーンを削除
	if (m_CurrentSceneName == itr->first)
		DeleteAll();
	// ---- シーンを削除 ----

	// シーンインデックスを0に設定
	m_CurrentSceneIndex = 0;
	m_PrevSceneIndex = 0;

	// シーンのセーブデータを削除
	std::filesystem::remove(itr->second);
	// シーンの一覧から削除
	m_SceneSaveData.erase(itr);

	// シーンをセーブ
	SaveScene();

	// シーンの変更
	auto NowItr = m_SceneSaveData.begin();
	std::advance(NowItr, m_CurrentSceneIndex);
	m_CurrentSceneName = NowItr->first; // 現在のシーン名を更新

	LoadSprites(); // スプライトをロード
	m_PrevSceneIndex = m_CurrentSceneIndex; // 前のシーンインデックスを更新
}

void SpriteManager::DeleteScene(_In_ const int &In_Index) noexcept
{
	auto itr = m_SceneSaveData.begin();
	if (In_Index == -1)
		std::advance(itr, m_CurrentSceneIndex);
	else
	{
		if(In_Index < 0 || In_Index >= m_SceneSaveData.size())
			return; // 範囲外なら何もしない
		std::advance(itr, In_Index);
	}

	if (itr == m_SceneSaveData.end())
		return; // 見つからなければ何もしない

	// 現在のシーンを削除する場合は、スプライトを削除してからシーンを削除
	if (m_CurrentSceneName == itr->first)
		DeleteAll();
	// ---- シーンを削除 ----

	// シーンインデックスを0に設定
	m_CurrentSceneIndex = 0;
	m_PrevSceneIndex = 0;

	// シーンのセーブデータを削除
	std::filesystem::remove(itr->second);
	// シーンの一覧から削除
	m_SceneSaveData.erase(itr);

	// シーンをセーブ
	SaveScene();

	// シーンの変更
	auto NowItr = m_SceneSaveData.begin();
	std::advance(NowItr, m_CurrentSceneIndex);
	m_CurrentSceneName = NowItr->first; // 現在のシーン名を更新

	LoadSprites(); // スプライトをロード
	m_PrevSceneIndex = m_CurrentSceneIndex; // 前のシーンインデックスを更新
}

void SpriteManager::ConvertTo2D() noexcept
{
	// 現在選択されている3Dスプライトを取得
	auto pSprite = m_SpriteObjects[_3D].begin();
	std::advance(pSprite, m_3DIndex);

	// 同じ名前のスプライトが2Dスプライトリストに存在しているか確認
	if (CheckConflictSpriteName((*pSprite)->GetName(), _2D))
	{
		// 既に同名のスプライトが存在する場合は変換しない
		MessageBoxA(nullptr, "変換先のリストに同名のスプライトが存在しています。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}

	auto SR = (*pSprite)->GetComponent<SpriteRenderer>();
	// 3Dスプライトを2Dスプライトに変換
	SR->Set3D(false);
	SR->SetBillBoard(false); // ビルボードは無効化
	// 2Dスプライトのリストに移動
	m_SpriteObjects[_2D].splice(m_SpriteObjects[_2D].begin(), m_SpriteObjects[_3D], pSprite);
	m_SpriteNames[_2D].splice(m_SpriteNames[_2D].begin(), m_SpriteNames[_3D], std::next(m_SpriteNames[_3D].begin(), m_3DIndex));

	--m_3DIndex; // インデックスを戻す
	if(m_3DIndex < 0)
		m_3DIndex = 0; // インデックスが負にならないようにする

	return;
}

void SpriteManager::ConvertTo3D() noexcept
{
	// 現在選択されている2Dスプライトを取得
	auto pSprite = m_SpriteObjects[_2D].begin();
	std::advance(pSprite, m_2DIndex);

	// 同じ名前のスプライトが3Dスプライトリストに存在しているか確認
	if (CheckConflictSpriteName((*pSprite)->GetName(), _3D))
	{
		// 既に同名のスプライトが存在する場合は変換しない
		MessageBoxA(nullptr, "変換先のリストに同名のスプライトが存在しています。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}

	auto SR = (*pSprite)->GetComponent<SpriteRenderer>();
	// 2Dスプライトを3Dスプライトに変換
	SR->Set3D(true);
	SR->SetBillBoard(false); // ビルボードは無効化
	// 2Dスプライトのリストに移動
	m_SpriteObjects[_3D].splice(m_SpriteObjects[_3D].begin(), m_SpriteObjects[_2D], pSprite);
	m_SpriteNames[_3D].splice(m_SpriteNames[_3D].begin(), m_SpriteNames[_2D], std::next(m_SpriteNames[_2D].begin(), m_2DIndex));

	--m_2DIndex; // インデックスを戻す
	if (m_2DIndex < 0)
		m_2DIndex = 0; // インデックスが負にならないようにする

	return;
}

void SpriteManager::SaveSprites() const noexcept
{
	json data;

	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		std::string ModeName = (i == _2D) ? "2D" : "3D";
		std::string KeyName = ModeName + "_Sprites";
		std::string Name = ModeName + "_Sprite";

		for (auto &itr : m_SpriteObjects[i])
		{
			auto SR = itr->GetComponent<SpriteRenderer>();
			json work;
			std::string name = itr->GetName();
			name = name.substr(0, name.find_last_of("_SpriteManager") - 13); // "_SpriteManager"を除いた名前を保存
			work[Name]["Name"] = name;
			work[Name]["FilePath"] = SR->GetAssetPath();
			work[Name]["Position"] = { itr->GetPosition().x, itr->GetPosition().y, itr->GetPosition().z };
			work[Name]["Scale"] = { itr->GetScale().x, itr->GetScale().y, itr->GetScale().z };
			work[Name]["Rotation"] = { itr->GetRotation().x, itr->GetRotation().y, itr->GetRotation().z };
			work[Name]["Layer"] = SR->GetLayer();
			work[Name]["Is3D"] = SR->GetIs3D();
			work[Name]["IsBillBoard"] = SR->GetIsBillBoard();
			data[KeyName].push_back(work); // 2Dスプライトのデータを追加
		}
	}

	std::string path = "Assets\\SpriteManager";
	std::string FileName = m_CurrentSceneName + "Sprites.json";
	std::string OpenPath = path + "\\" + FileName;
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	std::ofstream file(OpenPath, std::ios::out);

	if (file.is_open())
	{
		file << data.dump() << std::endl; // JSON形式で保存
		file.close();
	}
	else
	{
		OutputDebugStringA("error : Failed to open file for writing.\n");
	}
}

void SpriteManager::LoadSprites() noexcept
{
	std::string path = "Assets\\SpriteManager";
	std::string FileName = m_CurrentSceneName + "Sprites.json";
	std::string OpenPath = path + "\\" + FileName;
	if (!std::filesystem::exists(path))
	{
		OutputDebugStringA("error : SpriteManager directory does not exist.\n");
		return;
	}

	std::ifstream file(OpenPath, std::ios::in);

	if (file.is_open())
	{
		// JSONファイルの形式を確認
		if(!json::accept(file))
		{
			OutputDebugStringA("error : Invalid JSON format in file.\n");
			return;
		}
		// ポジションを戻す
		file.seekg(0, std::ios::beg);

		// JSONデータを読み込む
		json data = json::parse(file);

		// サイズの確認
		int size = static_cast<int>(data.size());
		if(size > _MAX_RENDER_MODE)
		{
			// JSONデータのサイズが最大レンダーモード数を超えている場合
			OutputDebugStringA("error : JSON data exceeds maximum render mode size.\n");
			return;
		}

		// 2Dスプライトと3Dスプライトの数をカウント
		int *CountSprite = new int[size];
		std::string ModeName[_MAX_RENDER_MODE] = { "2D_Sprites", "3D_Sprites" };
		std::string KeyName[_MAX_RENDER_MODE] = { "2D_Sprite", "3D_Sprite" };

		for(int i = 0; i < size; ++i)
			CountSprite[i] = static_cast<int>(data[ModeName[i]].size());

		// スプライトのデータを読み込み
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < CountSprite[i]; ++j)
			{
				json spriteData = data[ModeName[i]][j][KeyName[i]];
				std::string name = spriteData["Name"];
				FilePath filePath = spriteData["FilePath"];
				DirectX::XMFLOAT3 position = { spriteData["Position"][0], spriteData["Position"][1], spriteData["Position"][2] };
				DirectX::XMFLOAT3 scale = { spriteData["Scale"][0], spriteData["Scale"][1], spriteData["Scale"][2] };
				DirectX::XMFLOAT3 rotation = { spriteData["Rotation"][0], spriteData["Rotation"][1], spriteData["Rotation"][2] };
				int layer = spriteData["Layer"];
				bool is3D = spriteData["Is3D"];
				bool isBillBoard = spriteData["IsBillBoard"];
				GameObject *sprite = CreateSprite(name, filePath, is3D, isBillBoard, layer);
				if (sprite)
				{
					sprite->SetPosition(position);
					sprite->SetScale(scale);
					sprite->SetRotation(rotation);
				}
			}
		}

		delete[] CountSprite; // 動的配列の解放
		file.close(); // ファイルを閉じる
	}
}

void SpriteManager::SaveScene() const noexcept
{
	if (!std::filesystem::exists("Assets\\SpriteManager"))
		std::filesystem::create_directories("Assets\\SpriteManager");

	std::ofstream file("Assets\\SpriteManager\\Scenes.csv", std::ios::trunc);

	if (file.is_open())
	{
		file << m_CurrentSceneName << "\n"; // 現在のシーン名を保存
		for (auto &itr : m_SceneSaveData)
		{
			file << itr.first << "," << itr.second << "\n"; // シーン名とパスをCSV形式で保存
		}
		file.close();
	}
}

void SpriteManager::LoadAllScene() noexcept
{
	if(!std::filesystem::exists("Assets\\SpriteManager"))
		std::filesystem::create_directories("Assets\\SpriteManager");

	std::ifstream file("Assets\\SpriteManager\\Scenes.csv");
	if (file.is_open())
	{
		m_SceneSaveData.clear(); // 既存のデータをクリア
		std::string line;
		// 最初の行は現在のシーン名を読み込む
		if (std::getline(file, line))
		{
			m_CurrentSceneName = line.substr(0,line.find(',')); // 現在のシーン名を設定
		}

		while (std::getline(file, line))
		{
			size_t commaPos = line.find(',');
			if (commaPos != std::string::npos)
			{
				std::string sceneName = line.substr(0, commaPos);
				std::string scenePath = line.substr(commaPos + 1);
				m_SceneSaveData[sceneName] = FilePath{ scenePath.c_str() }; // FilePathに変換して保存
			}
		}
		file.close();
	}
	else
	{
		OutputDebugStringA("error : Failed to open Scenes.csv for reading.\n");
		m_SceneSaveData.clear();
	}
}

bool SpriteManager::CheckConflictSpriteName(_In_ const std::string_view &In_SpriteName, _In_ const RenderMode &In_Mode) const noexcept
{
	// 指定されたレンダーモードのスプライト名が既に存在するか確認
	for (auto &itr : m_SpriteNames[In_Mode])
	{
		if (itr == In_SpriteName.data())
			return true; // 既に存在する場合はtrueを返す
	}
	return false; // 存在しない場合はfalseを返す
}

void SpriteManager::SpriteController() noexcept
{
	CursorHit2DSprite(); // マウスカーソルが2Dスプライトに当たったか確認
	MouseControl2DSprite(); // マウス操作で2Dスプライトを動かす

	CursorHit3DSprite();	 // マウスカーソルが3Dスプライトに当たったか確認
	MouseControl3DSprite(); // マウス操作で3Dスプライトを動かす
}

void SpriteManager::CursorHit2DSprite() noexcept
{
	if (Input::IsKeyTrigger(VK_LBUTTON) && !Input::IsKeyPress('3'))
	{
		// マウスカーソルの位置を取得
		DirectX::XMFLOAT2 MousePos;
		MousePos.x = Input::GetMouseRelativePos_CenterZero().x;	// ウィンドウの中心からの相対位置
		MousePos.y = Input::GetMouseRelativePos_CenterZero().y; // ウィンドウの中心からの相対位置
		MousePos.y *= -1.0f; // Y座標を反転(ウィンドウの座標系に合わせる)

		// マウスカーソルが2Dスプライトに当たったか確認
		// 手前のスプライトを優先するため、逆イテレータを使用
		auto itr = m_SpriteObjects[_2D].rbegin();
		for (int i = static_cast<int>(m_SpriteObjects[_2D].size()) - 1; i >= 0;--i,++itr)
		{
			if (*itr)
			{
				// スケールを取得
				float ScaleX = (*itr)->GetScale().x;
				float ScaleY = (*itr)->GetScale().y;

				// スプライトの中心座標を取得
				auto cmp = (*itr)->GetComponent<SpriteRenderer>();
				DirectX::XMFLOAT3 center = cmp->GetPositionPixel();
				// スプライトの四隅の座標を計算
				DirectX::XMFLOAT2 TopLeft = { center.x - (cx_nSpriteRadius * ScaleX), center.y - (cx_nSpriteRadius * ScaleY) };
				DirectX::XMFLOAT2 BottomRight = { center.x + (cx_nSpriteRadius * ScaleX), center.y + (cx_nSpriteRadius * ScaleY) };

				// カーソルと当たっているか
				if (MousePos.x >= TopLeft.x && MousePos.x <= BottomRight.x &&
					MousePos.y >= TopLeft.y && MousePos.y <= BottomRight.y)
				{
					m_ClickPointOffsetX_2D = MousePos.x - center.x; // カーソルのX座標とスプライトの中心X座標の差を保存
					m_ClickPointOffsetY_2D = MousePos.y - center.y; // カーソルのY座標とスプライトの中心Y座標の差を保存
					m_Selected2DSpriteIndex = i; // 選択されたスプライトのインデックスを保存
					m_bIsLeftClickTrigger = true; // 左クリックがトリガーされたことを示すフラグを設定
					m_2DIndex = m_Selected2DSpriteIndex; // インデックスを更新
					return; // 当たったスプライトが見つかったので終了
				}
			}
		}

		m_Selected2DSpriteIndex = -1; // 当たったスプライトがない場合は-1を設定
	}
}

void SpriteManager::MouseControl2DSprite() noexcept
{
	if(m_Selected2DSpriteIndex == -1)
	{
		m_bIsLeftClickTrigger = false; // 選択されていない場合はフラグをリセット
		return; // スプライトが選択されていないので何もしない
	}

	if (m_bIsLeftClickTrigger && !Input::IsKeyRelease(VK_LBUTTON) && !m_bIs3KeyTrigger)
	{
		// 対象のスプライトを取得
		auto itr = m_SpriteObjects[_2D].begin();
		std::advance(itr, m_Selected2DSpriteIndex);
		if (*itr)
		{
			// スプライトの位置を取得
			auto cmp = (*itr)->GetComponent<SpriteRenderer>();
			DirectX::XMFLOAT3 pos = cmp->GetPositionPixel();

			// スプライトの位置をカーソル移動に合わせて動かす
			pos.x = Input::GetMouseRelativePos_CenterZero().x - m_ClickPointOffsetX_2D; // カーソルのX座標からオフセットを引く
			// Y座標のオフセットはDirectX11の座標系に合わせてあるので、マウス座標のみ座標系を合わせる
			pos.y = (-Input::GetMouseRelativePos_CenterZero().y) - m_ClickPointOffsetY_2D; // カーソルのY座標からオフセットを引く

			pos.z = 0.0f; // 2DスプライトなのでZ座標は0に設定

			// スプライトの位置を更新
			cmp->SetPositionPixel(pos);
		}
	}
	else
	{
		m_bIsLeftClickTrigger = false; // 左クリックが離されたのでフラグをリセット
		m_Selected2DSpriteIndex = -1; // 選択されたスプライトのインデックスをリセット
	}
}

void SpriteManager::CursorHit3DSprite() noexcept
{
	if (m_bIs3KeyTrigger)
		return;

	if (Input::IsKeyPress('3') && Input::IsKeyTrigger(VK_LBUTTON))
	{
		// マウス座標(スクリーン座標)をワールド座標に変換
		DirectX::XMVECTOR mousePos = ScreenToWorldPos(Input::GetMouseRelativePos(), 0.5f, m_pCamera->GetView(false), m_pCamera->GetProj(false));
		// カメラの位置を取得
		DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPosition();
		DirectX::XMVECTOR vCam = DirectX::XMLoadFloat3(&CamPos);

		// カメラ座標からマウス座標へのベクトルを取得、方向に変換するために正規化
		DirectX::XMVECTOR vRayDir = DirectX::XMVectorSubtract(mousePos, vCam);
		vRayDir = DirectX::XMVector3Normalize(vRayDir);

		SerchHitRay3DSprite(vCam, vRayDir); // レイキャストで3Dスプライトを検索

		if (m_Selected3DSpriteIndex != -1)
		{
			// マウスカーソルが3Dスプライトに当たった場合、インデックスを更新
			m_bIs3KeyTrigger = true; // 3Dスプライトが選択されたことを示すフラグを設定
			m_3DIndex = m_Selected3DSpriteIndex; // インデックスを更新
		}
	}
}

void SpriteManager::MouseControl3DSprite() noexcept
{
	if (m_Selected3DSpriteIndex == -1)
	{
		m_bIs3KeyTrigger = false;
		return;
	}

	if (m_bIs3KeyTrigger && !Input::IsKeyRelease(VK_LBUTTON) && !m_bIsLeftClickTrigger)
	{
		// 対象のスプライトを取得
		auto itr = m_SpriteObjects[_3D].begin();
		std::advance(itr, m_Selected3DSpriteIndex);

		if (*itr)
		{
			// スプライトの位置を取得
			DirectX::XMFLOAT3 pos = (*itr)->GetPosition();

			// ------- 毎フレームのマウス座標更新処理 -------

			// マウス座標(スクリーン座標)をワールド座標に変換
			DirectX::XMVECTOR Mouse = ScreenToWorldPos(Input::GetMouseRelativePos(), 0.5f, m_pCamera->GetView(false), m_pCamera->GetProj(false));
			// カメラの位置を取得
			DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPosition();
			DirectX::XMVECTOR vCam = DirectX::XMLoadFloat3(&CamPos);

			// カメラ座標からマウス座標へのベクトルを取得、方向に変換するために正規化
			DirectX::XMVECTOR vRayDir = DirectX::XMVectorSubtract(Mouse, vCam);
			vRayDir = DirectX::XMVector3Normalize(vRayDir);

			DirectX::XMFLOAT3 scale = (*itr)->GetScale() / 2.0f; // スプライトのスケールを取得
			scale.z = cx_fEpsilon; // Z軸のスケールは小さく設定(平面として扱う)
			DirectX::XMFLOAT4 qua = (*itr)->GetQuat(); // スプライトのクォータニオンを取得
			float fRayLength;

			// レイと平面の交差判定を行う
			bool result = IntersectRayPlane(vCam, vRayDir, pos, scale, qua, fRayLength);

			if(result)
				m_fRayLength = std::abs(fRayLength); // レイの長さを保存

			float x = Input::GetMouseDifference().x * (m_fRayLength * 0.00145f); // マウスのX座標の移動量を取得
			float y = -Input::GetMouseDifference().y * (m_fRayLength * 0.00145f); // マウスのY座標の移動量を取得

			// マウスホイールの回転を取得
			float z = static_cast<float>(Input::GetMouseWheelDelta()) / 10.0f;

			// スプライトの位置をカーソル移動に合わせて動かす
			pos.x += x;
			// Y座標のオフセットはDirectX11の座標系に合わせてあるので、マウス座標のみ座標系を合わせる
			pos.y += y;

			// Z座標はマウスホイールの回転で調整
			pos.z += z;

			// スプライトの位置を更新
			(*itr)->SetPosition(pos);
		}
	}
	else
	{
		m_bIs3KeyTrigger = false; // 3Dスプライトの選択が解除されたのでフラグをリセット
		m_Selected3DSpriteIndex = -1; // 選択されたスプライトのインデックスをリセット
	}
}

void SpriteManager::SerchHitRay3DSprite(_In_ DirectX::XMVECTOR In_vRayPos, _In_ DirectX::XMVECTOR In_vRayDir) noexcept
{
	float fObjPos = 0.0f;
	float fNewObjPos = 0.0f;
	int WorkIndex = -1; // 当たったスプライトのインデックスを保持
	auto itr = m_SpriteObjects[_3D].begin();
	for (int i = 0;i < m_SpriteObjects[_3D].size();++i,++itr)
	{
		if (*itr == nullptr)
			continue;

		DirectX::XMFLOAT3 scale = (*itr)->GetScale() / 2.0f;
		DirectX::XMFLOAT4 qua = (*itr)->GetQuat();
		scale.z = cx_fEpsilon;
		auto Pos = (*itr)->GetPosition();
		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&Pos);
		DirectX::XMVECTOR vExtents = DirectX::XMLoadFloat3(&scale);
		DirectX::XMVECTOR vOrientation = DirectX::XMLoadFloat4(&qua);

		float fRayLength;

		bool result = IntersectRayPlane(In_vRayPos, In_vRayDir, (*itr)->GetPosition(), scale, (*itr)->GetQuat(), fRayLength);

		if (result)
		{
			if (WorkIndex == -1)
			{
				WorkIndex = i; // スプライトが選択されていない場合はインデックスを設定
				m_fRayLength = fRayLength; // レイの長さを保存
			}
			else
			{
				float SpriteLength = 0.0f;
				float NewSpriteLength = 0.0f;
				auto NowSprite = m_SpriteObjects[_3D].begin();
				std::advance(NowSprite, WorkIndex);
				auto NowPos = (*NowSprite)->GetPosition();
				DirectX::XMStoreFloat(&SpriteLength, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&NowPos), In_vRayPos));
				DirectX::XMStoreFloat(&NewSpriteLength, DirectX::XMVectorSubtract(vCenter, In_vRayPos));

				if (std::abs(SpriteLength) > std::abs(NewSpriteLength))
				{
					WorkIndex = i; // 新しいスプライトの方が近い場合はインデックスを更新
					m_fRayLength = fRayLength; // レイの長さを更新
				}
			}
		}
	}
	// 3Dスプライトに当たっていたら、そのインデックスを保存
	if (WorkIndex != -1)
		m_Selected3DSpriteIndex = WorkIndex; // 当たったスプライトのインデックスを保存
}
