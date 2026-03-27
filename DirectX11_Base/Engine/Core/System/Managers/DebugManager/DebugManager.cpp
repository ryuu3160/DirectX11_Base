/*+===================================================================
	File: DebugManager.cpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:25 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugManager.hpp"
#include "Core/System/Component/Camera/CameraDCC.hpp"
#include "Core/DirectX11/System/RenderManager.hpp"
#include "Core/System/Managers/SceneManager.hpp"
#include "ryuu_lib/FrameManager/FrameManager.hpp"
#include "Core/DirectX11/ResourceManager/TextureManager.hpp"
#include "Core/System/Project/GameProject.hpp"
#include "Core/System/Input/Input.hpp"
#include "ryuu_lib/WindowGenerator/Window.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	constexpr float cx_fToolBarHeight = 5.0f;
	static DebugWindow *c_NullWindow;
}

void DebugManager::Init()
{
	// データの読み込み
	LoadDebugData();

	// ファイルメニューの初期設定
	FileMenuInit();

	// 編集メニューの初期化
	EditMenuInit();

	// 表示メニューの初期化
	ViewMenuInit();

	AddMenuBar("Camera", "Editor", [this]()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraObj>("EditorCamera");
				if (!CamObj)
				{
					DebugLogError("EditorCameraが見つかりません。");
					return;
				}
				auto camera = CamObj->GetComponent<Camera>();
				auto context = RenderManager::GetInstance().GetRenderContext("Main");
				if (camera && context)
				{
					context->GetCamera()->SetActive(false);
					context->SwapCamera(camera);
					CamObj->SetActive(true);
				}
			}
		});
	AddMenuBar("Camera", "Game", [this]()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraObj>("GameCamera");
				if (!CamObj)
				{
					DebugLogError("GameCameraが見つかりません。");
					return;
				}
				auto camera = CamObj->GetComponent<Camera>();
				auto context = RenderManager::GetInstance().GetRenderContext("Main");
				if (camera && context)
				{
					context->GetCamera()->GetGameObject()->SetActive(false);
					context->SwapCamera(camera);
					CamObj->SetActive(true);
				}
			}
		});
}

void DebugManager::Update() noexcept
{
}

void DebugManager::Draw() noexcept
{
	if(ImGui::BeginMainMenuBar())
	{
		// ツールバーメニュー
		for(const auto &itr : m_MenuBarFuncs)
		{
			if(ImGui::BeginMenu(itr.first.c_str()))
			{
				for(const auto &menu : itr.second)
				{
					if(ImGui::MenuItem(menu.Name.c_str(), nullptr))
					{
						menu.Func();
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();
	}

	// デバッグウィンドウの描画
	for (const auto &window : m_DebugWindows)
	{
		if(window)
		{
			window->Draw();
		}
	}

	if (m_IsRequestSaveLayout)
	{
		ImGui::SaveIniSettingsToDisk("Engine/Assets/DebugResource/imgui_layout.ini");
		m_IsRequestSaveLayout = false;
	}
	if (m_IsRequestLoadLayout)
	{
		ImGui::LoadIniSettingsFromDisk("Engine/Assets/DebugResource/imgui_layout.ini");
		m_IsRequestLoadLayout = false;
	}
}

DebugWindow *DebugManager::CreateDebugWindow(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name, _In_ bool In_IsShowMenuBar, _In_ ImGuiWindowFlags In_Flags)
{
	auto itr = m_MenuBarFuncs.try_emplace(In_GroupName.data());

	// 既に存在チェック
	auto WinItr = std::find_if(m_DebugWindows.begin(), m_DebugWindows.end(),
		[&](DebugWindowBase *window)
		{
			return window->m_GroupName == In_GroupName && window->GetName() == In_Name;
		});

	if(WinItr != m_DebugWindows.end())
		return dynamic_cast<DebugWindow *>(*WinItr);

	DebugWindow *NewWindow = new DebugWindow(In_Name, In_Flags);
	NewWindow->m_GroupName = std::string(In_GroupName);

	std::string Path = In_GroupName.data();
	Path += "/";
	WindowDataRead(Path, NewWindow);

	m_DebugWindows.push_back(NewWindow);

	// メニュー登録
	if(In_IsShowMenuBar)
	{
		auto menu = m_MenuBarFuncs.try_emplace(In_GroupName.data());
		MenuBar NewMenu;
		NewMenu.Name = In_Name.data();
		NewMenu.Func = [NewWindow]() { NewWindow->ToggleIsOpen(); };
		menu.first->second.push_back(NewMenu);
	}

	return NewWindow;
}

DebugModal *DebugManager::CreateDebugModal(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name, _In_ bool In_IsShowMenuBar, _In_ ImGuiWindowFlags In_Flags)
{
	auto itr = m_MenuBarFuncs.try_emplace(In_GroupName.data());

	// 既に存在チェック
	auto WinItr = std::find_if(m_DebugWindows.begin(), m_DebugWindows.end(),
		[&](DebugWindowBase *window)
		{
			return window->m_GroupName == In_GroupName && window->GetName() == In_Name;
		});

	if(WinItr != m_DebugWindows.end())
		return dynamic_cast<DebugModal *>(*WinItr);

	DebugModal *NewModal = new DebugModal(In_Name, In_Flags);
	NewModal->m_GroupName = std::string(In_GroupName);

	std::string Path = In_GroupName.data();
	Path += "/";
	WindowDataRead(Path, NewModal);

	m_DebugWindows.push_back(NewModal);

	// メニュー登録(モーダルはOpen()を呼ぶ)
	if(In_IsShowMenuBar)
	{
		auto menu = m_MenuBarFuncs.try_emplace(In_GroupName.data());
		MenuBar NewMenu;
		NewMenu.Name = In_Name.data();
		NewMenu.Func = [NewModal]() { NewModal->Open(); };
		menu.first->second.push_back(NewMenu);
	}

	return NewModal;
}

DebugPopup *DebugManager::CreateDebugPopup(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name, _In_ bool In_IsShowMenuBar, _In_ ImGuiWindowFlags In_Flags)
{
	auto itr = m_MenuBarFuncs.try_emplace(In_GroupName.data());

	// 既に存在チェック
	auto WinItr = std::find_if(m_DebugWindows.begin(), m_DebugWindows.end(),
		[&](DebugWindowBase *window)
		{
			return window->m_GroupName == In_GroupName && window->GetName() == In_Name;
		});

	if(WinItr != m_DebugWindows.end())
		return dynamic_cast<DebugPopup *>(*WinItr);

	DebugPopup *NewPopup = new DebugPopup(In_Name, In_Flags);
	NewPopup->m_GroupName = std::string(In_GroupName);

	std::string Path = In_GroupName.data();
	Path += "/";
	WindowDataRead(Path, NewPopup);

	m_DebugWindows.push_back(NewPopup);

	// メニュー登録(ポップアップはOpen()を呼ぶ)
	if(In_IsShowMenuBar)
	{
		auto menu = m_MenuBarFuncs.try_emplace(In_GroupName.data());
		MenuBar NewMenu;
		NewMenu.Name = In_Name.data();
		NewMenu.Func = [NewPopup]() { NewPopup->Open(); };
		menu.first->second.push_back(NewMenu);
	}

	return NewPopup;
}

void DebugManager::AddMenuBar(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name, _In_ std::function<void()> In_Func)
{
	auto menu = m_MenuBarFuncs.try_emplace(In_GroupName.data());

	auto FindItr = std::find_if(menu.first->second.begin(), menu.first->second.end(),
		[&](const MenuBar &item)
		{
			return item.Name == In_Name;
		});
	if (FindItr != menu.first->second.end())
		return;

	MenuBar NewMenu;
	NewMenu.Name = In_Name.data();
	NewMenu.Func = In_Func;
	menu.first->second.push_back(NewMenu);
}

DebugWindowBase *DebugManager::GetDebugWindowBase(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name)
{
	auto itr = m_MenuBarFuncs.find(In_GroupName.data());

	if(itr != m_MenuBarFuncs.end())
	{
		for(const auto &window : m_DebugWindows)
		{
			if(window && window->m_GroupName == In_GroupName && window->GetName() == In_Name)
			{
				return window;
			}
		}
	}
	return c_NullWindow;
}

DebugWindowBase &DebugManager::GetDebugWindowBaseRef(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name)
{
	return *GetDebugWindowBase(In_GroupName, In_Name);
}

DebugWindow *DebugManager::GetDebugWindow(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name)
{
	return dynamic_cast<DebugWindow *>(GetDebugWindowBase(In_GroupName, In_Name));
}

DebugWindow &DebugManager::GetDebugWindowRef(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name)
{
	return *GetDebugWindow(In_GroupName, In_Name);
}

void DebugManager::DebugLog(_In_ std::string_view In_Msg)
{
	auto window = GetDebugWindow("View", "Log");
	if(window && window->NotDummy())
	{
		auto &item = (*window)["ConsoleLog"];
		if(item.GetKind() == DebugItem::Kind::Console)
		{
			auto TextItem = dynamic_cast<ItemConsole *>(&item);
			if(TextItem)
			{
				std::string Msg = Util::ShiftJISToUTF8(In_Msg);
				TextItem->AddOutput(Msg);
			}
		}
	}
}

void DebugManager::DebugLogWarning(_In_ std::string_view In_Msg)
{
	auto window = GetDebugWindow("View", "Log");
	if(window && window->NotDummy())
	{
		auto &item = (*window)["ConsoleLog"];
		if(item.GetKind() == DebugItem::Kind::Console)
		{
			auto TextItem = dynamic_cast<ItemConsole *>(&item);
			if(TextItem)
			{
				std::string Msg = Util::ShiftJISToUTF8(In_Msg);
				TextItem->AddOutput(Msg, ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "Warning");
			}
		}
	}
}

void DebugManager::DebugLogError(_In_ std::string_view In_Msg)
{
	auto window = GetDebugWindow("View", "Log");
	if(window && window->NotDummy())
	{
		auto &item = (*window)["ConsoleLog"];
		if(item.GetKind() == DebugItem::Kind::Console)
		{
			auto TextItem = dynamic_cast<ItemConsole *>(&item);
			if(TextItem)
			{
				std::string Msg = Util::ShiftJISToUTF8(In_Msg);
				TextItem->AddOutput(Msg, ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error");
			}
		}
	}
}

void DebugManager::HideAllWindows()
{
	for (const auto &window : m_DebugWindows)
	{
		if (window)
		{
			window->SetIsOpen(false);
		}
	}
}

void DebugManager::ShowAllWindows()
{
	for (const auto &window : m_DebugWindows)
	{
		if (window)
		{
			window->SetIsOpen(true);
		}
	}
}

DebugManager::DebugManager()
	: m_IsRequestLoadLayout(false), m_IsRequestSaveLayout(false)
{
	c_NullWindow = new DebugWindow("NullWindow");
	c_NullWindow->m_IsDummy = true;
}

DebugManager::~DebugManager()
{
	c_NullWindow->m_IsDummy = false;
	delete c_NullWindow;
	c_NullWindow = nullptr;

	// データの保存
	SaveDebugData();

	for (auto &itr : m_DebugWindows)
	{
		delete itr;
	}
	m_DebugWindows.clear();
	m_MenuBarFuncs.clear();
}

void DebugManager::ProjectWindowInit()
{
	auto ProjectWindow = CreateDebugWindow("View", "Project");
	auto Project = ProjectWindow->CreateItem<ItemProjectWindow>("ProjectWindow");

	// プロジェクトウィンドウの設定
	// フォルダアイコンを読み込み
	auto FolderTexture = TextureManager::GetInstance().LoadTexture("Engine/Assets/Icons/folder.png");
	Project->SetDefaultFolderIcon(FolderTexture);

	// ファイルアイコンを読み込み
	auto FileTexture = TextureManager::GetInstance().LoadTexture("Engine/Assets/Icons/file.png");
	Project->SetDefaultFileIcon(FileTexture);

	// 拡張子ごとのアイコン
	auto ImageTexture = TextureManager::GetInstance().LoadTexture("Engine/Assets/Icons/image.png");
	Project->RegisterIcon(".png", ImageTexture);
	Project->RegisterIcon(".jpg", ImageTexture);
	Project->RegisterIcon(".jpeg", ImageTexture);

	auto ScriptTexture = TextureManager::GetInstance().LoadTexture("Engine/Assets/Icons/script.png");
	Project->RegisterIcon(".cpp", ScriptTexture);
	Project->RegisterIcon(".h", ScriptTexture);
	Project->RegisterIcon(".hpp", ScriptTexture);

	auto CponTexture = TextureManager::GetInstance().LoadTexture("Engine/Assets/Icons/cpon.png");
	Project->RegisterIcon(".cpon", CponTexture);

	// ファイル選択時のコールバック
	Project->SetFileSelectedCallback([](const std::string &path)
		{
			DebugManager::GetInstance().DebugLog("File selected: {}", path);
		});
}

void DebugManager::FileMenuInit()
{
	// 新規プロジェクト作成ウィンドウ
	auto NewProjectWindow = CreateDebugModal("File", "NewProject", true, ImGuiWindowFlags_NoResize);
	NewProjectWindow->CreateItem<ItemNewProject>("NewProject");
	NewProjectWindow->SetShowDefaultButtons(false);
	NewProjectWindow->SetOpenCallback([]()
		{
			// 入力を無効化
			Input::SetInputEnabled(false);
			// ダイアログのサイズと位置
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400, 180), ImGuiCond_Appearing);
		});
	NewProjectWindow->SetCloseCallback([]()
		{
			// 入力を有効化
			Input::SetInputEnabled(true);
		});

	ImGuiWindowFlags PopupFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	auto ConfirmSwitchProject = CreateDebugModal("File", "ConfirmSwitchProject", false, PopupFlags);
	ConfirmSwitchProject->CreateItem<ItemText>("Switch project?");
	ConfirmSwitchProject->CreateItem<ItemSpacing>();
	ConfirmSwitchProject->CreateItem<ItemText>("The current project will be closed and a different project will be opened.", true);
	ConfirmSwitchProject->CreateItem<ItemSpacing>();
	ConfirmSwitchProject->CreateItem<ItemText>("Target:");
	ConfirmSwitchProject->CreateItem<ItemSameLine>();
	auto TargetPathText = ConfirmSwitchProject->CreateItem<ItemText>("Path", true);
	ConfirmSwitchProject->CreateItem<ItemSpacing>();
	ConfirmSwitchProject->CreateItem<ItemSeparator>();
	auto IsNoAsk = ConfirmSwitchProject->CreateItem<ItemValue>("Don't ask again", DebugItem::Kind::Bool);

	AddMenuBar("File", "Open Project...", [ConfirmSwitchProject,TargetPathText, IsNoAsk]()
		{
			bool IsNoAskValue = IsNoAsk->GetValue<bool>();

			if(!IsNoAskValue)
			{
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ConfirmSwitchProject->SetOpenCallback([center]()
					{
						// ダイアログの位置
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					});
			}

			HWND owner = Window::GetInstance().GetHwnd();

			auto picked = Util::PickFileUTF8(
				owner,
				"Open Project",
				"Projects",
				"Visual Studio Solution (*.sln)|*.sln||All Files (*.*)|*.*",
				"sln"
			);

			if(!picked)
				return;

			const std::string path = *picked;

			// 既にプロジェクトが開いていないなら、確認不要で即オープン
			if(!GameProjectManager::GetInstance().IsProjectLoaded())
			{
				if(!GameProjectManager::GetInstance().OpenProject(path))
					DebugManager::GetInstance().DebugLogError("Failed to open project: {}", path);
				return;
			}

			// 「次回から確認しない」がオンなら、即オープン
			if(IsNoAskValue)
			{
				if(!GameProjectManager::GetInstance().OpenProject(path))
					DebugManager::GetInstance().DebugLogError("Failed to open project: {}", path);
				return;
			}

			// 確認ダイアログを表示
			TargetPathText->SetText(path);
			ConfirmSwitchProject->SetOkCallback([path]()
				{
					if(!GameProjectManager::GetInstance().OpenProject(path))
						DebugManager::GetInstance().DebugLogError("Failed to open project: {}", path);
				});
			ConfirmSwitchProject->SetCancelCallback([IsNoAsk]()
				{
					IsNoAsk->GetValue<bool>() = false;
				});
			ConfirmSwitchProject->Open();
		});

	// 終了ボタン
	AddMenuBar("File", "Exit", []()
		{
			PostQuitMessage(0);
		});
}

void DebugManager::EditMenuInit()
{
	// ツールバーメニューの初期設定
	AddMenuBar("Edit", "Show All Debug Windows", [this]()
		{
			ShowAllWindows();
		});
	AddMenuBar("Edit", "Hide All Debug Windows", [this]()
		{
			HideAllWindows();
		});
	AddMenuBar("Edit", "Reset ImGui Layout", [this]()
		{
			m_IsRequestLoadLayout = true;
		});
	AddMenuBar("Edit", "Save ImGui Layout", [this]()
		{
			m_IsRequestSaveLayout = true;
		});
}

void DebugManager::ViewMenuInit()
{
	// フレームレート表示ウィンドウ
	CreateDebugWindow("View", "Hierarchy");
	CreateDebugWindow("View", "Inspector");

	// プロジェクトウィンドウの設定
	ProjectWindowInit();

	auto log = CreateDebugWindow("View", "Log");
	auto DebugMenu = CreateDebugWindow("View", "DebugMenu");

	// ログウィンドウの初期設定
	auto Output = log->CreateItem<ItemConsole>("ConsoleLog", true);
	Output->AddLevel("Warning", { 1.0f,0.5f,0.0f,1.0f });
	Output->AddLevel("Error", { 1.0f,0.1f,0.1f,1.0f });

	// デバッグメニューの初期設定
	DebugMenu->CreateItem<ItemCallback>("Pause", DebugItem::Kind::Bool,
		[](bool IsSet, void *ptr)
		{
			if(!IsSet)
				return;

			bool *IsPause = reinterpret_cast<bool *>(ptr);
			if(*IsPause)
			{
				FrameManager::GetInstance().SetTimeScale(0.0f);
			}
			else
			{
				FrameManager::GetInstance().SetTimeScale(1.0f);
			}
		});
	DebugMenu->CreateItem<ItemSameLine>();
	auto IsDrawGizmoCallback = DebugMenu->CreateItem<ItemCallback>("IsDrawGizmo", DebugItem::Kind::Bool,
		[](bool IsSet, void *ptr)
		{
			if(!IsSet)
				return;
			bool *IsDrawGizmo = reinterpret_cast<bool *>(ptr);
			RenderManager::GetInstance().SetDrawGizmos(*IsDrawGizmo);
		});
	IsDrawGizmoCallback->GetValue() = RenderManager::GetInstance().IsDrawGizmos();

	auto ScreenColor = DebugMenu->CreateItem<ItemValue>("ScreenColor", DebugItem::Kind::Color, true);
	DX11_Core::GetInstance().SetWindowColor(ScreenColor->GetColor());
	ScreenColor->SetNoticeFunc([]()
		{
			auto &DX11Core = DX11_Core::GetInstance();
			auto color = DebugManager::GetInstance().GetDebugWindowRef("View", "DebugMenu")["ScreenColor"].GetColor();
			DX11Core.SetWindowColor(color);
		});
}

void DebugManager::SaveDebugData()
{
	std::fstream file("Engine\\Assets\\DebugResource\\DebugManagerData.csv", std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		std::filesystem::create_directory("Engine\\Assets\\DebugResource\\");
		file.open("Engine\\Assets\\DebugResource\\DebugManagerData.csv", std::ios::out | std::ios::trunc);
	}

	if (file.is_open())
	{
		std::string data;

		for (auto &window : m_DebugWindows)
		{
			std::string path = window->m_GroupName + "/";

			WindowDataWrite(data, path, window);

			path += window->GetName() + "/";

			for (auto &item : window->m_Items)
			{
				DataWrite(data, path, item);
			}

			delete window;
		}
		m_DebugWindows.clear();

		file << data;

		file.close();
	}
}

void DebugManager::DataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string_view In_Path, _In_ DebugItem *In_Item)
{
	std::string Path = In_Path.data();

	if (In_Item->GetKind() == DebugItem::Kind::Group)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(In_Item);
		if (!group)
			return;
		Path += group->GetName() + "/";

		for (auto &item : group->GetGroupItems())
		{
			DataWrite(Inout_Data, Path, item);
		}

		return;
	}

	// 保存フラグが立っていれば保存する
	ItemValue *pValue = dynamic_cast<ItemValue *>(In_Item);
	ItemList *pList = nullptr;
	ItemInputText *pInputText = nullptr;
	ItemProjectWindow *pProject = nullptr;

	if(!pValue)
		pList = dynamic_cast<ItemList *>(In_Item);

	if (!pValue && !pList)
		pInputText = dynamic_cast<ItemInputText *>(In_Item);

	if(!pValue && !pList && !pInputText)
		pProject = dynamic_cast<ItemProjectWindow *>(In_Item);

	if(!pValue && !pList && !pInputText && !pProject)
		return;

	if(!((pValue && pValue->IsSave()) || (pList && pList->IsSave()) || (pInputText && pInputText->IsSave()) || pProject))
		return;

	// 種類保存
	Inout_Data += DebugItem::KindToStr(In_Item->GetKind()) + ",";
	// パス保存
	Inout_Data += Path + In_Item->GetName() + ",";
	// 値保存
	switch (In_Item->GetKind())
	{
	default:
		Inout_Data += "0";
		break;
	case DebugItem::Bool:
		if(pValue)
			Inout_Data += pValue->GetValue<bool>() ? "1" : "0";
		break;
	case DebugItem::Int:
		if (pValue)
		{
			std::string ValueStr;
			ValueStr = ToString(pValue->GetValue<int>());
			Inout_Data += ValueStr;
		}
		break;
	case DebugItem::Float:
		if (pValue)
		{
			std::string ValueStr;
			ValueStr = ToString(pValue->GetValue<float>());
			Inout_Data += ValueStr;
		}
		break;
	case DebugItem::Float2:
		if(pValue)
		{
			std::string ValueStrX;
			std::string ValueStrY;
			DirectX::XMFLOAT2 vec2 = pValue->GetValue<DirectX::XMFLOAT2>();
			ValueStrX = ToString(vec2.x);
			ValueStrY = ToString(vec2.y);
			Inout_Data += ValueStrX + "/" + ValueStrY;
		}
		break;
	case DebugItem::Color:
		if (pValue)
		{
			std::string ValueStrX;
			std::string ValueStrY;
			std::string ValueStrZ;
			std::string ValueStrW;
			DirectX::XMFLOAT4 color = pValue->GetValue<DirectX::XMFLOAT4>();
			ValueStrX = ToString(color.x,10);
			ValueStrY = ToString(color.y,10);
			ValueStrZ = ToString(color.z,10);
			ValueStrW = ToString(color.w,10);
			Inout_Data += ValueStrX + "/" + ValueStrY + "/" + ValueStrZ + "/" + ValueStrW;
		}
		break;
	case DebugItem::Path:
		if (pValue)
			Inout_Data += pValue->GetValue<std::string>();
		break;
	case DebugItem::InputStr:
		if(pInputText)
			Inout_Data += pInputText->GetText();
		break;
	case DebugItem::List:
		if (pList)
		{
			std::string ValueStr;
			ValueStr = ToString(pList->GetSelectNo());
			Inout_Data += ValueStr;
		}
		break;
	case DebugItem::Vector:
		if (pValue)
		{
			std::string ValueStrX;
			std::string ValueStrY;
			std::string ValueStrZ;
			DirectX::XMFLOAT3 vec = pValue->GetValue<DirectX::XMFLOAT3>();
			ValueStrX = ToString(vec.x);
			ValueStrY = ToString(vec.y);
			ValueStrZ = ToString(vec.z);
			Inout_Data += ValueStrX + "/" + ValueStrY + "/" + ValueStrZ;
		}
		break;

	case DebugItem::__ProjectWindow:
		if (pProject)
		{
			std::string IconSize,ProjectName, CurrentProjectPath;
			IconSize = ToString(pProject->GetIconSize());
			ProjectName = pProject->GetProjectName();
			CurrentProjectPath = pProject->GetRootPath();
			Inout_Data += IconSize + "^" + ProjectName + "^" + CurrentProjectPath;
		}
		break;
	}
	Inout_Data += "\n";
}

void DebugManager::WindowDataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string_view In_Path, _In_ DebugWindowBase *In_Window)
{
	if (!In_Window)
		return;

	DebugWindow *pWindow = dynamic_cast<DebugWindow *>(In_Window);
	DebugModal *pModal = nullptr;
	DebugPopup *pPopup = nullptr;

	if(!pWindow)
		pModal = dynamic_cast<DebugModal *>(In_Window);
	if(!pWindow && !pModal)
		pPopup = dynamic_cast<DebugPopup *>(In_Window);
	if(!pWindow && !pModal && !pPopup)
		return;

	if(pWindow)
	{
		Inout_Data += "Window,";
		Inout_Data += In_Path.data() + pWindow->GetName() + ",";
		Inout_Data += pWindow->IsOpen() ? "1" : "0";
		Inout_Data += "\n";
	}
	else if(pModal)
	{
		//Inout_Data += "Modal,";
		//Inout_Data += In_Path.data() + pModal->GetName();
		//Inout_Data += "\n";
	}
	else if(pPopup)
	{
		//Inout_Data += "Popup,";
		//Inout_Data += In_Path.data() + pPopup->GetName();
		//Inout_Data += "\n";
	}
}

void DebugManager::LoadDebugData()
{
	std::fstream file("Engine\\Assets\\DebugResource\\DebugManagerData.csv", std::ios::in);

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			std::vector<std::string> cells;
			size_t start = 0;
			size_t end = line.find(',');
			for (; end > start;)
			{
				cells.push_back(line.substr(start, end - start));
				start = end + 1;
				end = line.find(',', start);

				if (end == std::string::npos)
					end = line.length();
			}
			SaveData data;
			data.kind = DebugItem::StrToKind(cells[0]);
			data.path = cells[1];
			data.value = cells[2];
			m_SaveData.push_back(data);
		}
		file.close();
	}
}

void DebugManager::WindowDataRead(_In_ std::string_view In_Path, _Inout_ DebugWindowBase *Inout_Window)
{
	if (!Inout_Window)
		return;

	DebugWindow *pWindow = dynamic_cast<DebugWindow *>(Inout_Window);
	DebugModal *pModal = nullptr;
	DebugPopup *pPopup = nullptr;

	if(!pWindow)
		pModal = dynamic_cast<DebugModal *>(Inout_Window);
	if(!pWindow && !pModal)
		pPopup = dynamic_cast<DebugPopup *>(Inout_Window);
	if(!pWindow && !pModal && !pPopup)
		return;

	std::string Path = In_Path.data();
	Path += Inout_Window->GetName();
	auto DataItr = std::find_if(m_SaveData.begin(), m_SaveData.end(),
		[&Path](const SaveData &data)
		{
			return (data.path == Path);
		});
	if (DataItr == m_SaveData.end())
		return;

	if(pWindow)
	{
		pWindow->SetIsOpen(FromString<int>(DataItr->value) > 0);
	}
	else if(pModal)
	{
	}
	else if(pPopup)
	{
	}
}

std::string DebugManager::CharacterLimitRecursion(_In_ std::string_view In_Text, _In_ int In_LimitNum)
{
	if (In_Text.length() >= In_LimitNum)
	{
		std::string text = In_Text.data();
		text = text.substr(In_Text.find('\n') + 1);
		return CharacterLimitRecursion(text, In_LimitNum);
	}
	return In_Text.data();
}

void DebugManager::DataRead(_In_ std::string_view In_Path, _Inout_ DebugItem *Inout_Item)
{
	std::string Path = In_Path.data();

	if (Inout_Item->GetKind() == DebugItem::Kind::Group)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(Inout_Item);
		if (!group)
			return;
		Path += group->GetName() + "/";

		for(auto &item : group->GetGroupItems())
		{
			DataRead(Path, item);
		}
		return;
	}

	// 保存フラグの確認
	ItemValue *pValue = dynamic_cast<ItemValue *>(Inout_Item);
	ItemList *pList = nullptr;
	ItemInputText *pInputText = nullptr;
	ItemProjectWindow *pProject = nullptr;

	if(!pValue)
		pList = dynamic_cast<ItemList *>(Inout_Item);
	if (!pValue && !pList)
		pInputText = dynamic_cast<ItemInputText *>(Inout_Item);
	if(!pValue && !pList && !pInputText)
		pProject = dynamic_cast<ItemProjectWindow *>(Inout_Item);
	if(!pValue && !pList && !pInputText && !pProject)
		return;

	Path += Inout_Item->GetName();

	auto DataItr = std::find_if(m_SaveData.begin(), m_SaveData.end(),
		[&Path](const SaveData &data)
		{
			return (data.path == Path);
		});

	if(DataItr == m_SaveData.end())
		return;

	switch (Inout_Item->GetKind())
	{
	case DebugItem::Bool:
		pValue->GetValue() = FromString<int>(DataItr->value) > 0;
		break;
	case DebugItem::Int:
		pValue->GetValue() = FromString<int>(DataItr->value);
		break;
	case DebugItem::Float:
		pValue->GetValue() = FromString<float>(DataItr->value);
		break;
	case DebugItem::Float2:
		{
		std::string value = DataItr->value;

		if(value.empty())
			return;

		std::string elem = value;
		DirectX::XMFLOAT2 vec2;
		vec2.x = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		vec2.y = FromString<float>(elem);
		pValue->GetValue() = vec2;
	} break;
	case DebugItem::Color:
	{
		std::string value = DataItr->value;

		if(value.empty())
			return;

		std::string elem = value;
		DirectX::XMFLOAT4 color;
		color.x = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		color.y = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		color.z = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		color.w = FromString<float>(elem);
		pValue->GetValue() = color;
	} break;
	case DebugItem::Path:
		pValue->GetValue() = DataItr->value;
		break;
	case DebugItem::InputStr:
	{
		if (pInputText)
		{
			pInputText->GetText() = DataItr->value;
		}
	}
	break;
	case DebugItem::List:
		pList->GetSelectNo() = atoi(DataItr->value.c_str());
		if (pList->GetFunc())
		{
			auto it = pList->GetList().begin();
			for (int i = 0; i < pList->GetSelectNo(); ++i)
				it++;
			pList->GetFunc()(it->c_str());
		}
		break;
	case DebugItem::Vector:
	{
		std::string value = DataItr->value;
		if(value.empty())
			return;
		std::string elem = value;
		DirectX::XMFLOAT3 vec;
		vec.x = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		vec.y = FromString<float>(elem.erase(elem.find('/')));
		value.erase(0, value.find('/') + 1);
		elem = value;
		vec.z = FromString<float>(elem);
		pValue->GetValue() = vec;
	}
	break;
	case DebugItem::__ProjectWindow:
	{
		std::string value = DataItr->value;
		if(value.empty())
			return;
		std::string elem = value;
		float IconSize = FromString<float>(elem.erase(elem.find('^')));
		value.erase(0, value.find('^') + 1);
		elem = value;
		std::string ProjectName = elem.erase(elem.find('^'));
		value.erase(0, value.find('^') + 1);
		elem = value;
		std::string RootPath = elem;
		pProject->SetIconSize(IconSize);
		pProject->SetProjectName(ProjectName);
		pProject->SetRootPath(RootPath);
	}
	break;
	}
}
