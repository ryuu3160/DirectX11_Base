/*+===================================================================
	File: DebugWindow.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:34:04 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Managers/DebugManager/DebugItem.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// DebugWindowクラス
/// </summary>
class DebugWindow
{
	friend class DebugManager;
public:
	DebugWindow(_In_ std::string_view In_Name);
	~DebugWindow();

	void Draw() noexcept;

	void ChangeItems() noexcept;

	DebugItem &operator[](_In_ std::string_view In_ItemName);

	ItemGroup &GetGroupItem(_In_ std::string_view In_ItemName);

	template <typename T, typename ...Args>
	requires std::derived_from<T, DebugItem>
	T *CreateItem(std::string_view In_Name, Args&& ...args);

	template<typename T, typename ...Args>
	requires std::derived_from<T, ItemLayoutFunc>
	T *CreateItem();

	void RemoveItem(_In_ std::string_view In_Name);

	void ClearItems();

	std::string GetName() const noexcept { return m_Name; }
	std::string GetGroupName() const noexcept { return m_GroupName; }

	bool NotDummy() const noexcept { return !m_IsDummy; }

	bool IsOpen() const noexcept { return m_IsOpen; }

	void SetIsOpen(_In_ const bool In_IsOpen) noexcept { m_IsOpen = In_IsOpen; }
	void ToggleIsOpen() noexcept { m_IsOpen = !m_IsOpen; }

private:

	auto FindItem(_In_ std::string_view In_Name) noexcept
	{
		return std::find_if(m_Items.begin(), m_Items.end(),
			[&](DebugItem *item)
			{
				return item->GetName() == In_Name;
			});
	}

	void DrawImgui(_In_ DebugItem *In_Item) noexcept;

private:
	bool m_IsOpen;
	bool m_IsDummy;
	std::string m_GroupName;
	std::string m_Name;
	std::vector<DebugItem *> m_Items;
};

template<typename T, typename ...Args>
requires std::derived_from<T, DebugItem>
inline T *DebugWindow::CreateItem(std::string_view In_Name, Args && ...args)
{
	T *item = new T(In_Name.data(), std::forward<Args>(args)...);
	item->m_GroupName = m_GroupName;
	item->m_WindowName = m_Name;

	DebugManager::GetInstance().DataRead(m_GroupName + "/" + m_Name + "/", item);

	m_Items.push_back(item);
	return item;
}

template<typename T, typename ...Args>
requires std::derived_from<T, ItemLayoutFunc>
inline T *DebugWindow::CreateItem()
{
	T *item = new T();
	item->m_GroupName = m_GroupName;
	item->m_WindowName = m_Name;

	DebugManager::GetInstance().DataRead(m_GroupName + "/" + m_Name + "/", item);

	m_Items.push_back(item);
	return item;
}
