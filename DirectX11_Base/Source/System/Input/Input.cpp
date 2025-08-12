//Input.cpp
#include "Input.hpp"

namespace Input
{

	namespace
	{
		BYTE g_KeyTable[256]; // キーボードのキーの状態を保存するテーブル
		BYTE g_oldKeyTable[256]; // 前のキーボードのキーの状態を保存するテーブル
		POINT g_MousePos;			// マウスポインタのスクリーン上の座標
		POINT g_MousePrevPos;		// 前のマウスポインタのスクリーン上の座標

		POINTS g_MouseRelativePos;	// マウスポインタのウィンドウ上の相対座標
		int g_nMouseWheelDelta;		// マウスホイールの回転量
	}

HRESULT Init()
{
	//最初の入力
	if (GetKeyboardState(g_KeyTable))
	{
		return S_OK;
	}
	else
	{
		OutputDebugStringA("error : キーボードの初期化に失敗しました。");
		return E_FAIL;
	}
}

void Uninit()
{
}

void Update()
{
	//古い入力を更新
	memcpy_s(g_oldKeyTable, sizeof(g_oldKeyTable), g_KeyTable, sizeof(g_KeyTable));
	//現在の入力を取得
	//最初の入力
	if (!GetKeyboardState(g_KeyTable))
		OutputDebugStringA("error : キーボードの更新に失敗しました。");
	if (!GetCursorPos(&g_MousePos))
		OutputDebugStringA("error : マウスの位置取得に失敗しました。");
}

void EndUpdate() noexcept
{
	g_nMouseWheelDelta = 0; // マウスホイールの回転量をリセット
	g_MousePrevPos = g_MousePos;
}

bool IsKeyPress(BYTE key)
{
	return g_KeyTable[key] & 0x80;
}

bool IsKeyTrigger(BYTE key)
{
	return (g_KeyTable[key] ^ g_oldKeyTable[key]) & g_KeyTable[key] & 0x80;
}

bool IsKeyRelease(BYTE key)
{
	return (g_KeyTable[key] ^ g_oldKeyTable[key]) & g_oldKeyTable[key] & 0x80;
}

bool IsKeyRepeat(BYTE key)
{
	return false;
}

POINTS GetMouseRelativePos() noexcept
{
	return g_MouseRelativePos;
}

POINTS GetMouseRelativePos_CenterZero() noexcept
{
	POINTS work;
	work.x = g_MouseRelativePos.x - (cx_nWINDOW_WIDTH / 2);
	work.y = g_MouseRelativePos.y - (cx_nWINDOW_HEIGHT / 2);
	return work;
}

POINT GetMouseDifference() noexcept
{
	// 差分格納用
	POINT work;

	// 差分の計算
	work.x = g_MousePos.x - g_MousePrevPos.x;
	work.y = g_MousePos.y - g_MousePrevPos.y;

	return work;
}

int GetMouseWheelDelta() noexcept
{
	return g_nMouseWheelDelta / 120; // マウスホイールの回転量を120で割って、1回転あたりの値に変換
}

LRESULT InputCustomProc(_In_ HWND In_hWnd, _In_ UINT In_Msg, _In_ WPARAM In_wParam, _In_ LPARAM In_lParam)
{
	switch (In_Msg)
	{
	case WM_MOUSEMOVE:
		// マウスの相対位置を更新
		// マウスのウィンドウ上の相対座標を取得
		g_MouseRelativePos = MAKEPOINTS(In_lParam);
		break;
	case WM_MOUSEWHEEL:
		// マウスホイールの回転量を更新
		g_nMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(In_wParam);
		break;
	}

	return 0;
}

} // namespace Input