/*+===================================================================
	File: IME.hpp
	Summary: IME制御用クラス
	Author: ryuu3160
	Date: 2025/01/29 16:38 初回作成
				 /30 12:30 入力用の関数など追加
			  /02/06 06:30 日本語入力対応,ウィンドウプロシージャで行う処理をまとめた関数を追加,Init関数を追加

	(C) 2025 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "IME.hpp"

void IME::Init(HWND hWnd) noexcept
{
	m_bIME_Enabled = false;
	m_bTypeEnabled = false;
	m_nFontSize = 0;
	m_strInput.clear();
	m_strOldInput.clear();

	m_hWnd = hWnd;
	m_hIMC = ImmGetContext(hWnd);
}

void IME::DefaultProssesInWinProc(UINT In_nMessage, WPARAM In_wParam, LPARAM In_lParam) noexcept
{
	switch (In_nMessage)
	{
	case WM_CHAR: // 英字入力時の処理
		// 入力が無効の場合
		if (!IsTypeEnabled()) break;

		// 日本語入力がOFFの場合
		if (!IsIME_Enabled())
		{
			InputEng(In_wParam);
		}
		break;

	case WM_IME_STARTCOMPOSITION:
		SetCompositionWindow();
		EnableIME();
		break;

	case WM_IME_CHAR:
		// 入力が無効の場合
		if (!IsTypeEnabled()) break;
		// 日本語入力がONの場合
		if (IsIME_Enabled())
			InputJp(In_wParam);
		break;

	case WM_IME_ENDCOMPOSITION:
		DisableIME();
		break;
	}
}

void IME::EnableIME() noexcept
{
	m_bIME_Enabled = true;
	ImmSetConversionStatus(m_hIMC, IME_CMODE_NATIVE, IME_SMODE_NONE);
}

void IME::DisableIME() noexcept
{
	m_bIME_Enabled = false;
	ImmSetConversionStatus(m_hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
}

void IME::InputJp(WPARAM In_wParam) noexcept
{
	// 10進数to16進数で使う変数
	char work[5];
	std::string strWork1;
	std::string strWork2;
	std::string strWork3;
	// 16進数に変換
	// 必ず4桁で返ってくる(入ってきたコードが文字コードであれば)
	sprintf_s(work, "%X", static_cast<int>(In_wParam));
	// 2桁ずつに分ける
	strWork1 = work[0];
	strWork1 += work[1];
	strWork2 = work[2];
	strWork2 += work[3];
	// 2桁ずつの16進数を10進数に変換して足す(これで文字になる)
	strWork3 = static_cast<char>(std::strtol(strWork1.c_str(), nullptr, 16));
	strWork3 += static_cast<char>(std::strtol(strWork2.c_str(), nullptr, 16));
	// 文字列に追加
	m_strInput += strWork3;
}

void IME::InputEng(WPARAM In_wParam) noexcept
{
	// 記号&英数字
	if (In_wParam >= 33 && In_wParam <= 126)
		m_strInput += static_cast<char>(In_wParam);
	else if (In_wParam == VK_BACK) // バックスペース
		if(!m_strInput.empty()) // 文字入力がない場合は処理しない
			m_strInput.pop_back();
	else if (In_wParam == VK_SPACE) // スペース
		m_strInput += " ";
}

std::string IME::PopInput()
{
	std::string strReturnVal = m_strInput;
	m_strInput.clear();
	return strReturnVal;
}

void IME::SetCompositionWindowPos(const SF2Position& In_f2Pos) noexcept
{
	m_f2CompositionWindowPos = In_f2Pos;

	// IME用のハンドルが生成されていない場合はスルー
	if (m_hIMC == nullptr)
		return;

	LPCOMPOSITIONFORM lpCompForm = new COMPOSITIONFORM();
	ImmGetCompositionWindow(m_hIMC, lpCompForm); // 現在のコンポジションウィンドウ情報を取得
	lpCompForm->dwStyle = CFS_POINT; // 表示座標で使用するデータをポイントに指定
	lpCompForm->ptCurrentPos.x = static_cast<LONG>(m_f2CompositionWindowPos.x); // X座標設定
	lpCompForm->ptCurrentPos.y = static_cast<LONG>(m_f2CompositionWindowPos.y); // Y座標設定
	ImmSetCompositionWindow(m_hIMC, lpCompForm); // コンポジションウィンドウの情報を設定
	if (lpCompForm)
	{
		delete lpCompForm;
		lpCompForm = nullptr;
	}
}

void IME::SetCompositionFont(int In_nFontSizeHeight, int In_nFontSizeWidth, BYTE In_lfWeight,
	BYTE In_lfItalic, BYTE In_lfUnderline, BYTE In_lfStrikeOut, BYTE In_lfCharSet) noexcept
{
	LPLOGFONTA lpLogFont = new LOGFONTA(); // LOGFONTAで初期化
	// 現在のIMEのfont情報を取得、出来なかった場合はreturn
	if (!ImmGetCompositionFontA(m_hIMC, lpLogFont))
	{
		delete lpLogFont;
		lpLogFont = nullptr;
		return;
	}
	
	// フォントの情報を設定
	lpLogFont->lfHeight = In_nFontSizeHeight; // フォントの高さ
	lpLogFont->lfWidth = In_nFontSizeWidth; // フォントの幅
	lpLogFont->lfWeight = In_lfWeight; // フォントの太さ
	lpLogFont->lfItalic = In_lfItalic; // 斜体かどうか
	lpLogFont->lfUnderline = In_lfUnderline; // 下線付きかどうか
	lpLogFont->lfStrikeOut = In_lfStrikeOut; // 取り消し線付きかどうか
	lpLogFont->lfCharSet = In_lfCharSet; // 文字セット

	*m_pLogFont = *lpLogFont; // フォント情報を保存

	// フォントの情報を設定
	if (!ImmSetCompositionFontA(m_hIMC, lpLogFont))
	{
		// fontの設定に失敗した場合
		throw std::invalid_argument("フォント情報の設定に失敗しました。");
	}

	// 使用した構造体の開放
	delete lpLogFont;
	lpLogFont = nullptr;
}

void IME::SetCompositionWindow() const noexcept
{
	// 位置情報設定
	LPCOMPOSITIONFORM lpCompForm = new COMPOSITIONFORM();
	ImmGetCompositionWindow(m_hIMC, lpCompForm); // 現在のコンポジションウィンドウ情報を取得
	lpCompForm->dwStyle = CFS_POINT; // 表示座標で使用するデータをポイントに指定
	lpCompForm->ptCurrentPos.x = static_cast<LONG>(m_f2CompositionWindowPos.x); // X座標設定
	lpCompForm->ptCurrentPos.y = static_cast<LONG>(m_f2CompositionWindowPos.y); // Y座標設定
	ImmSetCompositionWindow(m_hIMC, lpCompForm); // コンポジションウィンドウの情報を設定
	if (lpCompForm)
	{
		delete lpCompForm;
		lpCompForm = nullptr;
	}

	// フォントの情報を設定
	if(m_pLogFont != nullptr)
		ImmSetCompositionFontA(m_hIMC, m_pLogFont); // フォントの情報を設定
}

IME::IME() noexcept
	: m_bIME_Enabled(false), m_bTypeEnabled(false)
	, m_nFontSize(0), m_hIMC(nullptr), m_hWnd(nullptr)
	, m_strInput(""), m_strOldInput("")
	, m_pLogFont(nullptr)
	, m_f2CompositionWindowPos({ 0.0f, 0.0f })
{
}

IME::~IME()
{
	ImmReleaseContext(m_hWnd, m_hIMC);
}
