/*+===================================================================
	File: DebugWindowBase.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu PM 04:18:45 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Managers/DebugManager/Item/DebugItem.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// DebugWindowBaseクラス
/// </summary>
class ENGINE_API DebugWindowBase
{
	friend class DebugManager;

public:
	enum class WindowType
	{
		Normal,      // 通常のウィンドウ (ImGui::Begin)
		Modal,       // モーダルダイアログ (ImGui::BeginPopupModal)
		Popup        // ポップアップ (ImGui::BeginPopup)
	};

	DebugWindowBase(_In_ std::string_view In_Name, _In_ WindowType In_Type, _In_ ImGuiWindowFlags In_Flags = 0);
	virtual ~DebugWindowBase();

	/// <summary>
	/// ウィンドウを描画（派生クラスで実装）
	/// </summary>
	virtual void Draw() noexcept = 0;

	/// <summary>
	/// アイテムのインデックスアクセス
	/// </summary>
	DebugItem &operator[](_In_ std::string_view In_ItemName);

	/// <summary>
	/// 名前を指定してデバッグアイテムへのポインタを取得します
	/// </summary>
	/// <param name="[In_ItemName]">取得するデバッグアイテムの名前</param>
	/// <returns>指定された名前に対応するデバッグアイテムへのポインタ</returns>
	DebugItem *GetItemPtr(_In_ std::string_view In_ItemName);

	/// <summary>
	/// グループアイテムを取得
	/// </summary>
	ItemGroup &GetGroupItem(_In_ std::string_view In_ItemName);

	/// <summary>
	/// アイテムを作成
	/// </summary>
	template <typename T, typename ...Args>
	requires std::derived_from<T, DebugItem>
	T *CreateItem(std::string_view In_Name, Args&& ...args);

	template<typename T>
	requires std::derived_from<T, ItemLayoutFunc>
	T *CreateItem();

	/// <summary>
	/// アイテムを削除
	/// </summary>
	void RemoveItem(_In_ std::string_view In_Name);

	/// <summary>
	/// すべてのアイテムをクリア
	/// </summary>
	void ClearItems();

	// Getter/Setter
	std::string GetName() const noexcept { return m_Name; }
	std::string GetGroupName() const noexcept { return m_GroupName; }
	WindowType GetWindowType() const noexcept { return m_WindowType; }
	bool NotDummy() const noexcept { return !m_IsDummy; }
	bool IsOpen() const noexcept { return m_IsOpen; }
	void SetIsOpen(_In_ const bool In_IsOpen) noexcept { m_IsOpen = In_IsOpen; }
	void ToggleIsOpen() noexcept { m_IsOpen = !m_IsOpen; }

	// WindowFlagsの操作
	void SetWindowFlags(_In_ ImGuiWindowFlags In_Flags) noexcept { m_WindowFlags = In_Flags; }
	void AddWindowFlags(_In_ ImGuiWindowFlags In_Flags) noexcept { m_WindowFlags |= In_Flags; }
	void RemoveWindowFlags(_In_ ImGuiWindowFlags In_Flags) noexcept { m_WindowFlags &= ~In_Flags; }

protected:
	/// <summary>
	/// アイテムを検索
	/// </summary>
	auto FindItem(_In_ std::string_view In_Name) noexcept
	{
		return std::find_if(m_Items.begin(), m_Items.end(),
			[&](DebugItem *item)
			{
				return item->GetName() == In_Name;
			});
	}

	/// <summary>
	/// アイテムを描画
	/// </summary>
	void DrawItems() noexcept;

	/// <summary>
	/// アイテムの変更処理
	/// </summary>
	void ChangeItems() noexcept;

	/// <summary>
	/// フルウィンドウ名を取得
	/// </summary>
	std::string GetFullWindowName() const noexcept
	{
		return m_GroupName + "/" + m_Name;
	}

	/// <summary>
	/// ウィンドウの表示名を取得
	/// </summary>
	/// <returns>m_Nameとm_GroupNameを"##"で連結したウィンドウ名</returns>
	std::string GetViewWindowName() const noexcept
	{
		return m_Name + "##" + m_GroupName;
	}

protected:
	bool m_IsOpen;
	bool m_IsDummy;
	std::string m_GroupName;
	std::string m_Name;
	WindowType m_WindowType;
	std::vector<DebugItem *> m_Items;
	ImGuiWindowFlags m_WindowFlags;
};

// テンプレート実装
template<typename T, typename ...Args>
requires std::derived_from<T, DebugItem>
inline T *DebugWindowBase::CreateItem(std::string_view In_Name, Args && ...args)
{
	T *item = new T(In_Name.data(), std::forward<Args>(args)...);
	item->m_GroupName = m_GroupName;
	item->m_WindowName = m_Name;
	item->m_pWindow = this;
	item->Awake();

	DebugManager::GetInstance().DataRead(m_GroupName + "/" + m_Name + "/", item);

	m_Items.push_back(item);
	return item;
}

template<typename T>
	requires std::derived_from<T, ItemLayoutFunc>
inline T *DebugWindowBase::CreateItem()
{
	T *item = new T();
	item->m_GroupName = m_GroupName;
	item->m_WindowName = m_Name;
	item->m_pWindow = this;
	item->Awake();

	DebugManager::GetInstance().DataRead(m_GroupName + "/" + m_Name + "/", item);

	m_Items.push_back(item);
	return item;
}