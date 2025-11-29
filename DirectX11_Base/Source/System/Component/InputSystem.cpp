/*+===================================================================
	File: InputSystem.cpp
	Summary:
	Author: AT13C192 01 ê¬ñÿóYàÍòY
	Date: 2025/11/30 Sun AM 02:21:08 èââÒçÏê¨
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InputSystem.hpp"

// ==============================
//	íËêîíËã`
// ==============================
namespace
{
}

InputSystem::InputSystem()
	: Component("InputSystem")
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::ReadWrite(_In_ DataAccessor *In_Data)
{
}

void InputSystem::Init() noexcept
{
}

void InputSystem::Update(_In_ float In_Tick) noexcept
{
}

void InputSystem::RegisterKeyCallBack(_In_ BYTE In_Key, _In_ KeyState In_State, _In_ std::function<void()> In_CallBack) noexcept
{
	auto itr = m_InputMap.try_emplace(In_Key, Info{ In_State, In_CallBack });
	if(!itr.second)
		DebugManager::GetInstance().DebugLog("InputSystem::AddInputCallback: Key %d is already registered.", In_Key);
}

void InputSystem::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
}

#ifdef _DEBUG
void RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{

}
#endif
