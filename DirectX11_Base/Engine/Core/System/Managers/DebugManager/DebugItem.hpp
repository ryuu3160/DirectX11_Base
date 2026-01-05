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
class ItemLayoutFunc;

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
		LayoutFunc,	// レイアウト用関数
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

	bool GetBool();
	int GetInt();
	float GetFloat();
	DirectX::XMFLOAT2 GetVector2();
	DirectX::XMFLOAT3 GetVector();
	DirectX::XMFLOAT4 GetColor();
	std::string GetStr();

	DebugItem &operator[](_In_ std::string_view In_Name);

	// リストで使用する関数
	void AddListItem(_In_ std::string_view In_Name);
	void RemoveListItem(_In_ std::string_view In_Name);

	virtual void DrawImGui() {};

public:
	// 文字列から項目の種別を取得
	static Kind StrToKind(_In_ std::string In_Str);
	// 項目の種別から文字列を取得
	static std::string KindToStr(_In_ Kind In_Kind);


protected:
	std::string m_GroupName;
	std::string m_WindowName;
	std::string m_Name;
	Kind m_Kind;
};

/// <summary>
/// ItemValueクラス
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_Kind]">保存する型を表すKind情報</param>
/// <param name="[In_IsSave]">値を保存するかどうか</param>
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

	/// <summary>
	/// 値が変更された際に呼ばれる関数を設定
	/// </summary>
	/// <param name="[In_NoticeFunc]">対象関数へのポインタ</param>
	void SetNoticeFunc(_In_ std::function<void()> In_NoticeFunc) noexcept;

private:
	Value m_Value;
	std::function<void()> m_Notice;
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

/// <summary>
/// ItemBindクラス
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_Kind]">紐づける型を表すKind情報</param>
/// <param name="[In_IsSave]">紐づける値へのポインタ</param>
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

	/// <summary>
	/// 値が変更された際に呼ばれる関数を設定
	/// </summary>
	/// <param name="[In_NoticeFunc]">対象関数へのポインタ</param>
	void SetNoticeFunc(_In_ std::function<void()> In_NoticeFunc) noexcept;

private:
	void *m_vPtr;
	bool m_IsString;
	std::function<void()> m_Notice;
};

/// <summary>
/// ItemCallbackクラス
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_Kind]">保存する型を表すKind情報</param>
/// <param name="[In_Func]">コールバック関数</param>
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

	// グループで使用する関数

	template <typename T, typename ...Args>
	requires (std::derived_from<T, DebugItem>)
	T *CreateGroupItem(_In_ std::string_view In_Name, _In_ Args&& ...In_Args)
	{
		if(m_Kind != Kind::Group)
			return nullptr;

		T *item = new T(In_Name.data(), std::forward<Args>(In_Args)...);

		std::string Path = m_GroupName + "/" + m_WindowName + "/" + m_Name + "/";
		DataRead(Path, item);

		dynamic_cast<ItemGroup *>(this)->m_Items.push_back(static_cast<DebugItem *>(item));
		return item;
	}

	template<typename T, typename ...Args>
	requires std::derived_from<T, ItemLayoutFunc>
	T *CreateGroupItem(_In_ Args&& ...In_Args)
	{
		if(m_Kind != Kind::Group)
			return nullptr;

		T *item = new T(std::forward<Args>(In_Args)...);

		std::string Path = m_GroupName + "/" + m_WindowName + "/" + m_Name + "/";
		DataRead(Path, item);

		dynamic_cast<ItemGroup *>(this)->m_Items.push_back(static_cast<DebugItem *>(item));
		return item;
	}

	template<typename T>
	requires std::derived_from<T, ItemLayoutFunc>
	T* CreateGroupItem()
	{
		if (m_Kind != Kind::Group)
			return nullptr;
		T* item = new T();
		std::string Path = m_GroupName + "/" + m_WindowName + "/" + m_Name + "/";
		DataRead(Path, item);
		dynamic_cast<ItemGroup*>(this)->m_Items.push_back(static_cast<DebugItem*>(item));
		return item;
	}

private:
	void DataRead(_In_ std::string_view In_FullPath, _In_ DebugItem *In_pItem);
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

	void AddLevel(_In_ std::string_view In_Name, _In_ const ImVec4 &In_Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	void AddOutput(_In_ std::string_view In_Text, _In_ const ImVec4 &In_Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), _In_ std::string_view In_Level = "Default");

	void SetShowClearButton(_In_ bool In_IsShow) { m_IsShowClearButton = In_IsShow; }
	void SetShowAutoScrollButton(_In_ bool In_IsShow) { m_IsShowAutoScrollButton = In_IsShow; }
	void SetShowSerchBox(_In_ bool In_IsShow) { m_IsShowSerchBox = In_IsShow; }

	void SetOutputFolderPath(_In_ std::string_view In_FolderPath) { m_FolderPath = In_FolderPath.data(); }

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

/// <summary>
/// ItemLayoutFuncクラス
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_LayoutType]">レイアウトの種類</param>
class ItemLayoutFunc : public DebugItem
{
public:
	enum LayoutType
	{
		SameLine,					// 同一行に表示
		NewLine,					// SameLineの解除（改行）
		Spacing,					// ウィジェット間に空白を入れる
		Separator,					// セパレータを入れる
		Indent,						// インデントを追加
		UnIndent,					// インデントをもとに戻す
		Dummy,						// 見えないスペーサー
		SetNextItemWidth,			// 次のアイテムの幅を設定
		AlignTextToFramePadding,	// テキストをフレームパディングに合わせる
	};

	ItemLayoutFunc(_In_ std::string In_Name, _In_ LayoutType In_LayoutType);
	~ItemLayoutFunc();
	virtual void DrawImGui() override {};

private:
	LayoutType m_LayoutType;
};

/// <summary>
/// ImGuiの同一行にアイテムを配置するためのレイアウト機能を提供する
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_OffsetX]">同一行に配置する際のX方向のオフセット値</param>
/// <param name="[In_SpacingW]">同一行に配置する際の間隔幅（負の値の場合はデフォルト値を使用）</param>
class ItemSameLine : public ItemLayoutFunc
{
public:
	ItemSameLine(_In_ std::string In_Name = "SameLine", _In_ float In_OffsetX = 0.0f, _In_ float In_SpacingW = -1.0f);
	~ItemSameLine();
	void DrawImGui() override;
private:
	float m_OffsetX;
	float m_SpacingW;
};

/// <summary>
/// ImGuiの改行を行うためのレイアウト機能を提供する
/// </summary>
class ItemNewLine : public ItemLayoutFunc
{
public:
	ItemNewLine(_In_ std::string In_Name = "NewLine");
	~ItemNewLine();
	void DrawImGui() override;
};

/// <summary>
/// ImGuiのウィジェット間に空白を入れるためのレイアウト機能を提供する
/// </summary>
class ItemSpacing : public ItemLayoutFunc
{
public:
	ItemSpacing(_In_ std::string In_Name = "Spacing", _In_ int In_SpaceNum = 1);
	~ItemSpacing();
	void DrawImGui() override;
private:
	int m_SpaceNum;
};

/// <summary>
/// ImGuiのセパレータを描画するためのレイアウト機能を提供する
/// </summary>
class ItemSeparator : public ItemLayoutFunc
{
public:
	ItemSeparator(_In_ std::string In_Name = "Separator");
	~ItemSeparator();
	void DrawImGui() override;
};

/// <summary>
/// ImGuiのインデントを追加するためのレイアウト機能を提供する
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_IndentW]">インデント幅（負の値の場合はデフォルト値を使用）</param>
class ItemIndent : public ItemLayoutFunc
{
public:
	ItemIndent(_In_ std::string In_Name = "Indent", _In_ float In_IndentW = 0.0f);
	~ItemIndent();
	void DrawImGui() override;
private:
	float m_IndentW;
};

/// <summary>
/// ImGuiのインデントを元に戻すためのレイアウト機能を提供する
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// param name="[In_IndentW]">インデント幅（負の値の場合はデフォルト値を使用）</param>
class ItemUnIndent : public ItemLayoutFunc
{
public:
	ItemUnIndent(_In_ std::string In_Name = "UnIndent", _In_ float In_IndentW = 0.0f);
	~ItemUnIndent();
	void DrawImGui() override;
private:
	float m_IndentW;
};

/// <summary>
/// ImGuiの見えないスペーサーを描画するためのレイアウト機能を提供する
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_Width]">スペーサーの幅</param>
/// <param name="[In_Height]">スペーサーの高さ</param>
class ItemDummy : public ItemLayoutFunc
{
public:
	ItemDummy(_In_ std::string In_Name = "Dummy", _In_ float In_Width = 0.0f, _In_ float In_Height = 0.0f);
	~ItemDummy();
	void DrawImGui() override;
private:
	float m_Width;
	float m_Height;
};

/// <summary>
/// ImGuiの次のアイテムの幅を設定するためのレイアウト機能を提供する
///	</summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_Width]">次のアイテムの幅</param>
class ItemSetNextItemWidth : public ItemLayoutFunc
{
public:
	ItemSetNextItemWidth(_In_ std::string In_Name = "NextItemWidth", _In_ float In_Width = 16.0f);
	~ItemSetNextItemWidth();
	void DrawImGui() override;
private:
	float m_Width;
};

class ItemAlignTextToFramePadding : public ItemLayoutFunc
{
public:
	ItemAlignTextToFramePadding(_In_ std::string In_Name = "ItemAlignTextToFramePadding");
	~ItemAlignTextToFramePadding();
	void DrawImGui() override;
};
