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
#include "System/Object/CameraDCC.hpp"
#include "DirectX11/System/RenderManager.hpp"
#include "System/ryuu_lib/FrameManager/FrameManager.hpp"

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
	m_ToolBarFlags |= ImGuiWindowFlags_MenuBar;
	m_ToolBarFlags |= ImGuiWindowFlags_NoCollapse;
	m_ToolBarFlags |= ImGuiWindowFlags_NoMove;
	m_ToolBarFlags |= ImGuiWindowFlags_NoResize;
	m_ToolBarFlags |= ImGuiWindowFlags_NoTitleBar;
	m_ToolBarFlags |= ImGuiWindowFlags_NoScrollbar;
	m_ToolBarFlags |= ImGuiWindowFlags_NoDocking;
	m_ToolBarFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	// データの読み込み
	LoadDebugData();

	// デフォルトウィンドウの作成
	auto log = CreateDebugWindow("System", "Log");
	auto DebugMenu = CreateDebugWindow("System", "DebugMenu");
	// フレームレート表示ウィンドウ
	CreateDebugWindow("System", "Hierarchy");
	CreateDebugWindow("System", "Inspector");
	// ツールバーメニューの初期設定
	AddToolBarMenu("System", "Show All Debug Windows", [this]()
		{
			ShowAllWindows();
		});
	AddToolBarMenu("System", "Hide All Debug Windows", [this]()
		{
			HideAllWindows();
		});
	AddToolBarMenu("System", "Reset ImGui Layout", [this]()
		{
			m_IsRequestLoadLayout = true;
		});
	AddToolBarMenu("System", "Save ImGui Layout", [this]()
		{
			m_IsRequestSaveLayout = true;
		});

	// ログウィンドウの初期設定
	auto Output = log->CreateItem<ItemConsole>("ConsoleLog",true);
	Output->AddLevel("Warning", { 1.0f,0.5f,0.0f,1.0f });
	Output->AddLevel("Error",{1.0f,0.1f,0.1f,1.0f});

	AddToolBarMenu("Camera", "Editor", [this]()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraDCC>("EditorCamera");
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
	AddToolBarMenu("Camera", "Game", [this]()
		{
			auto scene = SceneManager::GetInstance().GetCurrentScene();
			if (scene)
			{
				auto CamObj = scene->GetObject<CameraBaseObj>("GameCamera");
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

	// デバッグメニューの初期設定
	DebugMenu->CreateItem<ItemCallback>("Pause", DebugItem::Kind::Bool,
		[](bool IsSet, void *ptr)
		{
			if (IsSet)
			{
				FrameManager::GetInstance().SetTimeScale(0.0f);
			}
			else
			{
				FrameManager::GetInstance().SetTimeScale(1.0f);
			}
		});
}

void DebugManager::Update() noexcept
{
}

void DebugManager::Draw() noexcept
{
	ImGuiViewport *vp = ImGui::GetMainViewport();
	ImVec2 pos = ImVec2(vp->WorkPos.x, vp->WorkPos.y);
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(cx_nWINDOW_WIDTH, cx_fToolBarHeight));
	if (ImGui::Begin("ToolBar",nullptr, m_ToolBarFlags))
	{
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
		if (ImGui::BeginMenuBar())
		{
			for (const auto &itr : m_ToolBarFuncs)
			{
				if (ImGui::BeginMenu(itr.first.c_str()))
				{
					for (const auto &menu : itr.second)
					{
						if (ImGui::MenuItem(menu.Name.c_str(), nullptr))
						{
							menu.Func();
						}
					}
					ImGui::EndMenu();
				}
			}

			ImGui::EndMenuBar();
		}
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();
	}
	ImGui::End();

	for (const auto &window : m_DebugWindows)
	{
		if (!window)
			continue;

		if (window->IsOpen())
		{
			std::string name = window->GetGroupName() + "/" + window->GetName();
			ImGui::Begin(name.c_str());
			window->Draw();
			ImGui::End();
		}
	}

	if (m_IsRequestSaveLayout)
	{
		ImGui::SaveIniSettingsToDisk("Assets/DebugResource/imgui_layout.ini");
		m_IsRequestSaveLayout = false;
	}
	if (m_IsRequestLoadLayout)
	{
		ImGui::LoadIniSettingsFromDisk("Assets/DebugResource/imgui_layout.ini");
		m_IsRequestLoadLayout = false;
	}
}

DebugWindow *DebugManager::CreateDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name)
{
	auto itr = m_ToolBarFuncs.try_emplace(In_GroupName.data());

	auto WinItr = std::find_if(m_DebugWindows.begin(), m_DebugWindows.end(),
		[&](DebugWindow *window)
		{
			return window->m_GroupName == In_GroupName && window->GetName() == In_Name;
		});

	if (WinItr != m_DebugWindows.end())
		return *WinItr;

	DebugWindow *NewWindow = new DebugWindow(In_Name);
	NewWindow->m_GroupName = std::string(In_GroupName);
	std::string Path = In_GroupName.data();
	Path += "/";
	WindowDataRead(Path, NewWindow);

	m_DebugWindows.push_back(NewWindow);

	auto menu = m_ToolBarFuncs.try_emplace(In_GroupName.data());
	ToolBarMenu NewMenu;
	NewMenu.Name = In_Name.data();
	NewMenu.Func = [NewWindow]() { NewWindow->ToggleIsOpen(); };
	menu.first->second.push_back(NewMenu);

	return NewWindow;
}

void DebugManager::AddToolBarMenu(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name, _In_ std::function<void()> In_Func)
{
	auto menu = m_ToolBarFuncs.try_emplace(In_GroupName.data());

	auto FindItr = std::find_if(menu.first->second.begin(), menu.first->second.end(),
		[&](const ToolBarMenu &item)
		{
			return item.Name == In_Name;
		});
	if (FindItr != menu.first->second.end())
		return;

	ToolBarMenu NewMenu;
	NewMenu.Name = In_Name.data();
	NewMenu.Func = In_Func;
	menu.first->second.push_back(NewMenu);
}

DebugWindow *DebugManager::GetDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name)
{
	auto itr = m_ToolBarFuncs.find(In_GroupName.data());

	if (itr != m_ToolBarFuncs.end())
	{
		for (const auto &window : m_DebugWindows)
		{
			if (window && window->m_GroupName == In_GroupName && window->GetName() == In_Name)
			{
				return window;
			}
		}
	}
	return c_NullWindow;
}

DebugManager::DebugManager()
	: m_ToolBarFlags(0), m_IsRequestLoadLayout(false), m_IsRequestSaveLayout(false)
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
	m_ToolBarFuncs.clear();
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


void DebugManager::SaveDebugData()
{
	std::fstream file("Assets\\DebugResource\\DebugManagerData.csv", std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		std::filesystem::create_directory("Assets\\DebugResource\\");
		file.open("Assets\\DebugResource\\DebugManagerData.csv", std::ios::out | std::ios::trunc);
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

void DebugManager::DataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugItem *In_Item)
{
	if (In_Item->GetKind() == DebugItem::Kind::Group)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(In_Item);
		if (!group)
			return;

		In_Path += group->GetName() + "/";

		for (auto &item : group->GetGroupItems())
		{
			DataWrite(Inout_Data, In_Path, item);
		}

		return;
	}

	// 保存フラグが立っていれば保存する
	ItemValue *pValue = dynamic_cast<ItemValue *>(In_Item);
	ItemList *pList = nullptr;
	ItemText *pText = nullptr;

	if(!pValue)
		pList = dynamic_cast<ItemList *>(In_Item);

	if (!pValue && !pList)
		pText = dynamic_cast<ItemText *>(In_Item);

	if(!pValue && !pList && !pText)
		return;

	if(!((pValue && pValue->IsSave()) || (pList && pList->IsSave()) || (pText && pText->IsSave())))
		return;

	// 種類保存
	Inout_Data += DebugItem::KindToStr(In_Item->GetKind()) + ",";
	// パス保存
	Inout_Data += In_Path + In_Item->GetName() + ",";
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
			ValueStrX = ToString(color.x);
			ValueStrY = ToString(color.y);
			ValueStrZ = ToString(color.z);
			ValueStrW = ToString(color.w);
			Inout_Data += ValueStrX + "/" + ValueStrY + "/" + ValueStrZ + "/" + ValueStrW;
		}
		break;
	case DebugItem::Path:
		if (pValue)
			Inout_Data += pValue->GetValue<std::string>();
		break;
	case DebugItem::InputStr:
		if(pText)
			Inout_Data += pText->GetText();
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
	}
	Inout_Data += "\n";
}

void DebugManager::WindowDataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugWindow *In_Window)
{
	if (!In_Window)
		return;

	Inout_Data += "Window,";
	Inout_Data += In_Path + In_Window->GetName() + ",";
	Inout_Data += In_Window->IsOpen() ? "1" : "0";
	Inout_Data += "\n";
}

void DebugManager::LoadDebugData()
{
	std::fstream file("Assets\\DebugResource\\DebugManagerData.csv", std::ios::in);

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

void DebugManager::WindowDataRead(_In_ std::string In_Path, _Inout_ DebugWindow *Inout_Window)
{
	if (!Inout_Window)
		return;
	In_Path += Inout_Window->GetName();
	auto DataItr = std::find_if(m_SaveData.begin(), m_SaveData.end(),
		[&In_Path](const SaveData &data)
		{
			return (data.path == In_Path);
		});
	if (DataItr == m_SaveData.end())
		return;
	Inout_Window->SetIsOpen(atoi(DataItr->value.c_str()) > 0);
}

std::string DebugManager::CharacterLimitRecursion(_In_ std::string In_Text, _In_ int In_LimitNum)
{
	if (In_Text.length() >= In_LimitNum)
	{
		std::string text;
		text = In_Text.substr(In_Text.find('\n') + 1);
		return CharacterLimitRecursion(text, In_LimitNum);
	}
	return In_Text;
}

void DebugManager::DataRead(_In_ std::string In_Path, _Inout_ DebugItem *Inout_Item)
{
	if (Inout_Item->GetKind() == DebugItem::Kind::Group)
	{
		ItemGroup *group = dynamic_cast<ItemGroup *>(Inout_Item);
		if (!group)
			return;
		In_Path += group->GetName() + "/";

		for(auto &item : group->GetGroupItems())
		{
			DataRead(In_Path, item);
		}
		return;
	}

	// 保存フラグの確認
	ItemValue *pValue = dynamic_cast<ItemValue *>(Inout_Item);
	ItemList *pList = nullptr;

	if(!pValue)
		pList = dynamic_cast<ItemList *>(Inout_Item);
	if (!pValue && !pList)
		return;

	In_Path += Inout_Item->GetName();

	auto DataItr = std::find_if(m_SaveData.begin(), m_SaveData.end(),
		[&In_Path](const SaveData &data)
		{
			return (data.path == In_Path);
		});

	if(DataItr == m_SaveData.end())
		return;

	switch (Inout_Item->GetKind())
	{
	case DebugItem::Bool:
		pValue->GetValue() = std::atoi(DataItr->value.c_str()) > 0;
		break;
	case DebugItem::Int:
		pValue->GetValue() = atoi(DataItr->value.c_str());
		break;
	case DebugItem::Float:
		pValue->GetValue() = strtof(DataItr->value.c_str(), nullptr);
		break;
	case DebugItem::Float2:
		{
		const char *top[2];
		top[0] = DataItr->value.c_str();
		const char* delim1 = strstr(top[0], "/");
		top[1] = (delim1 != nullptr) ? delim1 + 1 : top[0]; // 区切り文字が見つからない場合はtop[0]を再利用

		pValue->GetValue() = DirectX::XMFLOAT2(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr)
		);
	} break;
	case DebugItem::Color:
	{
		const char *top[4];
		top[0] = DataItr->value.c_str();
		const char* delim2 = strstr(top[0], "/");
		top[1] = (delim2 != nullptr) ? delim2 + 1 : top[0];
		const char* delim3 = strstr(top[1], "/");
		top[2] = (delim3 != nullptr) ? delim3 + 1 : top[1];
		const char* delim4 = strstr(top[2], "/");
		top[3] = (delim4 != nullptr) ? delim4 + 1 : top[2];

		pValue->GetValue() = DirectX::XMFLOAT4(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr),
			strtof(top[2], nullptr),
			strtof(top[3], nullptr)
		);
	} break;
	case DebugItem::Path:
		pValue->GetValue() = DataItr->value;
		break;
	case DebugItem::InputStr:
	{
		ItemText *pText = dynamic_cast<ItemText *>(Inout_Item);
		if (pText)
		{
			pText->GetText() = DataItr->value;
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
		const char *top[3];
		top[0] = DataItr->value.c_str();
		const char* delim5 = strstr(top[0], "/");
		top[1] = (delim5 != nullptr) ? delim5 + 1 : top[0];
		const char* delim6 = strstr(top[1], "/");
		top[2] = (delim6 != nullptr) ? delim6 + 1 : top[1];

		pValue->GetValue() = DirectX::XMFLOAT3(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr),
			strtof(top[2], nullptr)
		);
	}
	break;
	}
}
