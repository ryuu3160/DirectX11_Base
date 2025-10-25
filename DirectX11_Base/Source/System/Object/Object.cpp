/*+===================================================================
	File: Object.cpp
	Summary: オブジェクト基底クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/10/13 Mon PM 11:46:02 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Object.hpp"

void Object::Initialize() noexcept
{
	if (m_IsInitialized)
		return;

	Init();
	m_IsInitialized = true;
}