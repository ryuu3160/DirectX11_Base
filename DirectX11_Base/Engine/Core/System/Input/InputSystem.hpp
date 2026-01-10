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
#include "ryuu_lib/Singleton/Singleton.hpp"
#include "Core/System/Input/Input.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// InputSystemクラス
/// </summary>
class InputSystem : public Singleton<InputSystem>
{
	friend class Singleton<InputSystem>;
public:
	enum class KeyState
	{
		Press,
		Trigger,
		Release,
		Repeat
	};

	// コールバックのハンドル
	using CallbackHandle = size_t;
public:

	void Init() noexcept;

	void Update(_In_ float In_DeltaTime) noexcept;

	/// <summary>
	/// 指定されたキーと状態に対してコールバック関数を登録します
	/// </summary>
	/// <param name="[In_Key]">監視する仮想キーコード</param>
	/// <param name="[In_State]">コールバックをトリガーするキーの状態(押下、解放など)</param>
	/// <param name="[In_CallBack]">キーイベントが発生したときに呼び出される関数</param>
	/// <param name="[In_Enabled]">コールバックを即座に有効にするかどうか(デフォルトはtrue)</param>
	/// <returns>登録されたコールバックを管理するためのハンドル</returns>
	CallbackHandle RegisterKeyCallBack(_In_ BYTE In_Key, _In_ KeyState In_State, _In_ std::function<void()> In_CallBack, _In_ bool In_Enabled = true) noexcept;

	/// <summary>
	/// キーコールバックの登録を解除します
	/// </summary>
	/// <param name="[In_Handle]">登録解除するコールバックのハンドル</param>
	void UnregisterKeyCallBack(_In_ CallbackHandle In_Handle) noexcept;

	/// <summary>
	/// 指定されたキーに対するすべてのコールバックを登録解除します
	/// </summary>
	/// <param name="[In_Key]">コールバックを登録解除するキー</param>
	void UnregisterAllCallbacksForKey(_In_ BYTE In_Key) noexcept;

	/// <summary>
	/// すべてのコールバックをクリアします
	/// </summary>
	void ClearAllCallbacks() noexcept;

	/// <summary>
	/// コールバックの有効/無効を設定します
	/// </summary>
	/// <param name="[In_Handle]">設定対象のコールバックハンドル</param>
	/// <param name="[In_Enabled]">コールバックを有効にする場合はtrue、無効にする場合はfalse</param>
	void SetCallbackEnabled(_In_ CallbackHandle In_Handle, _In_ bool In_Enabled) noexcept;

private:
	InputSystem();
	~InputSystem() override;

private:
	struct Info
	{
		CallbackHandle Handle{};
		KeyState State{};
		std::function<void()> CallBack;
		bool Enabled{};
	};
	std::unordered_map<BYTE, std::vector<Info>> m_InputMap;
	CallbackHandle m_NextHandle;
};
