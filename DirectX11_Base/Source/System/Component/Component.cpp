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

void Component::Update(_In_ float In_Tick) noexcept
{
}

void Component::LateUpdate(_In_ float In_Tick) noexcept
{
}

void Component::FixedUpdate(_In_ double In_FixedTick) noexcept
{
}

void Component::DestroySelf() noexcept
{
	Object::DestroySelf();
	m_pTransform->RemoveComponent(m_Name);
}

#ifdef _DEBUG
void Component::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
}
#endif

void Component::DataWrite(_In_ std::shared_ptr<cpon_block> In_pCponBlock)
{
	cpon_block::Object Data = std::make_shared<cpon_object>();
	Data->SetObjectName(m_Name);
	DataAccessor accessor(Data);
	SaveLoad(&accessor);
	In_pCponBlock->AddObject(Data);
}

void Component::DataRead(_In_ std::shared_ptr<cpon_object> In_pCponObj)
{
	DataAccessor accessor(In_pCponObj);
	SaveLoad(&accessor);
}
