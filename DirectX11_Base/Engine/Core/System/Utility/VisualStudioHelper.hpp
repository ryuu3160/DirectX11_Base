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
        int Priority;                // 優先度（高いほど優先）
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
    static bool OpenFileInRunningInstance(
        _In_ const std::filesystem::path &In_FilePath,
        _In_ int In_LineNumber = -1
    );

private:
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
};
