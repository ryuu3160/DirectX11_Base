/*+===================================================================
	File: Window.hpp
	Summary: Window作成クラス
	Author: ryuu3160
	Date: 2024/10/16 18:34 初回作成
		  2025/04/17 15:11 パラメーターの設定関数の追加、取得関数の追加、コメントの追加
			　/06/23 11:24 リファクタリング

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
	static LRESULT CALLBACK m_WndProc(_In_ HWND In_hWnd, _In_ UINT In_unMessage, _In_ WPARAM In_wParam, _In_ LPARAM In_lParam);

	/// <summary>
	/// ウィンドウの作成
	/// </summary>
	/// <param name="[In_lpcTitleName]">タイトルバーの表示名</param>
	/// <param name="[In_nWidth]">ウィンドウの横幅</param>
	/// <param name="[In_nHeight]">ウィンドウの縦幅</param>
	/// <param name="[In_hInstance]">WinMainの引数にあるインスタンスハンドル</param>
	/// <param name="[In_nCmdShow]">ウィンドウの表示方法</param>
	void Create(_In_ LPCTSTR In_lpcTitleName, _In_ UINT In_unWidth, UINT In_unHeight, _In_ HINSTANCE In_hInstance, _In_ int In_nCmdShow = SW_SHOWDEFAULT);

	/// <summary>
	/// ウィンドウを画面中央に移動
	/// </summary>
	void SetWindowPosCenter();

	/// <summary>
	/// メッセージループで使用する
	/// </summary>
	/// <returns>メッセージループの結果(trueの間はゲームループを実行してください)</returns>
	/// <memo>if文の条件式で使用し、trueが帰ってきた場合は、ゲームのメインループを実行してください</memo>
	bool MessageLoop();

	/// <summary>
	/// メインループの制御
	/// </summary>
	/// <returns>
	/// <para>Windowのバツボタンが押された場合、falsを返す</para>
	/// <para>それ以外はtrueを返し続ける</para>
	/// </returns>
	bool IsLoop() const noexcept;

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	inline HWND& GetHwnd() noexcept { return m_hWnd;}
	/// <summary>
	/// ウィンドウの横幅を取得
	/// </summary>
	/// <returns>横幅(UINT型)</returns>
	inline UINT GetWidth() const noexcept { return m_unWidth; }
	/// <summary>
	/// ウィンドウの縦幅を取得
	/// </summary>
	/// <returns>縦幅(UINT型)</returns>
	inline UINT GetHeight() const noexcept { return m_unHeight; }

	/// <summary>
	/// タイトルバーの表示名を取得
	/// </summary>
	/// <returns>タイトルバーの表示名</returns>
	inline LPCSTR GetTitleName() const noexcept { return m_lpcTitleName; }

	/// <summary>
	/// ウィンドウクラス名の取得
	/// </summary>
	/// <returns>ウィンドウクラス名</returns>
	inline LPCSTR GetClassName() const noexcept { return m_lpcClassName; }

	/// <summary>
	/// 親ウィンドウのハンドルを取得
	/// </summary>
	/// <returns>親ウィンドウのハンドル</returns>
	inline HWND &GetParentHwnd() noexcept { return m_hWndParent; }

	/// <summary>
	/// クラスネームの設定
	/// </summary>
	/// <param name="[In_alpcName]">クラス名</param>
	void SetClassName(_In_ const LPCSTR &In_alpcName);

	/// <summary>
	/// 親ウィンドウの設定
	/// </summary>
	/// <param name="[In_hWndParent]">親ウィンドウへのハンドル</param>
	void SetParent(_In_ HWND In_hWndParent = HWND_DESKTOP);

	/// <summary>
	/// ウィンドウカラーの設定
	/// </summary>
	/// <param name="[In_nColor]">色パラメータ</param>
	void SetColor(_In_ const int &In_nColor = WHITE_BRUSH);
	/// <summary>
	/// アイコンの設定
	/// </summary>
	/// <param name="[In_ahIns]">HINSTANCEへの参照</param>
	/// <param name="[In_alpcName]">LPCTSTRへの参照</param>
	void SetIcon(_In_ HINSTANCE& In_ahIns, _In_ LPCTSTR& In_alpcName);
	/// <summary>
	/// カーソルアイコンの設定
	/// </summary>
	/// <param name="[In_ahIns]">HINSTANCEへの参照</param>
	/// <param name="[In_alpcName]">LPCTSTRへの参照</param>
	void SetCursorIcon(_In_ HINSTANCE& In_ahIns, _In_ LPCTSTR& In_alpcName);
	/// <summary>
	/// ウィンドウ挙動の設定
	/// </summary>
	/// <param name="[In_unStyle]">UINT型</param>
	void SetStyle(_In_ UINT In_unStyle);

	/// <summary>
	/// ウィンドウスタイルの設定
	/// </summary>
	/// <param name="[In_dwStyle]">ウィンドウスタイル値とコントロールスタイルの組み合わせ</param>
	void SetWindowStyle(_In_ DWORD In_dwStyle = WS_CAPTION | WS_SYSMENU);

	/// <summary>
	/// 拡張ウィンドウスタイルの設定
	/// </summary>
	/// <param name="[In_dwExStyle]">拡張ウィンドウスタイル</param>
	void SetWindowExStyle(_In_ DWORD In_dwExStyle = WS_EX_OVERLAPPEDWINDOW);

	/// <summary>
	/// メニューのハンドル又は子ウィンドウ識別子の設定
	/// </summary>
	/// <param name="[In_hMenu]">HMENU</param>
	void SetMenu(_In_ HMENU In_hMenu = NULL);

	/// <summary>
	/// MDIクライアントウィンドウを作成するときに使用
	/// </summary>
	/// <param name="[lpParam]">CLIENTCREATESTRUCTへのポインタ</param>
	void SetCreateStructParam(_In_ LPCLIENTCREATESTRUCT In_lpParam);

	/// <summary>
	/// MDI子ウィンドウを作成するときに使用
	/// </summary>
	/// <param name="[lpParam]">MDICREATESTRUCTへのポインタ</param>
	void SetCreateStructParam(_In_ LPMDICREATESTRUCT In_lpParam);

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
	WNDCLASSEX m_wcex;		// ウィンドウクラス情報
	HWND m_hWnd;			// ウィンドウハンドル
	UINT m_unWidth;			// ウィンドウの横幅
	UINT m_unHeight;		// ウィンドウの縦幅
	RECT m_rcWindowRect;	// ウィンドウの矩形
	MSG m_msgRoop;			// メッセージループ
	bool m_bIsCloseWindow;	// ウィンドウが閉じられたかどうか

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