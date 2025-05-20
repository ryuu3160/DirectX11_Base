/*+===================================================================
	File: Window.hpp
	Summary: Window作成クラス
	Author: ryuu3160
	Date: 2024/10/16 18:34 初回作成
		  2025/04/17 15:11 パラメーターの設定関数の追加、取得関数の追加、コメントの追加

	(C) 2024 ryuu3160. All rights reserved.
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <Windows.h>
#include "../Singleton/Singleton.hpp"

// メンバ関数にGetClassNameがあるため、マクロを無効化
#undef GetClassName

/// <summary>
/// ウィンドウ作成クラス
/// </summary>
class Window : public Singleton<Window>
{
	friend class Singleton<Window>;
public:
	/// <summary>
	/// ウィンドウプロシージャー
	/// </summary>
	/// <param name="[In_hWnd]">ウィンドウのハンドル</param>
	/// <param name="[In_unMessage]">メッセージ</param>
	/// <param name="[In_wParam]">追加のメッセージ</param>
	/// <param name="[In_lParam]">追加のメッセージ</param>
	/// <returns>結果</returns>
	static LRESULT CALLBACK m_WndProc(HWND In_hWnd, UINT In_unMessage, WPARAM In_wParam, LPARAM In_lParam);

	/// <summary>
	/// ウィンドウの作成
	/// </summary>
	/// <param name="[In_lpcTitleName]">タイトルバーの表示名</param>
	/// <param name="[In_nWidth]">ウィンドウの横幅</param>
	/// <param name="[In_nHeight]">ウィンドウの縦幅</param>
	/// <param name="[In_hInstance]">WinMainの引数にあるインスタンスハンドル</param>
	/// <param name="[In_nCmdShow]">ウィンドウの表示方法</param>
	void Create(LPCTSTR In_lpcTitleName, int In_nWidth, int In_nHeight, HINSTANCE In_hInstance, int In_nCmdShow = SW_SHOWDEFAULT);

	/// <summary>
	/// ウィンドウを画面中央に移動
	/// </summary>
	void SetWindowPosCenter();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	HWND& GetHwnd();
	/// <summary>
	/// ウィンドウの横幅を取得
	/// </summary>
	/// <returns>横幅(int型)</returns>
	int GetWidth() const;
	/// <summary>
	/// ウィンドウの縦幅を取得
	/// </summary>
	/// <returns>縦幅(int型)</returns>
	int GetHeight() const;

	/// <summary>
	/// タイトルバーの表示名を取得
	/// </summary>
	/// <returns>タイトルバーの表示名</returns>
	LPCSTR GetTitleName() const;

	/// <summary>
	/// ウィンドウクラス名の取得
	/// </summary>
	/// <returns>ウィンドウクラス名</returns>
	LPCSTR GetClassName() const;

	/// <summary>
	/// 親ウィンドウのハンドルを取得
	/// </summary>
	/// <returns>親ウィンドウのハンドル</returns>
	HWND GetParentHwnd() const;

	/// <summary>
	/// クラスネームの設定
	/// </summary>
	/// <param name="[In_alpcName]">クラス名</param>
	void SetClassName(LPCSTR& In_alpcName);

	/// <summary>
	/// 親ウィンドウの設定
	/// </summary>
	/// <param name="[In_hWndParent]">親ウィンドウへのハンドル</param>
	void SetParent(HWND In_hWndParent = HWND_DESKTOP);

	/// <summary>
	/// ウィンドウカラーの設定
	/// </summary>
	/// <param name="[In_nColor]">色パラメータ</param>
	void SetColor(int In_nColor = WHITE_BRUSH);
	/// <summary>
	/// アイコンの設定
	/// </summary>
	/// <param name="[In_ahIns]">HINSTANCEへの参照</param>
	/// <param name="[In_alpcName]">LPCTSTRへの参照</param>
	void SetIcon(HINSTANCE& In_ahIns, LPCTSTR& In_alpcName);
	/// <summary>
	/// カーソルアイコンの設定
	/// </summary>
	/// <param name="[In_ahIns]">HINSTANCEへの参照</param>
	/// <param name="[In_alpcName]">LPCTSTRへの参照</param>
	void SetCursorIcon(HINSTANCE& In_ahIns, LPCTSTR& In_alpcName);
	/// <summary>
	/// ウィンドウ挙動の設定
	/// </summary>
	/// <param name="[In_unStyle]">UINT型</param>
	void SetStyle(UINT In_unStyle);

	/// <summary>
	/// ウィンドウスタイルの設定
	/// </summary>
	/// <param name="[In_dwStyle]">ウィンドウスタイル値とコントロールスタイルの組み合わせ</param>
	void SetWindowStyle(DWORD In_dwStyle = WS_CAPTION | WS_SYSMENU);

	/// <summary>
	/// 拡張ウィンドウスタイルの設定
	/// </summary>
	/// <param name="[In_dwExStyle]">拡張ウィンドウスタイル</param>
	void SetWindowExStyle(DWORD In_dwExStyle = WS_EX_OVERLAPPEDWINDOW);

	/// <summary>
	/// メニューのハンドル又は子ウィンドウ識別子の設定
	/// </summary>
	/// <param name="[In_hMenu]">HMENU</param>
	void SetMenu(HMENU In_hMenu = NULL);

	/// <summary>
	/// MDIクライアントウィンドウを作成するときに使用
	/// </summary>
	/// <param name="[lpParam]">CLIENTCREATESTRUCTへのポインタ</param>
	void SetCreateStructParam(LPCLIENTCREATESTRUCT In_lpParam);

	/// <summary>
	/// MDI子ウィンドウを作成するときに使用
	/// </summary>
	/// <param name="[lpParam]">MDICREATESTRUCTへのポインタ</param>
	void SetCreateStructParam(LPMDICREATESTRUCT In_lpParam);

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Window();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Window() override;

private:
	WNDCLASSEX m_wcex;		//ウィンドウクラス情報
	HWND m_hWnd;			//ウィンドウハンドル
	int m_nWidth;			//ウィンドウの横幅
	int m_nHeight;			//ウィンドウの縦幅
	RECT m_rcWindowRect;	//ウィンドウの矩形

	LPCSTR m_lpcTitleName;	// タイトルバーの表示名

	// アイコン
	HINSTANCE m_hIconInstance;
	LPCTSTR m_lpcIconName;

	// カーソルアイコン
	HINSTANCE m_hCursorInstance;
	LPCTSTR m_lpcCursorName;
	
	UINT m_unStyle;			// ウィンドウ挙動
	LPCSTR m_lpcClassName;	// ウィンドウを識別するためのクラス名

	// ウィンドウのスタイル
	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	// ウィンドウの位置
	int m_nX;
	int m_nY;

	int m_Color;	// ウィンドウカラー

	HWND m_hWndParent; // 親ウィンドウへのハンドル

	// メニューのハンドル、又は、ウィンドウのスタイルに応じて子ウィンドウ識別子を指定する
	// 重なったウィンドウ又はポップアップウィンドウの場合、ウィンドウで使用するメニューを識別する
	// クラスメニューを使用する場合は、NULLを指定できる
	// 子ウィンドウの場合、子ウィンドウ識別子※を指定
	// アプリケーションは、子ウィンドウ識別子を決定する
	// 同じ親ウィンドウを持つすべての子ウィンドウで一意である必要がある
	// ※:イベントについて親に通知するためにダイアログ ボックス コントロールによって使用される整数値
	HMENU m_hMenu;

	// MDIクライアントウィンドウ、又はMDI子ウィンドウを作成するときに使用
	// これを指定しない場合はNULLを指定する
	// このメッセージは、戻る前に、この関数によって作成されたウィンドウに送信される
	LPVOID m_lpParam;
};