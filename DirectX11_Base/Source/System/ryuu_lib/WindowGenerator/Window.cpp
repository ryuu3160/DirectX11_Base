/*+===================================================================
	File: Window.hpp
	Summary: Window作成クラス
	Author: ryuu3160
	Date: 2024/10/16 18:34 初回作成
		  2025/04/17 15:11 パラメーターの設定関数の追加、取得関数の追加、コメントの追加

	(C) 2024 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Window.hpp"
#include <Windows.h>

Window::Window()
{
	// 初期化
	ZeroMemory(&m_wcex, sizeof(m_wcex));
	m_hWnd = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_rcWindowRect = { 0, 0, 0, 0 };

	// 既定のアイコン
	m_hIconInstance = NULL;
	m_lpcIconName = IDI_APPLICATION;
	m_hCursorInstance = NULL;
	m_lpcCursorName = IDC_ARROW;

	// 既定のタイトル名
	m_lpcTitleName = "DefaultWindowTitle";

	// 既定のクラス名
	m_lpcClassName = "MainWindow";

	// 既定のウィンドウ挙動
	m_unStyle = CS_CLASSDC | CS_DBLCLKS;

	// ウィンドウの位置とスタイル
	m_dwStyle = WS_CAPTION | WS_SYSMENU;
	m_dwExStyle = WS_EX_OVERLAPPEDWINDOW;
	m_nX = CW_USEDEFAULT;
	m_nY = CW_USEDEFAULT;
	m_Color = WHITE_BRUSH;

	// 親ウィンドウへのハンドル
	m_hWndParent = HWND_DESKTOP;
	// メニューのハンドル
	m_hMenu = NULL;
	// MDIクライアントウィンドウなどで使用
	m_lpParam = NULL;
}

Window::~Window()
{
	// ウィンドウクラスの登録解除
	UnregisterClass(m_lpcClassName, m_wcex.hInstance);
}

LRESULT CALLBACK Window::m_WndProc(HWND In_hWnd, UINT In_unMessage, WPARAM In_wParam, LPARAM In_lParam)
{
	switch (In_unMessage)
	{
		// xボタンが押されたときのメッセージ
	case WM_CLOSE:
		if (IDNO == MessageBoxA(In_hWnd, "×ボタンが押されました。\nウィンドウを閉じますか？", "Window Close", MB_YESNO))
		{
			return 0;	// WM_CLOSEの処理をここで終わる
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	// 既定のウィンドウプロシージャを呼び出す
	return DefWindowProc(In_hWnd, In_unMessage, In_wParam, In_lParam);
}

void Window::Create(LPCTSTR In_lpcTitleName, int In_nWidth, int In_nHeight, HINSTANCE In_hInstance, int In_nCmdShow)
{
	// ------------------------------
	//	ウィンドウクラス情報の作成
	// ------------------------------
	m_wcex.hInstance = In_hInstance;									// インスタンスハンドル
	m_wcex.lpszClassName = m_lpcClassName;								// ウィンドウクラス名
	m_wcex.lpfnWndProc = m_WndProc;										// ウィンドウプロシージャ//ウィンドウプロシージャ
	m_wcex.style = m_unStyle;											// ウィンドウの挙動// ウィンドウの挙動
	m_wcex.cbSize = sizeof(WNDCLASSEX);									// ウィンドウクラス情報のサイズ
	m_wcex.hIcon = LoadIcon(m_hIconInstance, m_lpcIconName);			// アプリのアイコン// アプリのアイコン
	m_wcex.hIconSm = m_wcex.hIcon;										// タスクバーのアイコン
	m_wcex.hCursor = LoadCursor(m_hCursorInstance, m_lpcCursorName);	// マウスのアイコン// マウスのアイコン
	m_wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(m_Color));// 背景の色// 背景の色

	// ------------------------------
	//	ウィンドウクラス情報の登録
	// ------------------------------
	if (!RegisterClassEx(&m_wcex))
	{
		throw std::runtime_error("ウィンドウクラスの登録に失敗");
		return;
	}

	// ------------------------------
	//  情報の保存
	// ------------------------------

	m_nWidth = In_nWidth;							// ウィンドウの横幅を保存
	m_nHeight = In_nHeight;							// ウィンドウの縦幅を保存
	m_rcWindowRect = { 0, 0, m_nWidth, m_nHeight };	// ウィンドウの矩形を保存
	m_lpcTitleName = In_lpcTitleName;				// タイトルバーの表示名を保存

	// ------------------------------
	//	ウィンドウの作成・表示
	// ------------------------------

	// ウィンドウのサイズを調整
	AdjustWindowRectEx(&m_rcWindowRect, m_dwStyle, false, m_dwExStyle);

	// ウィンドウの作成
	m_hWnd = CreateWindowExA(
		m_dwExStyle,								// 拡張ウィンドウスタイル
		m_wcex.lpszClassName,						// ウィンドウクラス名
		m_lpcTitleName,								// ウィンドウのタイトル
		m_dwStyle,									// ウィンドウスタイル
		m_nX, m_nY,									// ウィンドウの表示位置
		m_rcWindowRect.right - m_rcWindowRect.left, // ウィンドウの幅
		m_rcWindowRect.bottom - m_rcWindowRect.top,	// ウィンドウの高さ
		m_hWndParent,								// 親ウィンドウのハンドル	
		m_hMenu,									// メニューハンドル
		In_hInstance,								// インスタンスハンドル
		m_lpParam									// CREATESTRUCT構造体へのポインタ
	);

	// エラー処理
	if (m_hWnd == NULL)
	{
		throw std::runtime_error("ウィンドウの作成に失敗");
		return;
	}

	// ウィンドウの表示
	ShowWindow(m_hWnd, In_nCmdShow);
	UpdateWindow(m_hWnd);
}

void Window::SetWindowPosCenter()
{
	// ウィンドウの矩形を取得
	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	// ウィンドウの位置を画面中央に設定
	m_nX = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
	m_nY = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;

	// ウィンドウの位置を設定
	SetWindowPos(m_hWnd, NULL, m_nX, m_nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

HWND& Window::GetHwnd()
{
	return m_hWnd;
}

int Window::GetWidth() const
{
	return m_nWidth;
}

int Window::GetHeight() const
{
	return m_nHeight;
}

LPCSTR Window::GetTitleName() const
{
	return m_lpcTitleName;
}

LPCSTR Window::GetClassName() const
{
	return m_lpcClassName;
}

HWND Window::GetParentHwnd() const
{
	return m_hWndParent;
}

void Window::SetClassName(LPCSTR& In_alpcName)
{
	m_lpcClassName = In_alpcName;
}

void Window::SetParent(HWND In_hWndParent)
{
	m_hWndParent = In_hWndParent;
}

void Window::SetColor(int In_nColor)
{
	m_Color = In_nColor;
}

void Window::SetIcon(HINSTANCE& In_ahIns, LPCTSTR& In_alpcName)
{
	m_hIconInstance = In_ahIns;
	m_lpcIconName = In_alpcName;
}

void Window::SetCursorIcon(HINSTANCE& In_ahIns, LPCTSTR& In_alpcName)
{
	m_hCursorInstance = In_ahIns;
	m_lpcCursorName = In_alpcName;
}

void Window::SetStyle(UINT In_unStyle)
{
	m_unStyle = In_unStyle;
}

void Window::SetWindowStyle(DWORD In_dwStyle)
{
	m_dwStyle = In_dwStyle;
}

void Window::SetWindowExStyle(DWORD In_dwExStyle)
{
	m_dwExStyle = In_dwExStyle;
}

void Window::SetMenu(HMENU In_hMenu)
{
	m_hMenu = In_hMenu;
}

void Window::SetCreateStructParam(LPCLIENTCREATESTRUCT In_lpParam)
{
	m_lpParam = In_lpParam;
}

void Window::SetCreateStructParam(LPMDICREATESTRUCT In_lpParam)
{
	m_lpParam = In_lpParam;
}
