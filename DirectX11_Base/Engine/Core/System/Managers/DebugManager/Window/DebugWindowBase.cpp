/*+===================================================================
	File: DebugWindowBase.cpp
	Summary:
	Author: AT13C192 01 Â–Ø—Yˆê˜Y
	Date: 2026/3/12 Thu PM 04:18:54 ‰‰ñì¬
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugWindowBase.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

// ==============================
//	’è”’è‹`
// ==============================
namespace
{
	static DebugItem *s_NullItem = nullptr;
}

DebugWindowBase::DebugWindowBase(_In_ std::string_view In_Name, _In_ WindowType In_Type, _In_ ImGuiWindowFlags In_Flags)
	: m_Name(In_Name), m_WindowType(In_Type), m_WindowFlags(In_Flags), m_IsOpen(true), m_IsDummy(false)
{
}

DebugWindowBase::~DebugWindowBase()
{
	for(auto item : m_Items)
	{
		delete item;
	}
	m_Items.clear();
}

void DebugWindowBase::DrawItems() noexcept
{
	for(const auto &item : m_Items)
	{
		if(item)
			item->DrawImGui();
	}
}

void DebugWindowBase::ChangeItems() noexcept
{
	for(const auto &item : m_Items)
	{
		if(item)
			item->ChangeImGuiItem();
	}
}

DebugItem &DebugWindowBase::operator[](_In_ std::string_view In_ItemName)
{
	auto itr = FindItem(In_ItemName);
	if(itr == m_Items.end())
		return *s_NullItem;

	return **itr;
}

DebugItem *DebugWindowBase::GetItemPtr(_In_ std::string_view In_ItemName)
{
	auto itr = FindItem(In_ItemName);
	if(itr == m_Items.end())
		return s_NullItem;
	return *itr;
}

ItemGroup &DebugWindowBase::GetGroupItem(_In_ std::string_view In_ItemName)
{
	auto itr = FindItem(In_ItemName);
	if(itr == m_Items.end())
		return *static_cast<ItemGroup *>(s_NullItem);

	ItemGroup *pGroup = dynamic_cast<ItemGroup *>(*itr);
	if(!pGroup)
		return *static_cast<ItemGroup *>(s_NullItem);
	return *pGroup;
}

void DebugWindowBase::RemoveItem(_In_ std::string_view In_Name)
{
	auto itr = FindItem(In_Name);
	if(itr != m_Items.end())
	{
		delete *itr;
		m_Items.erase(itr);
	}
}

void DebugWindowBase::ClearItems()
{
	for(auto item : m_Items)
	{
		delete item;
	}
	m_Items.clear();
}