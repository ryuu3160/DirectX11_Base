/*+===================================================================
	File: ProjectFileManager.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 07:27:02 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	定数定義
// ==============================
namespace
{
}

/// <summary>
/// ProjectFileManagerクラス
/// </summary>
class ProjectFileManager
{
public:
    struct FileEntry
    {
        std::filesystem::path FilePath;     // ファイルパス
        std::string FilterPath;              // フィルター（Game/Script/AI など）
        bool IsHeader;                       // .hpp の場合 true
    };

    /// <summary>
    /// .vcxproj にファイルを追加
    /// </summary>
    static bool AddFileToProject(_In_ const std::filesystem::path &In_ProjectPath, _In_ const FileEntry &In_Entry);

    /// <summary>
    /// .vcxproj.filters にフィルターエントリを追加
    /// </summary>
    static bool AddFileToFilters(_In_ const std::filesystem::path &In_FiltersPath, _In_ const FileEntry &In_Entry);

    /// <summary>
    /// C++ スクリプトをプロジェクトに追加（.cpp + .hpp）
    /// </summary>
    static bool AddCppScriptToProject(_In_ const std::filesystem::path &In_CppPath, _In_ const std::filesystem::path &In_HppPath, _In_ const std::string &In_FilterPath);

private:
    /// <summary>
    /// XMLファイルを読み込み
    /// </summary>
    static std::string LoadXmlFile(_In_ const std::filesystem::path &In_Path);

    /// <summary>
    /// XMLファイルに書き込み
    /// </summary>
    static bool SaveXmlFile(_In_ const std::filesystem::path &In_Path, _In_ const std::string &In_Content);

    /// <summary>
    /// 相対パスに変換
    /// </summary>
    static std::string ToRelativePath(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_BasePath);
};
