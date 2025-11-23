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
#include "System/DebugManager/InitializeImGui.hpp"
#include "System/DebugManager/DebugWindow.hpp"
#include "System/DebugManager/DebugItem.hpp"

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
public:
	struct SaveData
	{
		DebugItem::Kind kind;
		std::string path;
		std::string value;
	};

public:
	DebugManager();
	~DebugManager();

	void Init();

	void Update() noexcept;

	void Draw() noexcept;

	/// <summary>
	/// デバッグウィンドウを作成する
	/// </summary>
	/// <param name="[In_Name]">ウィンドウ名</param>
	/// <returns>作成されたウィンドウへのポインタ</returns>
	DebugWindow *CreateDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name);

	/// <summary>
	/// 指定した名前に対応するデバッグウィンドウを取得します。
	/// </summary>
	/// <param name="[In_Name]">検索するデバッグウィンドウの名前を示す読み取り専用の std::string_view。</param>
	/// <returns>該当する DebugWindow へのポインタ。指定した名前のウィンドウが見つからない場合は nullptr を返します。</returns>
	DebugWindow *GetDebugWindow(_In_ const std::string_view In_GroupName, _In_ const std::string_view In_Name);

	template<typename ...Args>
	void DebugLog(_In_ const char* In_Format, Args&& ...args)
	{
		std::string msg = std::vformat(In_Format, std::make_format_args(args...));

		auto window = GetDebugWindow("System", "Log");
		if(window->NotDummy())
		{
			auto &item = (*window)["LogText"];
			if (item.GetKind() == DebugItem::Kind::InputStr)
			{
				auto TextItem = dynamic_cast<ItemText *>(&item);
				if (TextItem)
				{
					std::string &text = TextItem->GetText();
					text += msg + "\n";

					// 文字数制限
					if (TextItem->IsMultiline())
					{
						if (text.length() > 4096)
							text = text.substr(text.find('\n') + 1);
					}
					else
					{
						if (text.length() > 256)
							text = text.substr(text.find('\n'));
					}
				}
			}
		}
	}

	void DataRead(_In_ std::string In_Path, _Inout_ DebugItem *Inout_Item);

private:

	void SaveDebugData();
	void DataWrite(_Inout_opt_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugItem *In_Item);
	void WindowDataWrite(_Inout_ std::string &Inout_Data, _In_ std::string In_Path, _In_ DebugWindow *In_Window);

	void LoadDebugData();

	void WindowDataRead(_In_ std::string In_Path, _Inout_ DebugWindow *Inout_Window);

private:
	ImGuiWindowFlags m_ToolBarFlags;
	std::unordered_map<std::string, std::vector<DebugWindow*>> m_DebugWindows;
	std::vector<SaveData> m_SaveData;
};
