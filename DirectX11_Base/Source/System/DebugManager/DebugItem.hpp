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
		Vector,		// ベクター入力
		Color,		// 色入力
		Path,		// ファイルパスの指定
		InputStr,	// 文字列入力
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

	Value &GetValue() { return m_Value; }

private:
	Value m_Value;
	bool m_IsSave;
};

class ItemText : public DebugItem
{
public:
	ItemText(_In_ std::string In_Name, _In_ bool In_IsMultiline, _In_ ImGuiInputTextFlags In_Flags, _In_ bool In_IsSave);
	~ItemText();

	std::string &GetText() { return m_Text; }
	ImGuiInputTextFlags GetFlags() const { return m_Flags; }
	bool IsMultiline() const { return m_IsMultiline; }
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
	bool m_IsSave;
};

class ItemBind : public DebugItem
{
public:
	ItemBind(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ void* In_Ptr);
	~ItemBind();

	template <typename T>
	T *GetPtr()
	{
		return reinterpret_cast<T *>(m_vPtr);
	}

private:
	void *m_vPtr;
};

class ItemCallback : public DebugItem
{
public:
	ItemCallback(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ CallBack In_Func);
	~ItemCallback();

	Value &GetValue() { return m_Value; }
	void CallFunc(_In_ bool In_IsSet, _In_ void *In_Ptr) { m_Func(In_IsSet, In_Ptr); }

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

	std::vector<DebugItem *> &GetGroupItems() { return m_Items; }

private:
	std::vector<DebugItem *> m_Items;
};

class ItemList : public DebugItem
{
public:
	ItemList(_In_ std::string In_Name, _In_ ConstCallback In_Func = nullptr, _In_ bool In_IsSave = false);
	~ItemList();

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
