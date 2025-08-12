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
	auto &Instance = DX11_Initialize::GetInstance();

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
		m_Sprites[i].clear();
		m_SpriteNames[i].clear();
	}

	LoadAllScene();
	LoadSprites();
}

void SpriteManager::Update() noexcept
{
	if (MENU_OPEN_KEY)
		m_bIsOpen = !m_bIsOpen;

	UpdateViewAndProjection();

	SpriteController();

	std::string pos;
	for(const auto &itr : m_SpritePointerList[_3D])
	{
		if (itr == nullptr)
			continue;

		pos = itr->GetName();
		if (itr)
		{
			pos += "Pos : X :";
			pos += std::to_string(itr->GetPosition().x);
			pos += " Y : ";
			pos += std::to_string(itr->GetPosition().y);
			pos += " Z : ";
			pos += std::to_string(itr->GetPosition().z);
			pos += " Scale : ";
			pos += std::to_string(itr->GetScale().x);
			pos += " Y : ";
			pos += std::to_string(itr->GetScale().y);
			pos += " Z : ";
			pos += std::to_string(itr->GetScale().z);
			pos += "\n";
		}
		OutputDebugStringA(pos.c_str());
	}

	std::string mouseWorld;
	// スプライトの位置を取得
	DirectX::XMFLOAT3 SpritePos = (*m_SpritePointerList[_3D].begin())->GetPosition();
	SpritePos.y = 0.0f; // Y座標を0に設定して水平面上の位置を計算
	SpritePos.z = 0.0f; // Z座標も0に設定

	// カメラからスプライトの距離を取得
	float distance;
	DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPos();
	CamPos.y = 0.0f; // Y座標を0に設定して水平面上の距離を計算
	CamPos.z = 0.0f; // Z座標も0に設定
	DirectX::XMStoreFloat(&distance, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&SpritePos), DirectX::XMLoadFloat3(&CamPos)));
	//distance = std::abs(distance);
	OutputDebugStringA(("Distance : " + std::to_string(distance) + "\n").c_str());
	DirectX::XMVECTOR mousePos = ScreenToWorldPos(Input::GetMouseRelativePos(),distance, m_pCamera->GetView(false), m_pCamera->GetProj(false));
	DirectX::XMFLOAT3 mouseWorldPos;
	DirectX::XMStoreFloat3(&mouseWorldPos, mousePos);
	mouseWorld = "Mouse World Position\nX : ";
	mouseWorld += std::to_string(mouseWorldPos.x);
	mouseWorld += " Y : ";
	mouseWorld += std::to_string(mouseWorldPos.y);
	mouseWorld += " Z : ";
	mouseWorld += std::to_string(mouseWorldPos.z);
	mouseWorld += "\n";
	OutputDebugStringA(mouseWorld.c_str());
}

void SpriteManager::Draw2D() noexcept
{
	// 2Dスプライトの描画
	for (auto &itr : m_Sprites[_2D])
	{
		for (auto &sprite : itr.second)
		{
			if (sprite)
			{
				DirectX::XMFLOAT4X4 view;
				DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
				DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
				sprite->SetView(view);
				sprite->SetProjection(m_Projection2D);
				sprite->Draw();
			}
		}
	}
}

void SpriteManager::Draw3D() noexcept
{
	for (auto &itr : m_Sprites[_3D])
	{
		for (auto &sprite : itr.second)
		{
			if (sprite)
			{
				sprite->SetView(m_View);
				sprite->SetProjection(m_Projection3D);
				// ビルボードの場合は専用のView行列を設定
				if (sprite->GetIsBillBoard())
					sprite->SetBillBoardView(m_BillBoardView);

				sprite->Draw();
			}
		}
	}
}

std::list<Sprite *> SpriteManager::Get2DSprites() noexcept
{
	std::list<Sprite *> work;
	for (auto &itr : m_Sprites[_2D])
	{
		work.splice(work.end(), itr.second);
	}
	return work;
}

std::list<Sprite *> SpriteManager::Get3DSprites() noexcept
{
	std::list<Sprite *> work;
	for (auto &itr : m_Sprites[_3D])
	{
		work.splice(work.end(), itr.second);
	}
	return work;
}

Sprite *SpriteManager::GetSprite(_In_ const std::string_view &In_SpriteName) const noexcept
{

	return nullptr;
}

Sprite *SpriteManager::CreateSprite(_In_ const std::string_view &In_SpriteName, _In_ const FilePath &In_FilePath, _In_ const bool &In_Is3D, _In_ const bool &In_IsBillBoard, _In_ const int &In_Layer, _In_ const float &In_Scale) noexcept
{
	for (auto &itr : m_Sprites[In_Is3D ? _3D : _2D])
	{
		// 既に同名のスプライトが存在するか確認
		for (auto &sprite : itr.second)
		{
			if (sprite && sprite->GetName() == In_SpriteName.data())
				return sprite; // 既に存在する場合はそのまま返す
		}
	}

	// 新しいスプライトを作成
	Sprite *work = new Sprite();
	work->Load(In_FilePath, In_Layer, In_Scale);
	work->SetName(In_SpriteName.data());
	work->SetFilePath(In_FilePath.data());
	work->Set3D(In_Is3D);
	if (In_Is3D)
		work->SetBillBoard(In_IsBillBoard);
	else
		work->SetBillBoard(false); // 2Dスプライトはビルボードにしない
	// スプライトをマネージャーに登録
	m_Sprites[In_Is3D ? _3D : _2D][In_Layer].push_back(work);
	// スプライト名を保存
	m_SpriteNames[In_Is3D ? _3D : _2D].push_back(In_SpriteName.data());
	// スプライトへのポインタをリストに追加(名前のリストと順番が対応する)
	m_SpritePointerList[In_Is3D ? _3D : _2D].push_back(work);

	return work;
}

void SpriteManager::DeleteAll() noexcept
{
	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		for (auto &itr : m_Sprites[i])
		{
			for (auto &sprite : itr.second)
			{
				delete sprite;
				sprite = nullptr;
			}
			itr.second.clear();
		}
		m_Sprites[i].clear();
		m_SpriteNames[i].clear();
		m_SpritePointerList[i].clear();
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
	, m_bIsOpen(false), m_2DIndex(0), m_3DIndex(0)
	, m_CurrentSceneIndex(0)
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

	// Imguiの描画準備
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// ウィンドウ内の表示項目を描画
	for (int i = 0; i < m_vecWindow.size();++i)
	{
		// ウィンドウ内の描画開始
		ImGui::Begin(m_vecWindow[i]->GetWindowName().c_str());

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
			auto itr2D = m_SpritePointerList[_2D].begin();
			auto itr3D = m_SpritePointerList[_3D].begin();
			Sprite *Sprite2D = nullptr;
			Sprite *Sprite3D = nullptr;

			if (!m_SpritePointerList[_2D].empty())
			{
				std::advance(itr2D, m_2DIndex);
				Sprite2D = *itr2D;
			}
			if (!m_SpritePointerList[_3D].empty())
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
		auto rtv = Main::GetRenderTarget();
		auto dsv = Main::GetDepthStencil();

		// ImGuiのビューポートを更新
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		// レンダーターゲットを復元
		DX11_Initialize &Instance = DX11_Initialize::GetInstance();
		Instance.SetRenderTargets(1, &rtv, dsv);
	}
}

void SpriteManager::CreateScene(_In_ const std::string_view &In_SceneName) noexcept
{
	// シーンを作成
	if (m_SceneSaveData.find(In_SceneName.data()) == m_SceneSaveData.end())
	{
		std::string scenePathStr = "Assets\\SpriteManager\\" + std::string(In_SceneName) + ".json";
		FilePath scenePath = scenePathStr.c_str();

		m_SceneSaveData[In_SceneName.data()] = scenePath;
	}
}

void SpriteManager::ChangeScene(_In_ const int &In_Index) noexcept
{
	// インデックスが指定された場合は、インデックスを変更
	if (In_Index != -1)
	{
		m_CurrentSceneIndex = In_Index;
	}

	SaveSprites(); // 現在のスプライトを保存
	// 現在のスプライトを削除
	DeleteAll();

	auto itr = m_SceneSaveData.begin();
	std::advance(itr, m_CurrentSceneIndex);
	m_CurrentSceneName = itr->first; // 現在のシーン名を更新

	LoadSprites(); // スプライトをロード
}

void SpriteManager::ConvertTo2D() noexcept
{
	// 現在選択されている3Dスプライトを取得
	auto pSprite = m_SpritePointerList[_3D].begin();
	std::advance(pSprite, m_3DIndex);

	// 同じ名前のスプライトが2Dスプライトリストに存在しているか確認
	if (CheckConflictSpriteName((*pSprite)->GetName(), _2D))
	{
		// 既に同名のスプライトが存在する場合は変換しない
		MessageBoxA(nullptr, "変換先のリストに同名のスプライトが存在しています。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}

	// 3Dスプライトのリストから、現在選択されているスプライトを探す
	for (auto &itr : m_Sprites[_3D])
	{
		// レイヤーごとにスプライトを検索
		for (auto sprite = itr.second.begin();sprite != itr.second.end();)
		{
			// スプライトが現在選択されているスプライトと一致するか確認
			if((*sprite) == (*pSprite))
			{
				// 3Dスプライトを2Dスプライトに変換
				(*sprite)->Set3D(false);
				(*sprite)->SetBillBoard(false); // ビルボードは無効化
				m_Sprites[_2D][(*sprite)->GetLayer()].push_back((*sprite)); // 2Dスプライトのリストに追加
				m_SpriteNames[_2D].push_back((*sprite)->GetName()); // スプライト名を追加
				m_SpritePointerList[_2D].push_back((*sprite)); // ポインタリストにも追加
				// 3Dスプライトから削除
				itr.second.erase(sprite);
				// スプライト名のリストからも削除
				m_SpriteNames[_3D].erase(std::next(m_SpriteNames[_3D].begin(), m_3DIndex));
				// スプライトポインタリストからも削除
				m_SpritePointerList[_3D].erase(std::next(m_SpritePointerList[_3D].begin(), m_3DIndex));

				--m_3DIndex; // インデックスを戻す
				if(m_3DIndex < 0)
					m_3DIndex = 0; // インデックスが負にならないようにする

				return;
			}
			++sprite; // 次のスプライトへ
		}
	}
}

void SpriteManager::ConvertTo3D() noexcept
{
	// 現在選択されている2Dスプライトを取得
	auto pSprite = m_SpritePointerList[_2D].begin();
	std::advance(pSprite, m_2DIndex);

	// 同じ名前のスプライトが3Dスプライトリストに存在しているか確認
	if (CheckConflictSpriteName((*pSprite)->GetName(), _3D))
	{
		// 既に同名のスプライトが存在する場合は変換しない
		MessageBoxA(nullptr, "変換先のリストに同名のスプライトが存在しています。", "エラー", MB_OK | MB_ICONERROR);
		return;
	}

	// 2Dスプライトのリストから、現在選択されているスプライトを探す
	for (auto &itr : m_Sprites[_2D])
	{
		// レイヤーごとにスプライトを検索
		for (auto sprite = itr.second.begin(); sprite != itr.second.end();)
		{
			// スプライトが現在選択されているスプライトと一致するか確認
			if ((*sprite) == (*pSprite))
			{
				// 2Dスプライトを3Dスプライトに変換
				(*sprite)->Set3D(true);
				(*sprite)->SetBillBoard(false); // ビルボードは無効化
				m_Sprites[_3D][(*sprite)->GetLayer()].push_back((*sprite)); // 2Dスプライトのリストに追加
				m_SpriteNames[_3D].push_back((*sprite)->GetName()); // スプライト名を追加
				m_SpritePointerList[_3D].push_back((*sprite)); // ポインタリストにも追加
				// 3Dスプライトから削除
				itr.second.erase(sprite);
				// スプライト名のリストからも削除
				m_SpriteNames[_2D].erase(std::next(m_SpriteNames[_2D].begin(), m_2DIndex));
				// スプライトポインタリストからも削除
				m_SpritePointerList[_2D].erase(std::next(m_SpritePointerList[_2D].begin(), m_2DIndex));

				--m_2DIndex; // インデックスを戻す
				if (m_2DIndex < 0)
					m_2DIndex = 0; // インデックスが負にならないようにする

				return;
			}
			++sprite; // 次のスプライトへ
		}
	}
}

void SpriteManager::SaveSprites() const noexcept
{
	json data;

	for (int i = 0; i < _MAX_RENDER_MODE; ++i)
	{
		std::string ModeName = (i == _2D) ? "2D" : "3D";
		std::string KeyName = ModeName + "_Sprites";
		std::string Name = ModeName + "_Sprite";

		for (auto &sprites : m_Sprites[i])
		{
			for (auto &sprite : sprites.second)
			{
				json work;
				work[Name]["Name"] = sprite->GetName();
				work[Name]["FilePath"] = sprite->GetFilePath();
				work[Name]["Position"] = { sprite->GetPosition().x, sprite->GetPosition().y, sprite->GetPosition().z };
				work[Name]["Scale"] = { sprite->GetScale().x, sprite->GetScale().y, sprite->GetScale().z };
				work[Name]["Rotation"] = { sprite->GetRotation().x, sprite->GetRotation().y, sprite->GetRotation().z };
				work[Name]["Layer"] = sprite->GetLayer();
				work[Name]["Is3D"] = sprite->GetIs3D();
				work[Name]["IsBillBoard"] = sprite->GetIsBillBoard();
				data[KeyName].push_back(work); // 2Dスプライトのデータを追加
			}
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
				Sprite *sprite = CreateSprite(name, filePath, is3D, isBillBoard, layer);
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
		auto itr = m_SpritePointerList[_2D].rbegin();
		for (int i = static_cast<int>(m_SpritePointerList[_2D].size()) - 1; i >= 0;--i,++itr)
		{
			if (*itr)
			{
				// スケールを取得
				float ScaleX = (*itr)->GetScale().x;
				float ScaleY = (*itr)->GetScale().y;

				// スプライトの中心座標を取得
				DirectX::XMFLOAT3 center = (*itr)->GetPosition();
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
		auto itr = m_SpritePointerList[_2D].begin();
		std::advance(itr, m_Selected2DSpriteIndex);
		if (*itr)
		{
			// スプライトの位置を取得
			DirectX::XMFLOAT3 pos = (*itr)->GetPosition();

			// スプライトの位置をカーソル移動に合わせて動かす
			pos.x = Input::GetMouseRelativePos_CenterZero().x - m_ClickPointOffsetX_2D; // カーソルのX座標からオフセットを引く
			// Y座標のオフセットはDirectX11の座標系に合わせてあるので、マウス座標のみ座標系を合わせる
			pos.y = (-Input::GetMouseRelativePos_CenterZero().y) - m_ClickPointOffsetY_2D; // カーソルのY座標からオフセットを引く

			pos.z = 0.0f; // 2DスプライトなのでZ座標は0に設定
			
			// スプライトの位置を更新
			(*itr)->SetPosition(pos);
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
		DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPos();
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
		auto itr = m_SpritePointerList[_3D].begin();
		std::advance(itr, m_Selected3DSpriteIndex);

		if (*itr)
		{
			// スプライトの位置を取得
			DirectX::XMFLOAT3 pos = (*itr)->GetPosition();

			// ------- 毎フレームのマウス座標更新処理 -------
			
			// マウス座標(スクリーン座標)をワールド座標に変換
			DirectX::XMVECTOR Mouse = ScreenToWorldPos(Input::GetMouseRelativePos(), 0.5f, m_pCamera->GetView(false), m_pCamera->GetProj(false));
			// カメラの位置を取得
			DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPos();
			DirectX::XMVECTOR vCam = DirectX::XMLoadFloat3(&CamPos);

			// カメラ座標からマウス座標へのベクトルを取得、方向に変換するために正規化
			DirectX::XMVECTOR vRayDir = DirectX::XMVectorSubtract(Mouse, vCam);
			vRayDir = DirectX::XMVector3Normalize(vRayDir);

			DirectX::XMFLOAT3 scale = (*itr)->GetScale() / 2.0f; // スプライトのスケールを取得
			scale.z = cx_fEpsilon; // Z軸のスケールは小さく設定(平面として扱う)
			DirectX::XMFLOAT4 qua = (*itr)->GetQuaternion(); // スプライトのクォータニオンを取得
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
	auto itr = m_SpritePointerList[_3D].begin();
	for (int i = 0;i < m_SpritePointerList[_3D].size();++i,++itr)
	{
		if (*itr == nullptr)
			continue;

		DirectX::XMFLOAT3 scale = (*itr)->GetScale() / 2.0f;
		DirectX::XMFLOAT4 qua = (*itr)->GetQuaternion();
		scale.z = cx_fEpsilon;
		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&(*itr)->GetPosition());
		DirectX::XMVECTOR vExtents = DirectX::XMLoadFloat3(&scale);
		DirectX::XMVECTOR vOrientation = DirectX::XMLoadFloat4(&qua);

		float fRayLength;

		bool result = IntersectRayPlane(In_vRayPos, In_vRayDir, (*itr)->GetPosition(), scale, (*itr)->GetQuaternion(), fRayLength);

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
				auto NowSprite = m_SpritePointerList[_3D].begin();
				std::advance(NowSprite, WorkIndex);
				DirectX::XMStoreFloat(&SpriteLength, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&(*NowSprite)->GetPosition()), In_vRayPos));
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
