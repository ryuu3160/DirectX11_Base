/*+===================================================================
	File: Component.cpp
	Summary: コンポーネントの基底クラス 佐々木先生のコードを参考に作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:28:33 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Component.hpp"

Component::Component() : m_pTransform(nullptr)
{
}

void Component::Execute()
{
}

void Component::ReadWrite(_In_ DataAccessor *In_Data)
{
}
