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
	return Kind();
}

// ==============================
//  ItemValue
// ==============================


ItemValue::~ItemValue()
{
}
