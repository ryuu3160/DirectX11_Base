/*+===================================================================
	File: CustomWindowProc.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/7 Sat PM 03:10:40 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "CustomWindowProc.hpp"

namespace EngineCustomWindowProc
{
	LRESULT DisableSingleAltKey(_In_ HWND In_hWnd, _In_ UINT In_unMessage, _In_ WPARAM In_wParam, _In_ LPARAM In_lParam)
	{
        switch(In_unMessage)
        {
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            // Alt キー単体の場合は無視
            if(In_wParam == VK_MENU || In_wParam == VK_LMENU || In_wParam == VK_RMENU)
            {
                return true;
            }

            // Alt + F4 などの組み合わせは許可
            return false;
        }
        }

        return false;
	}
}

