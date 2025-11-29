/*+===================================================================
	File: InputSystem.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/30 Sun AM 02:20:39 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Component.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// InputSystemクラス
/// </summary>
class InputSystem : public Component
{
public:
	enum class KeyState
	{
		Press,
		Trigger,
		Release,
		Repeat
	};
public:
	InputSystem();
	~InputSystem() override;

	void ReadWrite(_In_ DataAccessor *In_Data) final;

	virtual void Init() noexcept override;

	virtual void Update(_In_ float In_Tick) noexcept override;

	void RegisterKeyCallBack(_In_ BYTE In_Key, _In_ KeyState In_State, _In_ std::function<void()> In_CallBack) noexcept;

protected:
#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif

private:
	struct Info
	{
		KeyState State;
		std::function<void()> CallBack;
	};
	std::unordered_map<BYTE, Info> m_InputMap;
};
