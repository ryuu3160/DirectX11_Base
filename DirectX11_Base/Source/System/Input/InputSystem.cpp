/*+===================================================================
	File: InputSystem.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/30 Sun AM 02:21:08 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InputSystem.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

void InputSystem::Init() noexcept
{
	m_InputMap.clear();
    m_NextHandle = 1;
}

void InputSystem::Update(_In_ float In_Tick) noexcept
{
    for(auto &[key, callbacks] : m_InputMap)
    {
        // キーの状態をチェック
        for(auto &info : callbacks)
        {
            if(!info.Enabled || !info.CallBack)
                continue;

            bool shouldCall = false;

            switch(info.State)
            {
            case KeyState::Press:
                shouldCall = Input::IsKeyPress(key);
                break;
            case KeyState::Trigger:
                shouldCall = Input::IsKeyTrigger(key);
                break;
            case KeyState::Release:
                shouldCall = Input::IsKeyRelease(key);
                break;
            case KeyState::Repeat:
                shouldCall = Input::IsKeyRepeat(key);
                break;
            }

            if(shouldCall)
            {
                info.CallBack();
            }
        }
    }
}

InputSystem::CallbackHandle InputSystem::RegisterKeyCallBack(_In_ BYTE In_Key, _In_ KeyState In_State, _In_ std::function<void()> In_CallBack, _In_ bool In_Enabled) noexcept
{
    CallbackHandle Handle = m_NextHandle++;

    Info info;
    info.Handle = Handle;
    info.State = In_State;
    info.CallBack = In_CallBack;
    info.Enabled = In_Enabled;

    // キーに対応するベクターに追加
    m_InputMap[In_Key].push_back(info);

    return Handle;
}

void InputSystem::UnregisterKeyCallBack(_In_ CallbackHandle In_Handle) noexcept
{
    for(auto &[key, callbacks] : m_InputMap)
    {
        auto it = std::remove_if(callbacks.begin(), callbacks.end(),
            [In_Handle](const Info &info)
            {
                return info.Handle == In_Handle;
            });

        if(it != callbacks.end())
        {
            callbacks.erase(it, callbacks.end());

            // ベクターが空になったらキーも削除
            if(callbacks.empty())
            {
                m_InputMap.erase(key);
            }

            return;
        }
    }
}

void InputSystem::UnregisterAllCallbacksForKey(_In_ BYTE In_Key) noexcept
{
    m_InputMap.erase(In_Key);
}

void InputSystem::ClearAllCallbacks() noexcept
{
	m_InputMap.clear();
}

void InputSystem::SetCallbackEnabled(_In_ CallbackHandle In_Handle, _In_ bool In_Enabled) noexcept
{
    for(auto &[key, callbacks] : m_InputMap)
    {
        for(auto &info : callbacks)
        {
            if(info.Handle == In_Handle)
            {
                info.Enabled = In_Enabled;
                return;
            }
        }
    }
}

InputSystem::InputSystem()
	: m_NextHandle(1)
{
}

InputSystem::~InputSystem()
{
}
