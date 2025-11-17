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
#include "InitializeImGui.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	static DebugItem *s_NullItem = nullptr;
}

DebugWindow::DebugWindow()
{
}

DebugWindow::~DebugWindow()
{
}

void DebugWindow::Draw() noexcept
{
	for (const auto &item : m_Items)
	{

		switch (item->GetKind())
		{
			// 項目名のみの表示
		case Item::Label:
			if (isBind)
			{
				char str[256];
				sprintf_s(str, "%s : %s", item->GetName(), reinterpret_cast<char *>(pBind->ptr));
				ImGui::Text("%s", str);
			}
			else
				ImGui::Text("%s", item->GetName());
			break;
			// チェックフラグの表示
		case Item::Bool:
			if (isValue)
				ImGui::Checkbox(item->GetCStrName(), &pValue->value.flg);
			else if (isBind)
				ImGui::Checkbox(item->GetCStrName(), reinterpret_cast<bool *>(pBind->ptr));
			else if (isCallback)
			{
				pCallback->func(false, &pCallback->value.flg);
				if (ImGui::Checkbox(item->GetName(), &pCallback->value.flg))
					pCallback->func(true, &pCallback->value.flg);
			}
			break;
			// 整数項目の表示
		case Item::Int:
			if (isValue)
				ImGui::InputInt(item->GetName(), &pValue->value.nValue);
			else if (isBind)
				ImGui::InputInt(item->GetName(), reinterpret_cast<int *>(pBind->ptr));
			else if (isCallback)
			{
				pCallback->func(false, &pCallback->value.nValue);
				if (ImGui::InputInt(item->GetName(), &pCallback->value.nValue))
					pCallback->func(true, &pCallback->value.nValue);
			}
			break;
			// 小数項目の表示
		case Item::Float:
			if (isValue)
				ImGui::InputFloat(item->GetName(), &pValue->value.fValue);
			else if (isBind)
				ImGui::InputFloat(item->GetName(), reinterpret_cast<float *>(pBind->ptr));
			else if (isCallback)
			{
				pCallback->func(false, &pCallback->value.fValue);
				if (ImGui::InputFloat(item->GetName(), &pCallback->value.fValue))
					pCallback->func(true, &pCallback->value.fValue);
			}
			break;
			// ベクトル項目の表示
		case Item::Vector:
			if (isValue)
				ImGui::InputFloat3(item->GetName(), &pValue->value.vec.x, "%.2f");
			else if (isBind)
				ImGui::InputFloat3(item->GetName(), reinterpret_cast<float *>(pBind->ptr), "%.2f");
			else if (isCallback)
			{
				pCallback->func(false, &pCallback->value.vec.x);
				if (ImGui::InputFloat3(item->GetName(), &pCallback->value.vec.x, "%.2f"))
					pCallback->func(true, &pCallback->value.vec.x);
			}
			break;
			// 色項目の表示
		case Item::Color:
			if (isValue)
				ImGui::ColorEdit4(item->GetName(), &pValue->value.color.x);
			else if (isBind)
				ImGui::ColorEdit4(item->GetName(), reinterpret_cast<float *>(pBind->ptr));
			else if (isCallback)
			{
				pCallback->func(false, &pCallback->value.color.x);
				if (ImGui::ColorEdit4(item->GetName(), &pCallback->value.color.x))
					pCallback->func(true, &pCallback->value.color.x);
			}
			break;
			// パス項目の表示
		case Item::Path:
			if (typeid(ItemValue) == typeid(*item)) // 通常表示
				ImGui::InputText(item->GetName(), static_cast<ItemValue *>(item)->value.str, MAX_PATH);
			else // 紐づけ項目の表示
				ImGui::InputText(item->GetName(), reinterpret_cast<char *>(static_cast<ItemBind *>(item)->ptr), MAX_PATH);
			break;
			// ボタンの表示
		case Item::Kind::Command:
			if (ImGui::Button(item->GetName()))
				pCallback->func(false, nullptr);
			break;
			// グループ項目の表示
		case Item::Group:
		{
			ItemGroup *ptr = reinterpret_cast<ItemGroup *>(item);
			// 表示項目がなければ表示しない
			if (ptr->group.empty()) break;

			// グループが展開されてなければ表示しない
			if (!ImGui::CollapsingHeader(item->GetName(), ImGuiTreeNodeFlags_DefaultOpen)) break;

			// グループ内の項目を再帰で表示
			auto it = ptr->group.begin();
			while (it != ptr->group.end())
			{
				DrawImgui(*it);
				++it;
			}
		}
		break;
		// リスト項目の表示
		case Item::List:
		{
			ItemList *ptr = reinterpret_cast<ItemList *>(item);
			// 表示項目がなければ表示しない
			if (ptr->list.empty()) break;

			// 表示項目の構築
			static const char *pList[100];
			auto it = ptr->list.begin();
			for (int i = 0; i < ptr->list.size() && i < 100; ++i, ++it)
				pList[i] = it->c_str();

			// 表示
			if (ImGui::ListBox(item->GetName(), &ptr->selectNo, pList, static_cast<int>(ptr->list.size())))
			{
				// 関数があれば選択時処理を実行
				if (ptr->func && ptr->selectNo >= 0)
					ptr->func(pList[ptr->selectNo]);
			}
		}
		break;
		}
	}
}

DebugItem &DebugWindow::operator[](std::string_view In_ItemName)
{
	auto itr = FindItem(In_ItemName);
	if (itr == m_Items.end())
		return *s_NullItem;

	return **itr;
}

void DebugWindow::RemoveItem(std::string_view In_Name)
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
