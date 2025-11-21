/*+===================================================================
	File: DebugWindow.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:34:14 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugWindow.hpp"
#include "DebugItem.hpp"
#include "InitializeImGui.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	static DebugItem *s_NullItem = nullptr;
}

DebugWindow::DebugWindow(_In_ const std::string_view In_Name)
	: m_Name(In_Name), m_IsOpen(true), m_IsDummy(false)
{
}

DebugWindow::~DebugWindow()
{
}

void DebugWindow::Draw() noexcept
{
	for (const auto &item : m_Items)
	{
		DrawImgui(item);
	}
}

DebugItem &DebugWindow::operator[](_In_ std::string_view In_ItemName)
{
	auto itr = FindItem(In_ItemName);
	if (itr == m_Items.end())
		return *s_NullItem;

	return **itr;
}

void DebugWindow::RemoveItem(_In_ std::string_view In_Name)
{
	auto itr = FindItem(In_Name);
	if (itr != m_Items.end())
	{
		delete *itr;
		m_Items.erase(itr);
	}
}

void DebugWindow::ClearItems()
{
	for (auto item : m_Items)
	{
		delete item;
	}
	m_Items.clear();
}

void DebugWindow::DrawImgui(_In_ DebugItem *In_Item) noexcept
{
	ItemValue *pValue = dynamic_cast<ItemValue *>(In_Item);
	ItemBind *pBind = nullptr;
	ItemCallback *pCallback = nullptr;
	if (!pValue)
		pBind = dynamic_cast<ItemBind *>(In_Item);
	if (!pValue && !pBind)
		pCallback = dynamic_cast<ItemCallback *>(In_Item);

	switch (In_Item->GetKind())
	{
		// 項目名のみの表示
	case DebugItem::Label:
		if (pBind)
		{
			char str[256];
			sprintf_s(str, "%s : %s", In_Item->GetCStrName(), pBind->GetPtr<char>());
			ImGui::Text("%s", str);
		}
		else
			ImGui::Text("%s", In_Item->GetCStrName());
		break;
		// チェックフラグの表示
	case DebugItem::Bool:
		if (pValue)
			ImGui::Checkbox(In_Item->GetCStrName(), &std::get<bool>(pValue->GetValue()));
		else if (pBind)
			ImGui::Checkbox(In_Item->GetCStrName(), pBind->GetPtr<bool>());
		else if (pCallback)
		{
			pCallback->CallFunc(false, &std::get<bool>(pCallback->GetValue()));
			if (ImGui::Checkbox(In_Item->GetCStrName(), &std::get<bool>(pCallback->GetValue())))
				pCallback->CallFunc(true, &std::get<bool>(pCallback->GetValue()));
		}
		break;
		// 整数項目の表示
	case DebugItem::Int:
		if (pValue)
			ImGui::InputInt(In_Item->GetCStrName(), &std::get<int>(pCallback->GetValue()));
		else if (pBind)
			ImGui::InputInt(In_Item->GetCStrName(), pBind->GetPtr<int>());
		else if (pCallback)
		{
			pCallback->CallFunc(false, &std::get<int>(pCallback->GetValue()));
			if (ImGui::InputInt(In_Item->GetCStrName(), &std::get<int>(pCallback->GetValue())))
				pCallback->CallFunc(true, &std::get<int>(pCallback->GetValue()));
		}
		break;
		// 小数項目の表示
	case DebugItem::Float:
		if (pValue)
			ImGui::InputFloat(In_Item->GetCStrName(), &std::get<float>(pValue->GetValue()));
		else if (pBind)
			ImGui::InputFloat(In_Item->GetCStrName(), pBind->GetPtr<float>());
		else if (pCallback)
		{
			pCallback->CallFunc(false, &std::get<float>(pCallback->GetValue()));
			if (ImGui::InputFloat(In_Item->GetCStrName(), &std::get<float>(pCallback->GetValue())))
				pCallback->CallFunc(true, &std::get<float>(pCallback->GetValue()));
		}
		break;
		// ベクトル項目の表示
	case DebugItem::Vector:
		if (pValue)
			ImGui::InputFloat3(In_Item->GetCStrName(), &std::get<DirectX::XMFLOAT3>(pValue->GetValue()).x, "%.2f");
		else if (pBind)
			ImGui::InputFloat3(In_Item->GetCStrName(), pBind->GetPtr<float>(), "%.2f");
		else if (pCallback)
		{
			pCallback->CallFunc(false, &std::get<DirectX::XMFLOAT3>(pCallback->GetValue()));
			if (ImGui::InputFloat3(In_Item->GetCStrName(), &std::get<DirectX::XMFLOAT3>(pCallback->GetValue()).x, "%.2f"))
				pCallback->CallFunc(true, &std::get<DirectX::XMFLOAT3>(pCallback->GetValue()).x);
		}
		break;
		// 色項目の表示
	case DebugItem::Color:
		if (pValue)
			ImGui::ColorEdit4(In_Item->GetCStrName(), &std::get<DirectX::XMFLOAT4>(pValue->GetValue()).x);
		else if (pBind)
			ImGui::ColorEdit4(In_Item->GetCStrName(), pBind->GetPtr<float>());
		else if (pCallback)
		{
			pCallback->CallFunc(false, &std::get<DirectX::XMFLOAT4>(pCallback->GetValue()).x);
			if (ImGui::ColorEdit4(In_Item->GetCStrName(), &std::get<DirectX::XMFLOAT4>(pCallback->GetValue()).x))
				pCallback->CallFunc(true, &std::get<DirectX::XMFLOAT4>(pCallback->GetValue()).x);
		}
		break;
		// パス項目の表示
	case DebugItem::Path:
		if (typeid(ItemValue) == typeid(*In_Item)) // 通常表示
		{
			char buffer[MAX_PATH];
			strncpy_s(buffer, std::get<std::string>(pValue->GetValue()).c_str(), MAX_PATH);
			if(ImGui::InputText(In_Item->GetCStrName(), buffer, MAX_PATH))
				std::get<std::string>(pValue->GetValue()) = buffer;
		}
		else // 紐づけ項目の表示
			ImGui::InputText(In_Item->GetCStrName(), pBind->GetPtr<char>(), MAX_PATH);
		break;
		// 入力文字列項目の表示
	case DebugItem::InputStr:
	{
		ItemText *pText = dynamic_cast<ItemText *>(In_Item);
		if (!pText) break;
		if (pText->IsMultiline())
		{
			// 複数行入力
			char buffer[4096];
			strncpy_s(buffer, pText->GetText().c_str(), 4096);
			float height = ImGui::GetTextLineHeight();
			if (pText->GetLineCount() > 0)
				height *= pText->GetLineCount();
			else
				height = ImGui::GetContentRegionAvail().y;
			if (ImGui::InputTextMultiline(In_Item->GetCStrName(), buffer, 4096, ImVec2(-FLT_MIN, height), pText->GetFlags()))
				pText->GetText() = buffer;
		}
		else
		{
			// 単一行入力
			char buffer[256];
			strncpy_s(buffer, pText->GetText().c_str(), 256);
			if (ImGui::InputText(In_Item->GetCStrName(), buffer, 256, pText->GetFlags()))
				pText->GetText() = buffer;
		}
	}
	break;
		// ボタンの表示
	case DebugItem::Kind::Command:
		if (ImGui::Button(In_Item->GetCStrName()) && pCallback)
			pCallback->CallFunc(false, nullptr);
		break;
		// グループ項目の表示
	case DebugItem::Group:
	{
		ItemGroup *ptr = dynamic_cast<ItemGroup *>(In_Item);
		// 表示項目がなければ表示しない
		if (ptr->GetGroupItems().empty()) break;

		// グループが展開されてなければ表示しない
		if (!ImGui::CollapsingHeader(In_Item->GetCStrName(), ImGuiTreeNodeFlags_DefaultOpen)) break;

		// グループ内の項目を再帰で表示
		auto itr = ptr->GetGroupItems().begin();
		for (; itr != ptr->GetGroupItems().end(); ++itr)
		{
			DrawImgui(*itr);
		}
	}
	break;
	// リスト項目の表示
	case DebugItem::List:
	{
		ItemList *ptr = dynamic_cast<ItemList *>(In_Item);
		// 表示項目がなければ表示しない
		if (ptr->GetList().empty()) break;

		// 表示項目の構築
		static const char *pList[100];
		auto it = ptr->GetList().begin();
		for (int i = 0; i < ptr->GetList().size() && i < 100; ++i, ++it)
			pList[i] = it->c_str();

		// 表示
		if (ImGui::ListBox(In_Item->GetCStrName(), &ptr->GetSelectNo(), pList, static_cast<int>(ptr->GetList().size())))
		{
			// 関数があれば選択時処理を実行
			if (ptr->GetFunc() && ptr->GetSelectNo() >= 0)
				ptr->GetFunc()(pList[ptr->GetSelectNo()]);
		}
	}
	break;
	}
}
