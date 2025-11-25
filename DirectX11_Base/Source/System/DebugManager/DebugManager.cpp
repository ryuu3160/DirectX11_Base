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

// ==============================
//	定数定義
// ==============================
namespace
{
	constexpr float cx_fTitleBarHeight = 80.0f;
	constexpr float cx_fToolBarHeight = 15.0f;
	static DebugWindow *c_NullWindow;
}

DebugManager::DebugManager()
	: m_ToolBarFlags(0)
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

void DebugManager::Init()
{
	m_ToolBarFlags |= ImGuiWindowFlags_MenuBar;
	m_ToolBarFlags |= ImGuiWindowFlags_NoCollapse;
	m_ToolBarFlags |= ImGuiWindowFlags_NoMove;
	m_ToolBarFlags |= ImGuiWindowFlags_NoResize;

	// データの読み込み
	LoadDebugData();
}

void DebugManager::Update() noexcept
{
}

void DebugManager::Draw() noexcept
{
	ImGuiViewport *vp = ImGui::GetMainViewport();
	ImVec2 pos = ImVec2(vp->WorkPos.x, vp->WorkPos.y - cx_fTitleBarHeight);
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(cx_nWINDOW_WIDTH, cx_fToolBarHeight), ImGuiCond_Always);
	if (ImGui::Begin("ToolBar",nullptr, m_ToolBarFlags))
	{
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
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

void DebugManager::SaveDebugData()
{
	std::fstream file("Assets\\Debug\\DebugManagerData.csv", std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		std::filesystem::create_directory("Assets\\Debug\\");
		file.open("Assets\\Debug\\DebugManagerData.csv", std::ios::out | std::ios::trunc);
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

	if(!pValue)
		pList = dynamic_cast<ItemList *>(In_Item);

	if (!pValue && !pList)
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
			Inout_Data += std::to_string(pValue->GetValue<int>());
		break;
	case DebugItem::Float:
		if (pValue)
			Inout_Data += std::to_string(pValue->GetValue<float>());
		break;
	case DebugItem::Float2:
		if(pValue)
		{
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT2>().x) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT2>().y);
		}
		break;
	case DebugItem::Color:
		if (pValue)
		{
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT4>().x) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT4>().y) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT4>().z) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT4>().w);
		}
		break;
	case DebugItem::List:
		if (pList)
			Inout_Data += std::to_string(pList->GetSelectNo());
		break;
	case DebugItem::Vector:
		if (pValue)
		{
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT3>().x) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT3>().y) + "/";
			Inout_Data += std::to_string(pValue->GetValue<DirectX::XMFLOAT3>().z);
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
	std::fstream file("Assets\\Debug\\DebugManagerData.csv", std::ios::in);

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
		const char *top[] = {
			DataItr->value.c_str(),
			strstr(top[0], "/") + 1,
		};
		pValue->GetValue() = DirectX::XMFLOAT2(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr)
		);
	} break;
	case DebugItem::Color:
	{
		const char *top[] = {
			DataItr->value.c_str(),
			strstr(top[0], "/") + 1,
			strstr(top[1], "/") + 1,
			strstr(top[2], "/") + 1,
		};
		pValue->GetValue() = DirectX::XMFLOAT4(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr),
			strtof(top[2], nullptr),
			strtof(top[3], nullptr)
		);
	} break;
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
		const char *top[] = {
			DataItr->value.c_str(),
			strstr(top[0], "/") + 1,
			strstr(top[1], "/") + 1,
		};
		pValue->GetValue() = DirectX::XMFLOAT3(
			strtof(top[0], nullptr),
			strtof(top[1], nullptr),
			strtof(top[2], nullptr)
		);
	}
	break;
	}
}
