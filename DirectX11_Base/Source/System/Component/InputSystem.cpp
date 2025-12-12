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

void InputSystem::SaveLoad(_In_ DataAccessor *In_Data)
{
}

void InputSystem::Init() noexcept
{
}

void InputSystem::Update(_In_ float In_Tick) noexcept
{
	if(In_Tick == 0.0f)
		return;

	for (const auto &itr : m_InputMap)
	{
		switch (itr.second.State)
		{
		case KeyState::Press:
			if (Input::IsKeyPress(itr.first))
				itr.second.CallBack();
			break;
		case KeyState::Trigger:
			if (Input::IsKeyTrigger(itr.first))
				itr.second.CallBack();
			break;
		case KeyState::Release:
			if (Input::IsKeyRelease(itr.first))
				itr.second.CallBack();
			break;
		case KeyState::Repeat:
			if (Input::IsKeyRepeat(itr.first))
				itr.second.CallBack();
			break;
		default:
			break;
		}
	}
}

void InputSystem::RegisterKeyCallBack(_In_ BYTE In_Key, _In_ KeyState In_State, _In_ std::function<void()> In_CallBack) noexcept
{
	auto itr = m_InputMap.try_emplace(In_Key, Info{ In_State, In_CallBack });
	if(!itr.second)
		DebugManager::GetInstance().DebugLogWarning("InputSystem::AddInputCallback: Key %d is already registered.", In_Key);
}

void InputSystem::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
}

#ifdef _DEBUG
void RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{

}
#endif
