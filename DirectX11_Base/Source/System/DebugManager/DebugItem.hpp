/*+===================================================================
	File: DebugItem.hpp
	Summary: デバッグ項目クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:00:12 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "InitializeImGui.hpp"
// ==============================
//  前方宣言
// ==============================
class ItemGroup;

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// DebugItemクラス
/// </summary>
class DebugItem
{
	friend class DebugWindow;
public:
	using CallBack = std::function<void(bool, void *)>;
	using ConstCallback = std::function<void(const void *)>;
	using Value = std::variant<
		bool,
		int,
		float,
		DirectX::XMFLOAT2,
		DirectX::XMFLOAT3,
		DirectX::XMFLOAT4,
		std::string
	>;

	enum Kind {
		Label,		// 項目のみの表示
		Bool,		// チェックボックス
		Int,		// 整数入力
		Float,		// 小数入力
		Float2,		// 2Dベクター入力
		Vector,		// ベクター入力
		Color,		// 色入力
		Path,		// ファイルパスの指定
		InputStr,	// 文字列入力
		Console,	// 文字列表示
		Command,	// ボタン
		Group,		// 表示項目をまとめる
		List,		// 一覧表示
	};

protected:
	DebugItem();

public:
	virtual ~DebugItem();

	std::string GetName() const;

	const char *GetCStrName() const;

	/// <summary>
	/// 項目の種別を取得
	/// </summary>
	/// <returns>種類</returns>
	Kind GetKind() const;

	bool GetBool() const;
	int GetInt() const;
	float GetFloat() const;
	DirectX::XMFLOAT2 GetVector2() const;
	DirectX::XMFLOAT3 GetVector() const;
	DirectX::XMFLOAT4 GetColor() const;
	std::string GetStr() const;

	// グループで使用する関数
	template <typename T, typename ...Args>
		requires std::derived_from<T, DebugItem>
	T *CreateGroupItem(_In_ const std::string_view In_Name, _In_ Args&& ...args);

	DebugItem &operator[](_In_ const std::string_view In_Name);

	// リストで使用する関数
	void AddListItem(_In_ const std::string_view In_Name);
	void RemoveListItem(_In_ const std::string_view In_Name);

	virtual void DrawImGui() {};

public:
	// 文字列から項目の種別を取得
	static Kind StrToKind(_In_ std::string In_Str);
	// 項目の種別から文字列を取得
	static std::string KindToStr(_In_ Kind In_Kind);


protected:
	std::string m_Name;
	Kind m_Kind;
};

class ItemValue : public DebugItem
{
public:
	ItemValue(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ bool In_IsSave = false);
	~ItemValue();

	void DrawImGui() override;

	bool IsSave() const { return m_IsSave; }

	Value &GetValue() { return m_Value; }

	template<typename T>
	requires TypePOD<T> || TypeString<T> || std::is_same_v<T, DirectX::XMFLOAT2> || std::is_same_v<T, DirectX::XMFLOAT3> || std::is_same_v<T, DirectX::XMFLOAT4>
	T &GetValue()
	{
		return std::get<T>(m_Value);
	}

private:
	Value m_Value;
	bool m_IsSave;
};

class ItemText : public DebugItem
{
public:
	ItemText(_In_ std::string In_Name, _In_ bool In_IsMultiline, _In_ ImGuiInputTextFlags In_Flags, _In_ bool Is_HideLabel, _In_ bool In_IsSave);
	~ItemText();

	void DrawImGui() override;

	std::string &GetText() { return m_Text; }
	ImGuiInputTextFlags GetFlags() const { return m_Flags; }
	bool IsMultiline() const { return m_IsMultiline; }
	bool IsHideLabel() const { return m_IsHideLabel; }
	bool IsSave() const { return m_IsSave; }
	int GetLineCount() const { return m_LineCount; }

	/// <summary>
	/// <para>マルチライン時の行数を設定</para>
	/// <para>0の場合は描画時余っているスペース全てを使用</para>
	/// </summary>
	/// <param name="In_LineCount"></param>
	void SetLineCount(_In_ int In_LineCount = 0) { m_LineCount = In_LineCount; }

private:
	std::string m_Text;
	ImGuiInputTextFlags m_Flags;
	int m_LineCount;
	bool m_IsMultiline;
	bool m_IsHideLabel;
	bool m_IsSave;
};

class ItemBind : public DebugItem
{
public:
	ItemBind(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ void *In_Ptr);
	ItemBind(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ std::string* In_Ptr);
	~ItemBind();

	void DrawImGui() override;

	template <typename T>
	T *GetPtr()
	{
		return reinterpret_cast<T *>(m_vPtr);
	}

	template<typename T>
	requires std::is_same_v<T,char>
	T *GetPtr()
	{
		if(m_IsString)
			return reinterpret_cast<T *>(static_cast<std::string *>(m_vPtr)->data());
		else
			return reinterpret_cast<T *>(m_vPtr);
	}

	void SetNoticeFunc(_In_ std::function<void()> In_NoticeFunc) noexcept;

private:
	void *m_vPtr;
	bool m_IsString;
	std::function<void()> m_Notice;
};

class ItemCallback : public DebugItem
{
public:
	ItemCallback(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ CallBack In_Func);
	~ItemCallback();

	void DrawImGui() override;

	Value &GetValue() { return m_Value; }
	void CallFunc(_In_ bool In_IsSet, _In_opt_ void *In_Ptr) { m_Func(In_IsSet, In_Ptr); }

private:
	Value m_Value;
	CallBack m_Func;
};

class ItemGroup : public DebugItem
{
	friend class DebugItem;
public:
	ItemGroup(_In_ std::string In_Name);
	~ItemGroup();

	void DrawImGui() override;

	std::vector<DebugItem *> &GetGroupItems() { return m_Items; }

private:
	std::vector<DebugItem *> m_Items;
};

class ItemList : public DebugItem
{
public:
	ItemList(_In_ std::string In_Name, _In_ ConstCallback In_Func = nullptr, _In_ bool In_IsSave = false);
	~ItemList();

	void DrawImGui() override;

	int &GetSelectNo() { return m_SelectNo; }
	std::list<std::string> &GetList() { return m_List; }
	ConstCallback GetFunc() { return m_Func; }
	bool IsSave() const { return m_IsSave; }

private:
	std::list<std::string>	m_List;		// アイテム一覧
	int						m_SelectNo;	// 選択項目番号
	ConstCallback			m_Func;		// 項目選択時のコールバック
	bool					m_IsSave;	// 選択番号の保存
};

/// <summary>
/// ItemConsoleクラス
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_IsOutputLogFile]">ログファイルに出力するかどうかのbool値</param>
class ItemConsole : public DebugItem
{
public:
	ItemConsole(_In_ std::string In_Name, _In_ bool In_IsOutputLogFile);
	~ItemConsole();
	void DrawImGui() override;

	void AddLevel(_In_ const std::string_view In_Name, _In_ const ImVec4 &In_Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	void AddOutput(_In_ const std::string_view In_Text, _In_ const ImVec4 &In_Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), _In_ const std::string_view In_Level = "Default");

	void SetShowClearButton(_In_ bool In_IsShow) { m_IsShowClearButton = In_IsShow; }
	void SetShowAutoScrollButton(_In_ bool In_IsShow) { m_IsShowAutoScrollButton = In_IsShow; }
	void SetShowSerchBox(_In_ bool In_IsShow) { m_IsShowSerchBox = In_IsShow; }

	void SetOutputFolderPath(_In_ const std::string_view In_FolderPath) { m_FolderPath = In_FolderPath.data(); }

private:
	struct OutputData
	{
		std::string Text;
		std::string TimeStr;
		std::string Level;
		ImVec4 Color;
	};

	struct LevelData
	{
		ImVec4 Color;
		bool IsShow{ true };
	};

	std::string CurrentTimeString();
	int GetButtonCount() const;

	void WriteLogToFile();

private:
	std::vector<OutputData> m_Outputs;
	std::unordered_map<std::string, LevelData> m_Levels;
	std::string m_FolderPath;

	std::mutex m_Mutex;
	char m_SerchBuffer[256];
	bool m_IsShowClearButton;
	bool m_IsShowAutoScrollButton;
	bool m_IsShowSerchBox;
	bool m_IsAutoScroll;
	bool m_ScrollToBottom;

	bool m_IsOutputLogFile;
};



// -------------------------------
//	テンプレート関数実装
// -------------------------------

template<typename T, typename ...Args>
requires std::derived_from<T, DebugItem>
inline T *DebugItem::CreateGroupItem(const std::string_view In_Name, Args && ...args)
{
	if (m_Kind != Kind::Group)
		return nullptr;

	T *item = new T(In_Name.data(), std::forward<Args>(args)...);
	dynamic_cast<ItemGroup *>(this)->m_Items.push_back(static_cast<DebugItem *>(item));
	return item;
}
