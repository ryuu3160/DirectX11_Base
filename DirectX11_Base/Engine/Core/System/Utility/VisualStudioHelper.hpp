/*+===================================================================
	File: VisualStudioHelper.hpp
	Summary: VisualStudioでファイルを開くためのヘルパークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 06:34:46 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

/// <summary>
/// VisualStudioHelperクラス
/// </summary>
class VisualStudioHelper
{
public:
    struct VSInfo
    {
        std::wstring Version;        // "2026", "2022" など
        std::wstring InstallPath;    // インストールパス
        std::wstring DevEnvPath;     // devenv.exe のパス
        int Priority = 0;                // 優先度（高いほど優先）
    };

    /// <summary>
    /// ファイルを Visual Studio で開く(最適なバージョンを自動選択)
    /// </summary>
    static bool OpenFileInVisualStudio(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath = "");

    /// <summary>
    /// インストールされているVisualStudioを検出
    /// </summary>
    static std::vector<VSInfo> DetectVisualStudioVersions();

    /// <summary>
    /// VisualStudioが実行中か確認
    /// </summary>
    static bool IsVisualStudioRunning();

    /// <summary>
    /// 実行中のVisualStudioでファイルを開く
    /// </summary>
    static bool OpenFileInRunningInstance(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath = "", _In_ int In_LineNumber = -1);

private:

	/// <summary>
	/// DTEを使用して実行中のインスタンスでファイルを開く
	/// </summary>
	/// <returns>ファイルが正常に開かれた場合はtrue、それ以外の場合はfalse</returns>
	static bool OpenFileInRunningInstanceWithDTE(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_LineNumber);
    
	/// <summary>
	/// 指定されたソリューションでVisualStudioを起動し、起動したインスタンスでファイルを開く
	/// </summary>
	/// <returns>タスクが正常に生成された場合はtrue、それ以外の場合はfalse</returns>
	static bool OpenFileInLaunchedInstance(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_LineNumber);

	/// <summary>
	/// ShellExecuteExを使用して実行中のインスタンスでファイルを開く
	/// </summary>
	/// <returns>ファイルが正常に開かれた場合はtrue、それ以外の場合はfalse</returns>
	static bool OpenFileInRunningInstanceWithDevenv(_In_ const std::filesystem::path &In_FilePath, _In_ int In_LineNumber);

    /// <summary>
	/// ソリューションを指定してVisualStudioを起動し、起動したインスタンスでファイルを開く
	/// </summary>
	/// <returns>起動に成功し、VisualStudioが起動している場合は有効なIStream*、それ以外の場合はnullptr</returns>
	static IStream *LaunchVisualStudioForSolution(_In_ std::wstring_view In_DevenExePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_TimeoutMs, _In_ int In_PoolIntervalMs);

	/// <summary>
	/// DTEを使用して実行中のインスタンスでファイルを開く
	/// </summary>
	static bool UseMarshaledDTEAndOpen(_In_opt_ IStream *In_pStream, _In_ const std::filesystem::path &In_FilePath);

    /// <summary>
    /// vswhere.exeを使ってVisualStudioを検出
    /// </summary>
    static std::vector<VSInfo> DetectUsingVSWhere();

    /// <summary>
    /// レジストリからVisualStudioを検出(フォールバック)
    /// </summary>
    static std::vector<VSInfo> DetectUsingRegistry();

    /// <summary>
    /// テキストエディタで開く(フォールバック)
    /// </summary>
    static bool OpenInTextEditor(_In_ const std::filesystem::path &In_FilePath);

	/// <summary>
	/// Visual Studioのメインウィンドウをアクティブにします
	/// </summary>
	/// <param name="[In_pDTE]">Visual StudioのDTEオブジェクトへのポインタ。nullptrの場合は実行中のVisual Studioを検索してアクティブにします</param>
	static void ActivateIDEMainWindow(_In_opt_ IDispatch *In_pDTE);
};
