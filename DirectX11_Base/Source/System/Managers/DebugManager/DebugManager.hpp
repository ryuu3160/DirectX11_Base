/*+===================================================================
	File: DebugManager.hpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:16 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Managers/DebugManager/InitializeImGui.hpp"
#include "System/Managers/DebugManager/DebugWindow.hpp"
#include "System/Managers/DebugManager/DebugItem.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// DebugManagerクラス
/// </summary>
class DebugManager : public Singleton<DebugManager>
{
	friend class Singleton<DebugManager>;

	struct ToolBarMenu
	{
		std::string Name;

		std::function<void()> Func;
	};

public:
	struct SaveData
	{
		DebugItem::Kind kind = DebugItem::Label;
		std::string path;
		std::string value;
	};

public:
	void Init();

	void Update() noexcept;

	void Draw() noexcept;

	/// <summary>
	/// デバッグウィンドウを作成する
	/// </summary>
	/// <param name="[In_Name]">ウィンドウ名</param>
	/// <returns>作成されたウィンドウへのポインタ</returns>
	DebugWindow *CreateDebugWindow(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name);

	/// <summary>
	/// ツールバーにメニュー項目を追加する
	/// </summary>
	/// <param name="[In_GroupName]">グループ名</param>
	/// <param name="[In_Name]">追加するメニュー項目の表示名</param>
	/// <param name="[In_Func]">メニュー項目が選択されたときに実行されるコールバック関数（引数なし、戻り値なし）。</param>
	void AddToolBarMenu(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name, _In_ std::function<void()> In_Func);

	/// <summary>
	/// 指定した名前に対応するデバッグウィンドウを取得する
	/// </summary>
	/// <param name="[In_GroupName]">検索するデバッグウィンドウの所属するグループ名</param>
	/// <param name="[In_Name]">検索するデバッグウィンドウの名前を示す読み取り専用のstd::string_view</param>
	/// <returns>該当する DebugWindow へのポインタ。指定した名前のウィンドウが見つからない場合はnullptrを返します</returns>
	DebugWindow *GetDebugWindow(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name);

	/// <summary>
	/// 指定した名前に対応するデバッグウィンドウへの参照を取得する
	/// </summary>
	/// <param name="[In_GroupName]">グループ名</param>
	/// <param name="[In_Name]">>ウィンドウ名</param>
	/// <returns></returns>
	DebugWindow &GetDebugWindowRef(_In_ std::string_view In_GroupName, _In_ std::string_view In_Name);

	template<typename ...Args>
	inline void DebugLog(_In_ const char* In_Format, Args&& ...args)
	{
		std::string msg = std::vformat(In_Format, std::make_format_args(args...));
		msg = ShiftJISToUTF8(msg);
		auto window = GetDebugWindow("System", "Log");
		if(window->NotDummy())
		{
			auto &item = (*window)["ConsoleLog"];
			if (item.GetKind() == DebugItem::Kind::Console)
			{
				auto TextItem = dynamic_cast<ItemConsole *>(&item);
				if (TextItem)
				{
					TextItem->AddOutput(msg);
				}
			}
		}
	}

	template<typename ...Args>
	inline void DebugLogWarning(_In_ const char *In_Format, Args&& ...args)
	{
		std::string msg = std::vformat(In_Format, std::make_format_args(args...));
		msg = ShiftJISToUTF8(msg);
		auto window = GetDebugWindow("System", "Log");
		if (window->NotDummy())
		{
			auto &item = (*window)["ConsoleLog"];
			if (item.GetKind() == DebugItem::Kind::Console)
			{
				auto TextItem = dynamic_cast<ItemConsole *>(&item);
				if (TextItem)
				{
					TextItem->AddOutput(msg, ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "Warning");
				}
			}
		}
	}

	template<typename ...Args>
	inline void DebugLogError(_In_ const char *In_Format, Args&& ...args)
	{
		std::string msg = std::vformat(In_Format, std::make_format_args(args...));
		msg = ShiftJISToUTF8(msg);
		auto window = GetDebugWindow("System", "Log");
		if (window->NotDummy())
		{
			auto &item = (*window)["ConsoleLog"];
			if (item.GetKind() == DebugItem::Kind::Console)
			{
				auto TextItem = dynamic_cast<ItemConsole *>(&item);
				if (TextItem)
				{
					TextItem->AddOutput(msg, ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error");
				}
			}
		}
	}

	void DataRead(_In_ std::string In_Path, _Inout_ DebugItem *Inout_Item);

private:
	DebugManager();
	~DebugManager();

	void HideAllWindows();
	void ShowAllWindows();

	void SaveDebugData();
	void DataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugItem *In_Item);
	void WindowDataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugWindow *In_Window);

	void LoadDebugData();

	void WindowDataRead(_In_ std::string In_Path, _Inout_ DebugWindow *Inout_Window);

	std::string CharacterLimitRecursion(_In_ std::string In_Text, _In_ int In_LimitNum);

private:
	ImGuiWindowFlags m_ToolBarFlags;
	std::unordered_map<std::string, std::vector<ToolBarMenu>> m_ToolBarFuncs;
	std::vector<DebugWindow*> m_DebugWindows;
	std::vector<SaveData> m_SaveData;
	bool m_IsRequestLoadLayout;
	bool m_IsRequestSaveLayout;
};
