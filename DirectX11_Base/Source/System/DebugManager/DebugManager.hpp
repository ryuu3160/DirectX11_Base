/*+===================================================================
	File: DebugManager.hpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:16 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/DebugManager/InitializeImGui.hpp"
#include "System/DebugManager/DebugWindow.hpp"
#include "System/DebugManager/DebugItem.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// DebugManagerクラス
/// </summary>
class DebugManager : public Singleton<DebugManager>
{
	friend class Singleton<DebugManager>;
public:
	DebugManager();
	~DebugManager();

	void Init();

	void Update() noexcept;

	void Draw() noexcept;

	/// <summary>
	/// デバッグウィンドウを作成する
	/// </summary>
	/// <param name="[In_Name]">ウィンドウ名</param>
	/// <returns>作成されたウィンドウへのポインタ</returns>
	DebugWindow *CreateDebugWindow(_In_ const std::string_view In_Name);

	/// <summary>
	/// 指定した名前に対応するデバッグウィンドウを取得します。
	/// </summary>
	/// <param name="[In_Name]">検索するデバッグウィンドウの名前を示す読み取り専用の std::string_view。</param>
	/// <returns>該当する DebugWindow へのポインタ。指定した名前のウィンドウが見つからない場合は nullptr を返します。</returns>
	DebugWindow *GetDebugWindow(_In_ const std::string_view In_Name);

private:

	void DrawImGui(_In_ DebugItem* In_Item) noexcept;

private:
	std::unordered_map<std::string, DebugWindow*> m_DebugWindows;
};
