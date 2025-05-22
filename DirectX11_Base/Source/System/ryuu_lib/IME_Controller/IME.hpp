/*+===================================================================
	File: IME.hpp
	Summary: IME制御クラス
	Author: ryuu3160
	Date: 2025/01/29 16:38 初回作成
				 /30 12:30 入力用の関数など追加
			  /02/06 06:30 日本語入力対応,ウィンドウプロシージャで行う処理をまとめた関数を追加,Init関数を追加

	(C) 2025 ryuu3160. All rights reserved.
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <imm.h>
#pragma comment(lib, "imm32.lib")

/// <summary>
/// IMEクラス
/// </summary>
class IME final : public Singleton<IME>
{
	friend class Singleton<IME>;
public:

	// 2D座標の構造体
	struct SF2Position
	{
		float x; // x座標
		float y; // y座標
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="[In_hWnd]">ウィンドウハンドル</param>
	void Init(HWND hWnd) noexcept;

	/// <summary>
	/// デフォルトのウィンドウプロシージャで行う処理
	/// </summary>
	/// <param name="[In_nMessage]">メッセージ</param>
	/// <param name="[wParam]">wParam</param>
	/// <param name="[lParam]">lParam</param>
	void DefaultProssesInWinProc(UINT In_nMessage, WPARAM wParam, LPARAM lParam) noexcept;

	/// <summary>
	/// <para>IMEの有効化</para>
	/// <para>ここで言うIMEの有効化は、日本語入力の有効化を指す</para>
	/// </summary>
	void EnableIME() noexcept;

	/// <summary>
	/// <para>IMEの無効化</para>
	/// <para>ここで言うIMEの無効化は、日本語入力の無効化を指す</para>
	/// </summary>
	void DisableIME() noexcept;

	/// <summary>
	/// 文字入力の有効化
	/// </summary>
	inline void EnableType() noexcept { m_bTypeEnabled = true; }

	/// <summary>
	/// 文字入力の無効化
	/// </summary>
	inline void DisableType() noexcept { m_bTypeEnabled = false; }

	/// <summary>
	/// <para>入力情報を取得する</para>
	/// <para>日本語入力専用</para>
	/// </summary>
	/// <param name="[In_wParam]">文字コード</param>
	void InputJp(WPARAM In_wParam) noexcept;

	/// <summary>
	/// <para>入力情報を取得する</para>
	/// <para>ローマ字入力専用</para>
	/// </summary>
	/// <param name="[In_wParam]">JISコード</param>
	void InputEng(WPARAM In_wParam) noexcept;

	/// <summary>
	/// 保存されている入力情報をクリアする
	/// </summary>
	inline void Clear() { m_strInput.clear(); }

	// ==============================
	//  Getter
	// ==============================

	/// <summary>
	/// 入力情報を取得する
	/// </summary>
	/// <returns>入力情報</returns>
	inline const std::string& GetInput() const noexcept
	{ return m_strInput; }

	/// <summary>
	/// 入力情報を取得し、保存されている入力情報をクリアする
	/// </summary>
	/// <returns>入力情報</returns>
	std::string PopInput();

	/// <summary>
	/// IMEが有効かどうかを取得する
	/// </summary>
	/// <returns>IMEが有効かどうか</returns>
	inline const bool& IsIME_Enabled() const noexcept
	{ return m_bIME_Enabled; }

	/// <summary>
	/// 文字入力が有効かどうかを取得する
	/// </summary>
	/// <returns>文字入力が有効かどうか</returns>
	inline const bool& IsTypeEnabled() const noexcept
	{ return m_bTypeEnabled; }

	// ==============================
	//  Setter
	// ==============================

	/// <summary>
	/// 入力ウィンドウの位置を設定する
	/// </summary>
	/// <param name="[In_f3Pos]">位置</param>
	void SetCompositionWindowPos(const SF2Position& In_f2Pos) noexcept;

	/// <summary>
	/// 入力ウィンドウのフォント情報を設定する
	/// </summary>
	/// <param name="[In_nFontSizeHeight]">フォントの高さ</param>
	/// <param name="[In_nFontSizeWidth]">フォントの幅</param>
	/// <param name="[In_lfWeight]">フォントの太さ</param>
	/// <param name="[In_lfItalic]">斜体かどうか</param>
	/// <param name="[In_lfUnderline]">下線付きかどうか</param>
	/// <param name="[In_lfStrikeOut]">取り消し線付きかどうか</param>
	/// <param name="[In_lfCharSet]">文字セット</param>
	void SetCompositionFont(int In_nFontSizeHeight, int In_nFontSizeWidth = 0, BYTE In_lfWeight = FW_NORMAL,
		BYTE In_lfItalic = false, BYTE In_lfUnderline = false, BYTE In_lfStrikeOut = false, BYTE In_lfCharSet = DEFAULT_CHARSET) noexcept;

	/// <summary>
	/// 既に設定されている情報を使用した入力ウィンドウの諸々の設定
	/// </summary>
	void SetCompositionWindow() const noexcept;

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IME() noexcept;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~IME();

private:
	HIMC m_hIMC; // IMEのコンテキスト
	HWND m_hWnd; // ウィンドウハンドル

	std::string m_strInput;		// 入力情報
	std::string m_strOldInput;	// 1フレーム前の入力情報
	bool m_bIME_Enabled;		// IMEが有効かどうか
	bool m_bTypeEnabled;		// 文字入力が有効かどうか

	LPLOGFONTA m_pLogFont;		// フォント情報
	int m_nFontSize;			// フォントサイズ

	SF2Position m_f2CompositionWindowPos; // 入力ウィンドウの位置
};
