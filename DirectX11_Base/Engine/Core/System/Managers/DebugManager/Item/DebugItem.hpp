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
#include "Core/System/Managers/DebugManager/InitializeImGui.hpp"
#include "Core/System/Utility/Concept.hpp"
// ==============================
//  前方宣言
// ==============================
class ItemGroup;
class ItemLayoutFunc;

// ==============================
//  定数
// ==============================
namespace
{
	constexpr ImVec4 ImGuiColor_Red(1.0f, 0.0f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Green(0.0f, 1.0f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Blue(0.0f, 0.0f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Yellow(1.0f, 1.0f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Cyan(0.0f, 1.0f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Magenta(1.0f, 0.0f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_White(1.0f, 1.0f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Black(0.0f, 0.0f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Gray(0.5f, 0.5f, 0.5f, 1.0f);
	constexpr ImVec4 ImGuiColor_Transparent(0.0f, 0.0f, 0.0f, 0.0f);
	constexpr ImVec4 ImGuiColor_Orange(1.0f, 0.65f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Purple(0.5f, 0.0f, 0.5f, 1.0f);
	constexpr ImVec4 ImGuiColor_Brown(0.6f, 0.3f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_Pink(1.0f, 0.75f, 0.8f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightBlue(0.68f, 0.85f, 0.9f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightGreen(0.56f, 0.93f, 0.56f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightYellow(1.0f, 1.0f, 0.88f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightCyan(0.88f, 1.0f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightMagenta(1.0f, 0.88f, 1.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_LightGray(0.83f, 0.83f, 0.83f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkRed(0.5f, 0.0f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkGreen(0.0f, 0.5f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkBlue(0.0f, 0.0f, 0.5f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkYellow(0.5f, 0.5f, 0.0f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkCyan(0.0f, 0.5f, 0.5f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkMagenta(0.5f, 0.0f, 0.5f, 1.0f);
	constexpr ImVec4 ImGuiColor_DarkGray(0.25f, 0.25f, 0.25f, 1.0f);
	constexpr ImVec4 ImGuiColor_Silver(0.75f, 0.75f, 0.75f, 1.0f);
	constexpr ImVec4 ImGuiColor_Gold(1.0f, 0.84f, 0.0f, 1.0f);
}

/// <summary>
/// DebugItemクラス
/// </summary>
class DebugItem
{
	friend class DebugWindowBase;
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
		Text,		// テキスト
		InputStr,	// 文字列入力
		Console,	// 文字列表示
		Command,	// ボタン
		Group,		// 表示項目をまとめる
		List,		// 一覧表示
		LayoutFunc,	// レイアウト用関数

		// システム専用
		__Hierarchy, // ヒエラルキーウィンドウ
		__ComponentSelector, // コンポーネントセレクター
		__ProjectWindow, // プロジェクトウィンドウ
		__NewProject, // 新規プロジェクト作成ウィンドウ
	};

protected:
	DebugItem();

public:
	virtual ~DebugItem();

	virtual void Awake() {};

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

	virtual void ChangeImGuiItem() {};

public:
	// 文字列から項目の種別を取得
	static Kind StrToKind(_In_ std::string_view In_Str);
	// 項目の種別から文字列を取得
	static std::string KindToStr(_In_ Kind In_Kind);


protected:
	std::string m_GroupName;
	std::string m_WindowName;
	std::string m_Name;
	Kind m_Kind;
	DebugWindowBase *m_pWindow;
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
	ItemValue(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ bool In_IsSave = false);
	~ItemValue();

	void DrawImGui() override;

	bool IsSave() const { return m_IsSave; }

	/// <summary>
	/// ドラッグ機能を有効または無効にします
	/// </summary>
	/// <param name="[In_Enable]">ドラッグ機能を有効にするかどうか</param>
	/// <param name="[In_DragSpeed]">ドラッグの速度</param>
	/// <param name="[In_DragMin]">ドラッグの最小値</param>
	/// <param name="[In_DragMax]">ドラッグの最大値</param>
	void EnableDrag(_In_ bool In_Enable, _In_ float In_DragSpeed = 1.0f, _In_ float In_DragMin = 0.0f, _In_ float In_DragMax = 0.0f);

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

	void DrawInputValue();
	void DrawDragValue();

private:
	Value m_Value;
	std::function<void()> m_Notice;
	float m_DragSpeed;
	float m_DragMin;
	float m_DragMax;
	bool m_IsSave;
	bool m_EnableDrag;
};

/// <summary>
/// テキスト表示
/// </summary>
/// <param name="[In_Text]">表示するテキスト</param>
/// <param name="[In_IsWrapped]">折り返すかどうか</param>
/// <param name="[In_IsBullet]">箇条書き表示にするかどうか</param>
/// <param name="[In_IsSave]">値を保存するかどうか</param>
class ItemText : public DebugItem
{
public:
	ItemText(_In_ std::string_view In_Text, _In_ bool In_IsWrapped = false, _In_ bool In_IsBullet = false, _In_ bool In_IsSave = false);
	~ItemText();

	void DrawImGui() override;

	inline std::string GetText() { return m_Text; }

	inline void SetText(_In_ std::string_view In_Text) { m_Text = In_Text; }

	/// <summary>
	/// 折り返すかどうかを取得
	/// </summary>
	inline bool IsWrapped() const { return m_IsWrapped; }
	/// <summary>
	/// 折り返すかどうかを設定
	/// </summary>
	inline void SetWrapped(_In_ bool In_IsWrapped) { m_IsWrapped = In_IsWrapped; }
	
	/// <summary>
	/// 箇条書き表示にするかどうかを取得
	/// </summary>
	inline bool IsBullet() const { return m_IsBullet; }
	/// <summary>
	/// 箇条書き表示にするかどうかを設定
	/// </summary>
	inline void SetBullet(_In_ bool In_IsBullet) { m_IsBullet = In_IsBullet; }

	/// <summary>
	/// 色を取得
	/// </summary>
	inline ImVec4 GetColor() const { return m_Color; }

	/// <summary>
	/// 色を設定
	/// </summary>
	inline void SetColor(_In_ ImVec4 In_Color)
	{
		m_Color = In_Color;
		m_IsChangeColor = true;
	}

	/// <summary>
	/// 色をリセット
	/// </summary>
	inline void ResetColor()
	{
		m_Color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		m_IsChangeColor = false;
	}

	inline bool IsSave() const { return m_IsSave; }
	inline int GetLineCount() const { return m_LineCount; }

	/// <summary>
	/// <para>マルチライン時の行数を設定</para>
	/// <para>0の場合は描画時余っているスペース全てを使用</para>
	/// </summary>
	/// <param name="In_LineCount"></param>
	inline void SetLineCount(_In_ int In_LineCount = 0) { m_LineCount = In_LineCount; }

private:
	std::string m_Text;
	ImVec4 m_Color;
	int m_LineCount;
	bool m_IsChangeColor;
	bool m_IsWrapped;
	bool m_IsBullet;
	bool m_IsSave;
};

/// <summary>
/// テキスト入力項目
/// </summary>
/// <param name="[In_Name]">アイテム名</param>
/// <param name="[In_IsMultiline]">マルチライン入力かどうか</param>
/// <param name="[In_Flags]">ImGuiInputTextFlagsのフラグ</param>
/// <param name="[Is_HideLabel]">ラベルを非表示にするかどうか</param>
/// <param name="[In_IsSave]">値を保存するかどうか</param>
class ItemInputText : public DebugItem
{
public:
	ItemInputText(_In_ std::string_view In_Name, _In_ bool In_IsMultiline, _In_ ImGuiInputTextFlags In_Flags, _In_ bool Is_HideLabel, _In_ bool In_IsSave);
	~ItemInputText();

	void DrawImGui() override;

	std::string &GetText() { return m_Text; }

	/// <summary>
	/// ImGuiInputTextFlagsのフラグを取得
	/// </summary>
	ImGuiInputTextFlags GetFlags() const { return m_Flags; }

	/// <summary>
	/// ImGuiInputTextFlagsのフラグを設定
	/// </summary>
	void SetFlags(_In_ ImGuiInputTextFlags In_Flags) { m_Flags = In_Flags; }

	/// <summary>
	/// ImGuiInputTextFlagsのフラグを追加
	/// </summary>
	void AddFlags(_In_ ImGuiInputTextFlags In_Flags) { m_Flags |= In_Flags; }

	/// <summary>
	/// 複数行入力かどうかを取得
	/// </summary>
	bool IsMultiline() const { return m_IsMultiline; }

	/// <summary>
	/// 複数行入力かどうかを設定
	/// </summary>
	void SetIsMultiline(_In_ bool In_IsMultiline) { m_IsMultiline = In_IsMultiline; }

	/// <summary>
	/// ラベルを非表示にするかどうかを取得
	/// </summary>
	bool IsHideLabel() const { return m_IsHideLabel; }

	/// <summary>
	/// ラベルを非表示にするかどうかを設定
	/// </summary>
	void SetHideLabel(_In_ bool In_IsHideLabel) { m_IsHideLabel = In_IsHideLabel; }

	/// <summary>
	/// 値を保存するかどうかを取得
	/// </summary>
	bool IsSave() const { return m_IsSave; }

	/// <summary>
	/// 値を保存するかどうかを設定
	/// </summary>
	void SetIsSave(_In_ bool In_IsSave) { m_IsSave = In_IsSave; }

	/// <summary>
	/// <para>マルチライン時の行数を取得</para>
	/// <para>0の場合は描画時余っているスペース全てを使用</para>
	/// </summary>
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
	ItemBind(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ void *In_Ptr);
	ItemBind(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ std::string* In_Ptr);
	~ItemBind();

	/// <summary>
	/// ドラッグ機能を有効または無効にします
	/// </summary>
	/// <param name="[In_Enable]">ドラッグ機能を有効にするかどうか</param>
	/// <param name="[In_DragSpeed]">ドラッグの速度</param>
	/// <param name="[In_DragMin]">ドラッグの最小値</param>
	/// <param name="[In_DragMax]">ドラッグの最大値</param>
	void EnableDrag(_In_ bool In_Enable, _In_ float In_DragSpeed = 1.0f, _In_ float In_DragMin = 0.0f, _In_ float In_DragMax = 0.0f);

	void DrawImGui() override;

	template <typename T>
	T *GetPtr()
	{
		if(m_vPtr == nullptr)
			return nullptr;

		return reinterpret_cast<T *>(m_vPtr);
	}

	template<typename T>
	requires std::is_same_v<T,char>
	T *GetPtr()
	{
		if(m_vPtr == nullptr)
			return nullptr;

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
	void DrawInputValue();
	void DrawDragValue();

private:
	void *m_vPtr;
	float m_DragSpeed;
	float m_DragMin;
	float m_DragMax;
	bool m_IsString;
	bool m_EnableDrag;
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
	ItemCallback(_In_ std::string_view In_Name, _In_ Kind In_Kind, _In_ CallBack In_Func);
	~ItemCallback();

	void EnableDrag(_In_ bool In_Enable, _In_ float In_DragSpeed = 1.0f, _In_ float In_DragMin = 0.0f, _In_ float In_DragMax = 0.0f);

	void DrawImGui() override;

	Value &GetValue() { return m_Value; }
	void CallFunc(_In_ bool In_IsSet, _In_opt_ void *In_Ptr) { m_Func(In_IsSet, In_Ptr); }

private:
	void DrawInputValue();
	void DrawDragValue();

private:
	Value m_Value;
	CallBack m_Func;
	float m_DragSpeed;
	float m_DragMin;
	float m_DragMax;
	bool m_EnableDrag;
};

class ItemGroup : public DebugItem
{
	friend class DebugItem;
public:
	ItemGroup(_In_ std::string_view In_Name);
	~ItemGroup();

	void DrawImGui() override;

	std::vector<DebugItem *> &GetGroupItems() { return m_Items; }

	// グループで使用する関数

	template <typename T, typename ...Args>
	requires (std::derived_from<T, DebugItem>)
	T *CreateGroupItem(std::string_view In_Name, Args&& ...In_Args)
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
	T *CreateGroupItem(Args&& ...In_Args)
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

	/// <summary>
	/// グループのアイテムをクリアします
	/// </summary>
	void ClearGroupItems();

protected:
	void DataRead(_In_ std::string_view In_FullPath, _In_ DebugItem *In_pItem);
protected:
	std::vector<DebugItem *> m_Items;
};

class ItemList : public DebugItem
{
public:
	ItemList(_In_ std::string_view In_Name, _In_ ConstCallback In_Func = nullptr, _In_ bool In_IsSave = false);
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
	ItemConsole(_In_ std::string_view In_Name, _In_ bool In_IsOutputLogFile);
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

	ItemLayoutFunc(_In_ std::string_view In_Name, _In_ LayoutType In_LayoutType);
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
	ItemSameLine(_In_ std::string_view In_Name = "SameLine", _In_ float In_OffsetX = 0.0f, _In_ float In_SpacingW = -1.0f);
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
	ItemNewLine(_In_ std::string_view In_Name = "NewLine");
	~ItemNewLine();
	void DrawImGui() override;
};

/// <summary>
/// ImGuiのウィジェット間に空白を入れるためのレイアウト機能を提供する
/// </summary>
class ItemSpacing : public ItemLayoutFunc
{
public:
	ItemSpacing(_In_ std::string_view In_Name = "Spacing", _In_ int In_SpaceNum = 1);
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
	ItemSeparator(_In_ std::string_view In_Name = "Separator");
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
	ItemIndent(_In_ std::string_view In_Name = "Indent", _In_ float In_IndentW = 0.0f);
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
	ItemUnIndent(_In_ std::string_view In_Name = "UnIndent", _In_ float In_IndentW = 0.0f);
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
	ItemDummy(_In_ std::string_view In_Name = "Dummy", _In_ float In_Width = 0.0f, _In_ float In_Height = 0.0f);
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
	ItemSetNextItemWidth(_In_ std::string_view In_Name = "NextItemWidth", _In_ float In_Width = 16.0f);
	~ItemSetNextItemWidth();
	void DrawImGui() override;
private:
	float m_Width;
};

class ItemAlignTextToFramePadding : public ItemLayoutFunc
{
public:
	ItemAlignTextToFramePadding(_In_ std::string_view In_Name = "ItemAlignTextToFramePadding");
	~ItemAlignTextToFramePadding();
	void DrawImGui() override;
};
