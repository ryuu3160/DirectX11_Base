/*+===================================================================
	File: DebugItem.cpp
	Summary: デバッグ項目クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:00:20 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugItem.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	static DebugItem *s_NullItem = nullptr;
}

DebugItem::DebugItem()
{
	m_Name = "None";
	m_Kind = Kind::Label;
}

DebugItem::~DebugItem()
{
}

std::string DebugItem::GetName() const
{
	return m_Name;
}

const char *DebugItem::GetCStrName() const
{
	return m_Name.c_str();
}

DebugItem::Kind DebugItem::GetKind() const
{
	return m_Kind;
}

bool DebugItem::GetBool() const
{
	if (m_Kind == Kind::Bool)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if(ptr)
			return std::get<bool>(ptr->GetValue());
	}
	return false;
}

int DebugItem::GetInt() const
{
	if (m_Kind == Kind::Int)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if (ptr)
			return std::get<int>(ptr->GetValue());
	}
	return 0;
}

float DebugItem::GetFloat() const
{
	if (m_Kind == Kind::Float)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if (ptr)
			return std::get<float>(ptr->GetValue());
	}
	return 0.0f;
}

DirectX::XMFLOAT2 DebugItem::GetVector2() const
{
	if(m_Kind == Kind::Float2)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if(ptr)
			return std::get<DirectX::XMFLOAT2>(ptr->GetValue());
	}
	return DirectX::XMFLOAT2{};
}

DirectX::XMFLOAT3 DebugItem::GetVector() const
{
	if (m_Kind == Kind::Vector)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if(ptr)
			return std::get<DirectX::XMFLOAT3>(ptr->GetValue());
	}
	return DirectX::XMFLOAT3{};
}

DirectX::XMFLOAT4 DebugItem::GetColor() const
{
	if (m_Kind == Kind::Color)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if(ptr)
			return std::get<DirectX::XMFLOAT4>(ptr->GetValue());
	}
	return DirectX::XMFLOAT4{};
}

std::string DebugItem::GetStr() const
{
	if (m_Kind == Label || m_Kind == Path)
	{
		auto me = *this;
		auto ptr = dynamic_cast<ItemValue *>(&me);
		if(ptr)
			return std::get<std::string>(ptr->GetValue());
	}
	return "";
}

DebugItem &DebugItem::operator[](_In_ const std::string_view In_Name)
{
	if (m_Kind != Kind::Group)
		return *s_NullItem;

	// グループ内のアイテム一覧を取得
	auto items = static_cast<ItemGroup *>(this)->GetGroupItems();

	// アイテム一覧から指定アイテムの探索
	auto it = std::find_if(items.begin(), items.end(),
		[&In_Name](const DebugItem *obj)
		{
			return std::strcmp(obj->GetName().c_str(), In_Name.data()) == 0;
		});

	// 見つかったアイテムの返却
	if (it != items.end())
		return **it;

	// 見つからなかったらダミーを返却
	return *s_NullItem;
}

void DebugItem::AddListItem(_In_ const std::string_view In_Name)
{
	if (m_Kind != Kind::List)
		return;

	auto ptr = dynamic_cast<ItemList *>(this);
	if (ptr)
	{
		ptr->GetList().push_back(In_Name.data());
	}
}

void DebugItem::RemoveListItem(_In_ const std::string_view In_Name)
{
	if (m_Kind != Kind::List)
		return;

	auto ptr = dynamic_cast<ItemList *>(this);
	if (ptr)
	{
		ptr->GetList().remove(In_Name.data());
	}
}

DebugItem::Kind DebugItem::StrToKind(_In_ std::string In_Str)
{
	if (In_Str == "Label")		return DebugItem::Label;
	if (In_Str == "Bool")		return DebugItem::Bool;
	if (In_Str == "Int")		return DebugItem::Int;
	if (In_Str == "Float")		return DebugItem::Float;
	if (In_Str == "Vector")		return DebugItem::Vector;
	if (In_Str == "Color")		return DebugItem::Color;
	if (In_Str == "Path")		return DebugItem::Path;
	if (In_Str == "Command")	return DebugItem::Command;
	if (In_Str == "Group")		return DebugItem::Group;
	if (In_Str == "List")		return DebugItem::List;
	return DebugItem::Label;
}

std::string DebugItem::KindToStr(_In_ Kind In_Kind)
{
	switch (In_Kind)
	{
	default:
	case DebugItem::Label:		return "Label";
	case DebugItem::Bool:		return "Bool";
	case DebugItem::Int:		return "Int";
	case DebugItem::Float:		return "Float";
	case DebugItem::Vector:		return "Vector";
	case DebugItem::Color:		return "Color";
	case DebugItem::Path:		return "Path";
	case DebugItem::Command:	return "Command";
	case DebugItem::Group:		return "Group";
	case DebugItem::List:		return "List";
	}
}

// ==============================
//  ItemValue
// ==============================


ItemValue::ItemValue(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ bool In_IsSave)
	: m_Value(), m_IsSave(In_IsSave)
{
	m_Name = In_Name;

	if(In_Kind == Kind::InputStr || In_Kind == Kind::Group || In_Kind == Kind::List || In_Kind == Kind::Command)
	{
		// InputStr、グループ、リスト、コマンドはItemValueでは扱えない
		In_Kind = Kind::Label;
	}

	m_Kind = In_Kind;
}

ItemValue::~ItemValue()
{
}

// ==============================
//  ItemText
// ==============================

ItemText::ItemText(_In_ std::string In_Name, _In_ bool In_IsMultiline, _In_ ImGuiInputTextFlags In_Flags, _In_ bool In_IsSave)
{
	m_Name = In_Name;
	m_Kind = Kind::InputStr;
	m_Flags = In_Flags;
	m_IsMultiline = In_IsMultiline;
	m_IsSave = In_IsSave;
	m_LineCount = 0;
}

ItemText::~ItemText()
{
}

// ==============================
//  ItemBind
// ==============================

ItemBind::ItemBind(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ void *In_Ptr)
{
	m_Name = In_Name;

	if(In_Kind == Kind::Group || In_Kind == Kind::List || In_Kind == Kind::Command)
	{
		// グループ、リスト、コマンドはItemBindでは扱えない
		In_Kind = Kind::Label;
	}

	m_Kind = In_Kind;
	m_vPtr = In_Ptr;
}

ItemBind::~ItemBind()
{
}

// ==============================
//  ItemCallback
// ==============================

ItemCallback::ItemCallback(_In_ std::string In_Name, _In_ Kind In_Kind, _In_ CallBack In_Func)
{
	m_Name = In_Name;
	if(In_Kind == Kind::Group || In_Kind == Kind::List)
	{
		// グループ、リストはItemCallbackでは扱えない
		In_Kind = Kind::Label;
	}
	m_Kind = In_Kind;
	m_Func = In_Func;

	switch (m_Kind)
	{
	case Kind::Label:
	case Kind::Path:
	case Kind::InputStr:
		m_Value = std::string{};
		break;
	case Kind::Bool:
		m_Value = false;
		break;
	case Kind::Int:
		m_Value = 0;
		break;
	case Kind::Float:
		m_Value = 0.0f;
		break;
	case Kind::Vector:
		m_Value = DirectX::XMFLOAT3{};
		break;
	case Kind::Color:
		m_Value = DirectX::XMFLOAT4{};
		break;
	}
}

ItemCallback::~ItemCallback()
{
}

// ==============================
//  ItemGroup
// ==============================

ItemGroup::ItemGroup(_In_ std::string In_Name)
{
	m_Name = In_Name;
	m_Kind = Kind::Group;
}

ItemGroup::~ItemGroup()
{
	for(auto &item : m_Items)
	{
		delete item;
	}
	m_Items.clear();
}

// ==============================
//  ItemList
// ==============================

ItemList::ItemList(_In_ std::string In_Name, _In_ ConstCallback In_Func, _In_ bool In_IsSave)
{
	m_Name = In_Name;
	m_Kind = Kind::List;
	m_SelectNo = -1;
	m_Func = In_Func;
	m_IsSave = In_IsSave;
}

ItemList::~ItemList()
{
	m_List.clear();
}
