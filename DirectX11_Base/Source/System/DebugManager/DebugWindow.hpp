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
#include "System/DebugManager/DebugItem.hpp"

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
public:
	DebugWindow(_In_ const std::string_view In_Name);
	~DebugWindow();

	void Draw() noexcept;

	DebugItem &operator[](_In_ std::string_view In_ItemName);

	template <typename T, typename ...Args>
	requires std::derived_from<T, DebugItem>
	T *CreateItem(const std::string_view In_Name, Args&& ...args);

	void RemoveItem(_In_ std::string_view In_Name);

	void ClearItems();

	std::string GetName() const noexcept { return m_Name; }

	bool IsOpen() const noexcept { return m_IsOpen; }

	void SetIsOpen(_In_ const bool In_IsOpen) noexcept { m_IsOpen = In_IsOpen; }
	void EnableOpen() noexcept { m_IsOpen = true; }
	void DisableOpen() noexcept { m_IsOpen = false; }

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
	std::string m_Name;
	std::vector<DebugItem *> m_Items;
};

template<typename T, typename ...Args>
requires std::derived_from<T, DebugItem>
inline T *DebugWindow::CreateItem(const std::string_view In_Name, Args && ...args)
{
	T *item = new T(In_Name.data(), std::forward<Args>(args)...);

	m_Items.push_back(static_cast<DebugItem*>(item));
	return item;
}
