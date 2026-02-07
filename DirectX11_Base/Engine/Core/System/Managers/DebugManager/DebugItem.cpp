/*+===================================================================
	File: DebugItem.cpp
	Summary: デバッグ項目クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:00:20 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugItem.hpp"
#include "DebugManager.hpp"
#include "ryuu_lib/FrameManager/FrameManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	static DebugItem *s_NullItem = nullptr;
	inline constexpr size_t cx_MaxConsoleLines = 5000;
}

DebugItem::DebugItem()
{
	m_Name = "None";
	m_Kind = Kind::Label;
}

DebugItem::~DebugItem()
{
}

std::string DebugItem::GetName() const
{
	return m_Name;
}

const char *DebugItem::GetCStrName() const
{
	return m_Name.c_str();
}

DebugItem::Kind DebugItem::GetKind() const
{
	return m_Kind;
}

bool DebugItem::GetBool()
{
	if (m_Kind == Kind::Bool)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if(ptr)
			return std::get<bool>(ptr->GetValue());
	}
	return false;
}

int DebugItem::GetInt()
{
	if (m_Kind == Kind::Int)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if (ptr)
			return std::get<int>(ptr->GetValue());
	}
	return 0;
}

float DebugItem::GetFloat()
{
	if (m_Kind == Kind::Float)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if (ptr)
			return std::get<float>(ptr->GetValue());
	}
	return 0.0f;
}

DirectX::XMFLOAT2 DebugItem::GetVector2()
{
	if(m_Kind == Kind::Float2)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if(ptr)
			return std::get<DirectX::XMFLOAT2>(ptr->GetValue());
	}
	return DirectX::XMFLOAT2{};
}

DirectX::XMFLOAT3 DebugItem::GetVector()
{
	if (m_Kind == Kind::Vector)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if(ptr)
			return std::get<DirectX::XMFLOAT3>(ptr->GetValue());
	}
	return DirectX::XMFLOAT3{};
}

DirectX::XMFLOAT4 DebugItem::GetColor()
{
	if (m_Kind == Kind::Color)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if(ptr)
			return std::get<DirectX::XMFLOAT4>(ptr->GetValue());
	}
	return DirectX::XMFLOAT4{};
}

std::string DebugItem::GetStr()
{
	if (m_Kind == Label || m_Kind == Path)
	{
		auto ptr = dynamic_cast<ItemValue *>(this);
		if(ptr)
			return std::get<std::string>(ptr->GetValue());
	}
	return "";
}

DebugItem &DebugItem::operator[](_In_ std::string_view In_Name)
{
	if (m_Kind != Kind::Group)
		return *s_NullItem;

	// グループ内のアイテム一覧を取得
	auto items = static_cast<ItemGroup *>(this)->GetGroupItems();

	// アイテム一覧から指定アイテムの探索
	auto it = std::find_if(items.begin(), items.end(),
		[&In_Name](const DebugItem *obj)
		{
			return std::strcmp(obj->GetName().c_str(), In_Name.data()) == 0;
		});

	// 見つかったアイテムの返却
	if (it != items.end())
		return **it;

	// 見つからなかったらダミーを返却
	return *s_NullItem;
}

void DebugItem::AddListItem(_In_ std::string_view In_Name)
{
	if (m_Kind != Kind::List)
		return;

	auto ptr = dynamic_cast<ItemList *>(this);
	if (ptr)
	{
		ptr->GetList().push_back(In_Name.data());
	}
}

void DebugItem::RemoveListItem(_In_ std::string_view In_Name)
{
	if (m_Kind != Kind::List)
		return;

	auto ptr = dynamic_cast<ItemList *>(this);
	if (ptr)
	{
		ptr->GetList().remove(In_Name.data());
	}
}

DebugItem::Kind DebugItem::StrToKind(_In_ std::string_view In_Str)
{
	if (In_Str == "Label")		return DebugItem::Label;
	if (In_Str == "Bool")		return DebugItem::Bool;
	if (In_Str == "Int")		return DebugItem::Int;
	if (In_Str == "Float")		return DebugItem::Float;
	if (In_Str == "Float2")		return DebugItem::Float2;
	if (In_Str == "Vector")		return DebugItem::Vector;
	if (In_Str == "Color")		return DebugItem::Color;
	if (In_Str == "Path")		return DebugItem::Path;
	if (In_Str == "InputStr")	return DebugItem::InputStr;
	if (In_Str == "Console")	return DebugItem::Console;
	if (In_Str == "Command")	return DebugItem::Command;
	if (In_Str == "Group")		return DebugItem::Group;
	if (In_Str == "List")		return DebugItem::List;
	if (In_Str == "LayoutFunc")	return DebugItem::LayoutFunc;
	if (In_Str == "__Hierarchy")	return DebugItem::__Hierarchy;
	return DebugItem::Label;
}

std::string DebugItem::KindToStr(_In_ Kind In_Kind)
{
	switch (In_Kind)
	{
	default:
	case DebugItem::Label:		return "Label";
	case DebugItem::Bool:		return "Bool";
	case DebugItem::Int:		return "Int";
	case DebugItem::Float:		return "Float";
	case DebugItem::Float2:		return "Float2";
	case DebugItem::Vector:		return "Vector";
	case DebugItem::Color:		return "Color";
	case DebugItem::Path:		return "Path";
	case DebugItem::InputStr:	return "InputStr";
	case DebugItem::Console:	return "Console";
	case DebugItem::Command:	return "Command";
	case DebugItem::Group:		return "Group";
	case DebugItem::List:		return "List";
	case DebugItem::LayoutFunc:	return "LayoutFunc";
	case DebugItem::__Hierarchy:	return "__Hierarchy";
	}
}

// ==============================
//  ItemValue
// ==============================


ItemValue::ItemValue(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ bool In_IsSave)
	: m_Value(), m_IsSave(In_IsSave), m_Notice(nullptr)
{
	m_Name = In_Name.data();

	if(In_Kind == Kind::InputStr || In_Kind == Kind::Group || In_Kind == Kind::List || In_Kind == Kind::Command)
	{
		// InputStr、グループ、リスト、コマンドはItemValueでは扱えない
		In_Kind = Kind::Label;
	}

	m_Kind = In_Kind;

	switch(m_Kind)
	{
	case Kind::Label:
	case Kind::Path:
	case Kind::InputStr:
		m_Value = std::string{};
		break;
	case Kind::Bool:
		m_Value = false;
		break;
	case Kind::Int:
		m_Value = 0;
		break;
	case Kind::Float:
		m_Value = 0.0f;
		break;
	case Kind::Vector:
		m_Value = DirectX::XMFLOAT3{};
		break;
	case Kind::Color:
		m_Value = DirectX::XMFLOAT4{};
		break;
	}
}

ItemValue::~ItemValue()
{
}

void ItemValue::DrawImGui()
{
	switch (m_Kind)
	{
		// 項目名のみの表示
	case DebugItem::Label:
		ImGui::Text("%s", m_Name.c_str());
		break;
		// チェックフラグの表示
	case DebugItem::Bool:
		if(ImGui::Checkbox(m_Name.c_str(), &std::get<bool>(m_Value)))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 整数項目の表示
	case DebugItem::Int:
		if(ImGui::InputInt(m_Name.c_str(), &std::get<int>(m_Value)))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 小数項目の表示
	case DebugItem::Float:
		if(ImGui::InputFloat(m_Name.c_str(), &std::get<float>(m_Value)))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 2Dベクトル項目の表示
	case DebugItem::Float2:
		if(ImGui::InputFloat2(m_Name.c_str(), &std::get<DirectX::XMFLOAT2>(m_Value).x, "%.2f"))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// ベクトル項目の表示
	case DebugItem::Vector:
		if(ImGui::InputFloat3(m_Name.c_str(), &std::get<DirectX::XMFLOAT3>(m_Value).x, "%.2f"))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 色項目の表示
	case DebugItem::Color:
		if(ImGui::ColorEdit4(m_Name.c_str(), &std::get<DirectX::XMFLOAT4>(m_Value).x))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// パス項目の表示
	case DebugItem::Path:
	{
		char buffer[MAX_PATH];
		strncpy_s(buffer, std::get<std::string>(m_Value).c_str(), MAX_PATH);
		if(ImGui::InputText(m_Name.c_str(), buffer, MAX_PATH))
		{
			std::get<std::string>(m_Value) = buffer;
			if(m_Notice)
				m_Notice();
		}
	}
	break;
	default:
		break;
	}
}

void ItemValue::SetNoticeFunc(_In_ std::function<void()> In_NoticeFunc) noexcept
{
	m_Notice = In_NoticeFunc;
}

// ==============================
//  ItemText
// ==============================

ItemText::ItemText(_In_ std::string_view In_Name, _In_ bool In_IsMultiline, _In_ ImGuiInputTextFlags In_Flags, _In_ bool Is_HideLabel, _In_ bool In_IsSave)
{
	m_Name = In_Name.data();
	m_Kind = Kind::InputStr;
	m_Flags = In_Flags;
	m_IsMultiline = In_IsMultiline;
	m_IsHideLabel = Is_HideLabel;
	m_IsSave = In_IsSave;
	m_LineCount = 0;
}

ItemText::~ItemText()
{
}

void ItemText::DrawImGui()
{
	switch (m_Kind)
	{
		// 入力文字列項目の表示
	case DebugItem::InputStr:
	{
		if (m_IsMultiline)
		{
			// 複数行入力
			char buffer[4096];
			strncpy_s(buffer,m_Text.c_str(), 4096);
			float height = ImGui::GetTextLineHeight();
			float width = ImGui::GetContentRegionAvail().x;
			std::string label;
			if (m_IsHideLabel)
				label = std::string("##");
			else
				width -= ImGui::CalcTextSize(m_Name.c_str()).x + ImGui::GetStyle().ItemSpacing.x;

			label += m_Name;
			if (m_LineCount > 0)
				height *= m_LineCount;
			else
				height = ImGui::GetContentRegionAvail().y;
			if (ImGui::InputTextMultiline(label.c_str(), buffer, 4096, ImVec2(width, height), m_Flags))
				m_Text = buffer;
		}
		else
		{
			// 単一行入力
			char buffer[256];
			strncpy_s(buffer, m_Text.c_str(), 256);
			if (ImGui::InputText(m_Name.c_str(), buffer, 256, m_Flags))
				m_Text = buffer;
		}
	}
	break;
	default:
		break;
	}
}

// ==============================
//  ItemBind
// ==============================

ItemBind::ItemBind(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ void *In_Ptr)
	: m_IsString(false), m_Notice(nullptr)
{
	m_Name = In_Name.data();

	if(In_Kind == Kind::Group || In_Kind == Kind::List || In_Kind == Kind::Command)
	{
		// グループ、リスト、コマンドはItemBindでは扱えない
		In_Kind = Kind::Label;
	}

	m_Kind = In_Kind;
	m_vPtr = In_Ptr;
}

ItemBind::ItemBind(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ std::string *In_Ptr)
	: ItemBind(In_Name, In_Kind, static_cast<void *>(In_Ptr))
{
	m_IsString = true;
}

ItemBind::~ItemBind()
{
}

void ItemBind::DrawImGui()
{
	switch (m_Kind)
	{
		// 項目名のみの表示
	case DebugItem::Label:
	{
		char str[256];
		sprintf_s(str, "%s : %s", m_Name.c_str(), this->GetPtr<char>());
		ImGui::Text("%s", str);
	}
	break;
		// チェックフラグの表示
	case DebugItem::Bool:
		if(ImGui::Checkbox(m_Name.c_str(), this->GetPtr<bool>()))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 整数項目の表示
	case DebugItem::Int:
		if(ImGui::InputInt(m_Name.c_str(), this->GetPtr<int>()))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 小数項目の表示
	case DebugItem::Float:
		if(ImGui::InputFloat(m_Name.c_str(), this->GetPtr<float>()))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 2Dベクトル項目の表示
	case DebugItem::Float2:
		if(ImGui::InputFloat2(m_Name.c_str(), this->GetPtr<float>(), "%.2f"))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// ベクトル項目の表示
	case DebugItem::Vector:
		if(ImGui::InputFloat3(m_Name.c_str(), this->GetPtr<float>(), "%.2f"))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// 色項目の表示
	case DebugItem::Color:
		if(ImGui::ColorEdit4(m_Name.c_str(), this->GetPtr<float>()))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
		// パス項目の表示
	case DebugItem::Path:
		// 紐づけ項目の表示
		if(ImGui::InputText(m_Name.c_str(), this->GetPtr<char>(), MAX_PATH))
		{
			if(m_Notice)
				m_Notice();
		}
		break;
	default:
		break;
	}
}

void ItemBind::SetNoticeFunc(_In_ std::function<void()> In_NoticeFunc) noexcept
{
	m_Notice = In_NoticeFunc;
}

// ==============================
//  ItemCallback
// ==============================

ItemCallback::ItemCallback(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ CallBack In_Func)
{
	m_Name = In_Name.data();
	if(In_Kind == Kind::Group || In_Kind == Kind::List)
	{
		// グループ、リストはItemCallbackでは扱えない
		In_Kind = Kind::Label;
	}
	m_Kind = In_Kind;
	m_Func = In_Func;

	switch (m_Kind)
	{
	case Kind::Label:
	case Kind::Path:
	case Kind::InputStr:
		m_Value = std::string{};
		break;
	case Kind::Bool:
		m_Value = false;
		break;
	case Kind::Int:
		m_Value = 0;
		break;
	case Kind::Float:
		m_Value = 0.0f;
		break;
	case Kind::Vector:
		m_Value = DirectX::XMFLOAT3{};
		break;
	case Kind::Color:
		m_Value = DirectX::XMFLOAT4{};
		break;
	}
}

ItemCallback::~ItemCallback()
{
}

void ItemCallback::DrawImGui()
{
	switch (m_Kind)
	{
		// 項目名のみの表示
	case DebugItem::Label:
		ImGui::Text("%s", m_Name.c_str());
		break;
		// チェックフラグの表示
	case DebugItem::Bool:
	{
		this->CallFunc(false, &std::get<bool>(m_Value));
		if (ImGui::Checkbox(m_Name.c_str(), &std::get<bool>(m_Value)))
			this->CallFunc(true, &std::get<bool>(m_Value));
	}
	break;
		// 整数項目の表示
	case DebugItem::Int:
	{
		this->CallFunc(false, &std::get<int>(m_Value));
		if (ImGui::InputInt(m_Name.c_str(), &std::get<int>(m_Value)))
			this->CallFunc(true, &std::get<int>(m_Value));
	}
	break;
		// 小数項目の表示
	case DebugItem::Float:
	{
		this->CallFunc(false, &std::get<float>(m_Value));
		if (ImGui::InputFloat(m_Name.c_str(), &std::get<float>(m_Value)))
			this->CallFunc(true, &std::get<float>(m_Value));
	}
	break;
		// 2Dベクトル項目の表示
	case DebugItem::Float2:
	{
		this->CallFunc(false, &std::get<DirectX::XMFLOAT2>(m_Value));
		if (ImGui::InputFloat2(m_Name.c_str(), &std::get<DirectX::XMFLOAT2>(m_Value).x, "%.2f"))
			this->CallFunc(true, &std::get<DirectX::XMFLOAT2>(m_Value).x);
	}
	break;
		// ベクトル項目の表示
	case DebugItem::Vector:
	{
		this->CallFunc(false, &std::get<DirectX::XMFLOAT3>(m_Value));
		if (ImGui::InputFloat3(m_Name.c_str(), &std::get<DirectX::XMFLOAT3>(m_Value).x, "%.2f"))
			this->CallFunc(true, &std::get<DirectX::XMFLOAT3>(m_Value).x);
	}
	break;
		// 色項目の表示
	case DebugItem::Color:
	{
		this->CallFunc(false, &std::get<DirectX::XMFLOAT4>(m_Value).x);
		if (ImGui::ColorEdit4(m_Name.c_str(), &std::get<DirectX::XMFLOAT4>(m_Value).x))
			this->CallFunc(true, &std::get<DirectX::XMFLOAT4>(m_Value).x);
	}
	break;
	// ボタンの表示
	case DebugItem::Kind::Command:
		if (ImGui::Button(m_Name.c_str()) && this)
			this->CallFunc(false, nullptr);
		break;
	default:
		break;
	}
}

// ==============================
//  ItemGroup
// ==============================

ItemGroup::ItemGroup(_In_ std::string_view In_Name)
{
	m_Name = In_Name.data();
	m_Kind = Kind::Group;
}

ItemGroup::~ItemGroup()
{
	for(auto &item : m_Items)
	{
		delete item;
	}
	m_Items.clear();
}

void ItemGroup::DrawImGui()
{
	switch (m_Kind)
	{
		// グループ項目の表示
	case DebugItem::Group:
	{
		// 表示項目がなければ表示しない
		if (m_Items.empty()) break;

		// グループが展開されてなければ表示しない
		if (!ImGui::CollapsingHeader(m_Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) break;

		// グループ内の項目を再帰で表示
		for (auto &itr : m_Items)
		{
			itr->DrawImGui();
		}
	}
	break;
	default:
		break;
	}
}

void ItemGroup::DataRead(_In_ std::string_view In_FullPath, _In_ DebugItem *In_pItem)
{
	DebugManager::GetInstance().DataRead(In_FullPath.data(), In_pItem);
}

// ==============================
//  ItemList
// ==============================

ItemList::ItemList(_In_ std::string_view In_Name, _In_ ConstCallback In_Func, _In_ bool In_IsSave)
{
	m_Name = In_Name.data();
	m_Kind = Kind::List;
	m_SelectNo = -1;
	m_Func = In_Func;
	m_IsSave = In_IsSave;
}

ItemList::~ItemList()
{
	m_List.clear();
}

void ItemList::DrawImGui()
{
	switch (m_Kind)
	{	// リスト項目の表示
	case DebugItem::List:
	{
		// 表示項目がなければ表示しない
		if (m_List.empty()) break;

		// 表示項目の構築
		static const char *pList[100];
		auto it = m_List.begin();
		for (int i = 0; i < m_List.size() && i < 100; ++i, ++it)
			pList[i] = it->c_str();

		// 表示
		if (ImGui::ListBox(m_Name.c_str(), &m_SelectNo, pList, static_cast<int>(m_List.size())))
		{
			// 関数があれば選択時処理を実行
			if (m_Func && m_SelectNo >= 0)
				m_Func(pList[m_SelectNo]);
		}
	}
	break;
	default:
		break;
	}
}

// ==============================
//  ItemConsole
// ==============================

ItemConsole::ItemConsole(_In_ std::string_view In_Name, _In_ bool In_IsOutputLogFile)
	: m_IsShowClearButton(true), m_IsShowAutoScrollButton(true), m_IsAutoScroll(true), m_IsShowSerchBox(true)
	, m_ScrollToBottom(false), m_IsOutputLogFile(In_IsOutputLogFile)
{
	m_Name = In_Name.data();
	m_Kind = Kind::Console;
	m_SerchBuffer[0] = '\0';
	LevelData DefaultLevel;
	DefaultLevel.Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	DefaultLevel.IsShow = true;
	m_Levels.try_emplace("Default",DefaultLevel);
	m_FolderPath = "Engine/Assets/DebugResource/LogFile";
}

ItemConsole::~ItemConsole()
{
	if (m_IsOutputLogFile)
		WriteLogToFile();
	m_Outputs.clear();
}

void ItemConsole::DrawImGui()
{
	switch (m_Kind)
	{
		// 文字列表示項目の表示
	case DebugItem::Console:
	{
		// ツールバー
		if (m_IsShowClearButton)
		{
			if (ImGui::Button("Clear"))
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_Outputs.clear();
			}
			ImGui::SameLine();
		}
		if (m_IsShowAutoScrollButton)
		{
			ImGui::Checkbox("Auto Scroll", &m_IsAutoScroll);
			ImGui::SameLine();
		}
		if (m_IsShowSerchBox)
		{
			ImGui::SetNextItemWidth(200.0f);
			ImGui::InputTextWithHint("##search", "Search...", m_SerchBuffer, IM_ARRAYSIZE(m_SerchBuffer));
			ImGui::SameLine();
		}
		ImGui::Spacing();
		// レベル表示切替ボタン(右揃えで表示)
		float LevelPos = ImGui::GetContentRegionAvail().x;
		for(int i = static_cast<int>(m_Levels.size()) - 1; i >= 0; --i)
		{
			auto level = m_Levels.begin();
			std::advance(level, i);
			LevelPos -= ImGui::GetStyle().ItemSpacing.x + ImGui::CalcTextSize(level->first.c_str()).x + ImGui::GetFrameHeight();
			ImGui::SameLine(LevelPos);
			if (ImGui::Checkbox(level->first.c_str(), &level->second.IsShow))
			{
			}
		}

		ImGui::Separator();
		// 出力内容表示領域
		ImGui::BeginChild("OutputRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImVec2 regionMin = ImGui::GetCursorScreenPos();
			ImDrawList *draw_list = ImGui::GetWindowDrawList();
			ImFont *font = ImGui::GetFont();
			float fontSize = ImGui::GetFontSize();

			// Copy snapshot to avoidロックを長時間保持
			std::vector<OutputData> snapshot;
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				snapshot = m_Outputs;
			}

			// 検索/フィルタ処理（簡易）
			std::vector<int> indices; indices.reserve(snapshot.size());
			for (int i = 0; i < (int)snapshot.size(); ++i)
			{
				const OutputData &e = snapshot[i];
				if (!m_Levels.find(e.Level)->second.IsShow) continue;
				if (m_SerchBuffer[0] != '\0')
				{
					std::string s = m_SerchBuffer;
					// 小文字化して簡易検索（必要なら強化）
					std::string hay = e.Text;
					std::transform(hay.begin(), hay.end(), hay.begin(), ::tolower);
					std::transform(s.begin(), s.end(), s.begin(), ::tolower);
					if (hay.find(s) == std::string::npos) continue;
				}
				indices.push_back(i);
			}

			// 大量行のときは ListClipper を使う
			ImGuiListClipper clipper;
			clipper.Begin((int)indices.size());
			while (clipper.Step())
			{
				for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line)
				{
					int idx = indices[line];
					const OutputData &e = snapshot[idx];

					// 行全体を Selectable にして行選択を実装（行単位コピー用）
					ImGui::PushID(line);
					ImVec4 col = e.Color;
					// 時刻とレベルを先頭に表示するフォーマット例
					std::string prefix = "[" + e.TimeStr + "] ";
					std::string LevelStr = e.Level + ": ";
					ImGui::TextUnformatted(prefix.c_str()); ImGui::SameLine();
					if (LevelStr != "Default: ") // defaultレベルの場合はレベル表示しない
					{
						ImGui::PushStyleColor(ImGuiCol_Text, m_Levels.find(e.Level)->second.Color);
						ImGui::TextUnformatted(LevelStr.c_str()); ImGui::PopStyleColor(); ImGui::SameLine();
					}

					// カラーテキストを単純に描く（必要なら BeginGroup/EndGroup で整形）
					ImGui::PushStyleColor(ImGuiCol_Text, col);
					// Selectable はテキストを表示しつつ選択可能にする：幅をフルに取るため ImGui::GetContentRegionAvail().x を利用
					float availW = ImGui::GetContentRegionAvail().x;
					// 改行の可能性があるので TextWrapped で折り返し表示する（ただし Selectable 内での折返しは難しいため単純実装）
					ImGui::TextWrapped("%s", e.Text.c_str());
					ImGui::PopStyleColor();

					// 行のクリックでクリップボードにコピー
					if (ImGui::IsItemClicked())
					{
						ImGui::SetClipboardText(e.Text.c_str());
					}
					ImGui::PopID();
				}
			}

			// 自動スクロール
			if (m_IsAutoScroll && (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f || m_ScrollToBottom))
			{
				ImGui::SetScrollHereY(1.0f);
				m_ScrollToBottom = false;
			}
		}
		ImGui::EndChild();
	}
	break;
	default:
		break;
	}

	// コンソール内容が最大行数を超えたら古い行を削除
	if(m_Outputs.size() > cx_MaxConsoleLines)
	{
		if (m_IsOutputLogFile)
			this->WriteLogToFile();

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Outputs.clear();
	}
}

void ItemConsole::AddLevel(_In_ std::string_view In_Name, _In_ const ImVec4 &In_Color)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	LevelData Data;
	Data.Color = In_Color;
	Data.IsShow = true;
	auto itr = m_Levels.try_emplace(In_Name.data(), Data);

	if(!itr.second)
		DebugManager::GetInstance().DebugLogWarning("ItemConsole::AddLevel: Level already exists: %s", In_Name.data());
}

void ItemConsole::AddOutput(_In_ std::string_view In_Text, _In_ const ImVec4 &In_Color, _In_ std::string_view In_Level)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	// デフォルトレベルの場合は無条件で追加
	if (std::strcmp(In_Level.data(), "Default") == 0)
	{
		m_Outputs.push_back({ In_Text.data(), this->CurrentTimeString(), In_Level.data(), In_Color });
		return;
	}

	// レベルが存在しない場合は追加しない
	auto itr = m_Levels.find(In_Level.data());
	if (itr == m_Levels.end())
		return;
	m_Outputs.push_back({ In_Text.data(), this->CurrentTimeString(), In_Level.data(), In_Color });
}

std::string ItemConsole::CurrentTimeString()
{
	return FrameManager::GetInstance().GetNowTimeString();
	std::time_t t = std::time(nullptr);
	std::tm tm{};

	localtime_s(&tm, &t);
	char buf[16];
	std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buf);
}

int ItemConsole::GetButtonCount() const
{
	int count = 0;
	if (m_IsShowClearButton)
		++count;
	if (m_IsShowAutoScrollButton)
		++count;
	if (m_IsShowSerchBox)
		++count;

	return count;
}

void ItemConsole::WriteLogToFile()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	if (m_Outputs.empty())
		return;
	// ファイル名を生成
	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::stringstream date;
	std::tm dateTm{};
	localtime_s(&dateTm, &currentTime);
	date << std::put_time(&dateTm, "%Y-%m-%d_%H%M%S");
	std::string FilePath = m_FolderPath + "/Log_" + m_Name + "_" + date.str() + ".log";


	std::fstream file(FilePath, std::ios::out | std::ios::trunc);

	if (!file.is_open())
	{
		std::filesystem::create_directories(m_FolderPath);
		file.open(FilePath, std::ios::out | std::ios::trunc);
		if (!file.is_open())
			return;
	}

	for (const auto &output : m_Outputs)
	{
		file << "[" << output.TimeStr << "] ";
		if (output.Level != "Default")
			file << output.Level << ": ";
		file << output.Text << std::endl;
	}
	file.close();
}

// ==============================
//  ItemLayoutFunc
// ==============================

ItemLayoutFunc::ItemLayoutFunc(_In_ std::string_view In_Name, _In_ LayoutType In_LayoutType)
{
	m_Name = In_Name.data();
	m_Kind = Kind::LayoutFunc;
	m_LayoutType = In_LayoutType;
}

ItemLayoutFunc::~ItemLayoutFunc()
{
}

// ==============================
//  ItemSameLine
// ==============================

ItemSameLine::ItemSameLine(_In_ std::string_view In_Name, _In_ float In_OffsetX, _In_ float In_SpacingW)
	: ItemLayoutFunc(In_Name, LayoutType::SameLine), m_OffsetX(In_OffsetX), m_SpacingW(In_SpacingW)
{
	if(m_SpacingW < 0.0f)
		m_SpacingW = -1.0f;
}

ItemSameLine::~ItemSameLine()
{
}

void ItemSameLine::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;

	ImGui::SameLine(m_OffsetX, m_SpacingW);
}

ItemNewLine::ItemNewLine(_In_ std::string_view In_Name)
	: ItemLayoutFunc(In_Name, LayoutType::NewLine)
{
}

ItemNewLine::~ItemNewLine()
{
}

void ItemNewLine::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::NewLine();
}

ItemSpacing::ItemSpacing(_In_ std::string_view In_Name, _In_ int In_SpaceNum)
	: ItemLayoutFunc(In_Name, LayoutType::Spacing), m_SpaceNum(In_SpaceNum)
{
}

ItemSpacing::~ItemSpacing()
{
}

void ItemSpacing::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	for(int i = 0; i < m_SpaceNum; ++i)
		ImGui::Spacing();
}

ItemSeparator::ItemSeparator(_In_ std::string_view In_Name)
	: ItemLayoutFunc(In_Name, LayoutType::Separator)
{
}

ItemSeparator::~ItemSeparator()
{
}

void ItemSeparator::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::Separator();
}

ItemIndent::ItemIndent(_In_ std::string_view In_Name, _In_ float In_IndentW)
	: ItemLayoutFunc(In_Name, LayoutType::Indent), m_IndentW(In_IndentW)
{
	if(m_IndentW < 0.0f)
		m_IndentW = 0.0f;
}

ItemIndent::~ItemIndent()
{
}

void ItemIndent::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::Indent(m_IndentW);
}

ItemUnIndent::ItemUnIndent(_In_ std::string_view In_Name, _In_ float In_IndentW)
	: ItemLayoutFunc(In_Name, LayoutType::UnIndent), m_IndentW(In_IndentW)
{
	if(m_IndentW < 0.0f)
		m_IndentW = 0.0f;
}

ItemUnIndent::~ItemUnIndent()
{
}

void ItemUnIndent::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::Unindent(m_IndentW);
}

ItemDummy::ItemDummy(_In_ std::string_view In_Name, _In_ float In_Width, _In_ float In_Height)
	: ItemLayoutFunc(In_Name, LayoutType::Dummy), m_Width(In_Width), m_Height(In_Height)
{
	if(m_Width < 0.0f)
		m_Width = 0.0f;
	if(m_Height < 0.0f)
		m_Height = 0.0f;
}

ItemDummy::~ItemDummy()
{
}

void ItemDummy::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::Dummy(ImVec2(m_Width, m_Height));
}

ItemSetNextItemWidth::ItemSetNextItemWidth(_In_ std::string_view In_Name, _In_ float In_Width)
	: ItemLayoutFunc(In_Name, LayoutType::SetNextItemWidth), m_Width(In_Width)
{
	if(m_Width < 0.0f)
		m_Width = -1.0f;
}

ItemSetNextItemWidth::~ItemSetNextItemWidth()
{
}

void ItemSetNextItemWidth::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::SetNextItemWidth(m_Width);
}

ItemAlignTextToFramePadding::ItemAlignTextToFramePadding(_In_ std::string_view In_Name)
	: ItemLayoutFunc(In_Name, LayoutType::AlignTextToFramePadding)
{
}

ItemAlignTextToFramePadding::~ItemAlignTextToFramePadding()
{
}

void ItemAlignTextToFramePadding::DrawImGui()
{
	if (m_Kind != Kind::LayoutFunc)
		return;
	ImGui::AlignTextToFramePadding();
}
