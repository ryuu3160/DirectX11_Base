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

Component::Component(_In_ std::string In_Name)
	: m_Name(In_Name), m_pTransform(nullptr)
{
}

Component::~Component()
{
	m_pTransform = nullptr;
}

void Component::Update() noexcept
{
}

void Component::LateUpdate() noexcept
{
}

void Component::ReadWrite(_In_ DataAccessor *In_Data)
{
}

void Component::DestroySelf() noexcept
{
	m_IsDestroyed = true;
	m_pTransform->RemoveComponent(m_Name);
}
