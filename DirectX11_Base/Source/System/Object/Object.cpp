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

void Object::SetActive(_In_ bool In_IsActive) noexcept
{
	if (m_IsActive == In_IsActive)
		return;
	m_IsActive = In_IsActive;
	if (m_IsActive)
		OnEnable();
}
