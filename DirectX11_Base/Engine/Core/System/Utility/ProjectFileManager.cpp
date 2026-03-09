/*+===================================================================
	File: ProjectFileManager.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 07:27:08 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ProjectFileManager.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

bool ProjectFileManager::AddFileToProject(_In_ const std::filesystem::path &In_ProjectPath, _In_ const FileEntry &In_Entry)
{
    if(!std::filesystem::exists(In_ProjectPath))
    {
        DebugManager::GetInstance().DebugLogError("Project file not found: {}",
            In_ProjectPath.string());
        return false;
    }

    // プロジェクトファイルを読み込み
    std::string Content = LoadXmlFile(In_ProjectPath);
    if(Content.empty())
        return false;

    // 相対パスに変換
    std::filesystem::path ProjectDir = In_ProjectPath.parent_path();
    std::string RelativePath = ToRelativePath(In_Entry.FilePath, ProjectDir);

    // エントリを作成
    std::string Entry;
    if(In_Entry.IsHeader)
    {
        Entry = "    <ClInclude Include=\"" + RelativePath + "\" />";
    }
    else
    {
        Entry = "    <ClCompile Include=\"" + RelativePath + "\">\n";
        Entry += "      <PrecompiledHeader>Use</PrecompiledHeader>\n";
        Entry += "    </ClCompile>";
    }

    // 既に存在するかチェック
    if(Content.find(RelativePath) != std::string::npos)
    {
        DebugManager::GetInstance().DebugLogWarning("File already in project: {}", RelativePath);
        return true;
    }

    // 挿入位置を探す
    std::string SearchTag = In_Entry.IsHeader ? "</ItemGroup>" : "</ItemGroup>";
    size_t InsertPos = Content.rfind(SearchTag);

    if(InsertPos == std::string::npos)
    {
        DebugManager::GetInstance().DebugLogError("Failed to find insertion point in project file");
        return false;
    }

    // エントリを挿入
    Content.insert(InsertPos, Entry + "\n  ");

    // ファイルに保存
    if(!SaveXmlFile(In_ProjectPath, Content))
        return false;

    return true;
}

bool ProjectFileManager::AddFileToFilters(_In_ const std::filesystem::path &In_FiltersPath, _In_ const FileEntry &In_Entry)
{
    if(!std::filesystem::exists(In_FiltersPath))
    {
        DebugManager::GetInstance().DebugLogWarning("Filters file not found: {}",
            In_FiltersPath.string());
        return false;
    }

    // フィルターファイルを読み込み
    std::string Content = LoadXmlFile(In_FiltersPath);
    if(Content.empty())
        return false;

    // 相対パスに変換
    std::filesystem::path ProjectDir = In_FiltersPath.parent_path();
    std::string RelativePath = ToRelativePath(In_Entry.FilePath, ProjectDir);

    // エントリを作成
    std::string Entry;
    if(In_Entry.IsHeader)
    {
        Entry = "    <ClInclude Include=\"" + RelativePath + "\">\n";
        Entry += "      <Filter>" + In_Entry.FilterPath + "</Filter>\n";
        Entry += "    </ClInclude>";
    }
    else
    {
        Entry = "    <ClCompile Include=\"" + RelativePath + "\">\n";
        Entry += "      <Filter>" + In_Entry.FilterPath + "</Filter>\n";
        Entry += "    </ClCompile>";
    }

    // 既に存在するかチェック
    if(Content.find(RelativePath) != std::string::npos)
    {
        DebugManager::GetInstance().DebugLogWarning("File already in filters: {}", RelativePath);
        return true;
    }

    // 挿入位置を探す
    size_t InsertPos = Content.rfind("</ItemGroup>");

    if(InsertPos == std::string::npos)
    {
        DebugManager::GetInstance().DebugLogError("Failed to find insertion point in filters file");
        return false;
    }

    // エントリを挿入
    Content.insert(InsertPos, Entry + "\n  ");

    // ファイルに保存
    if(!SaveXmlFile(In_FiltersPath, Content))
        return false;

    return true;
}

bool ProjectFileManager::AddCppScriptToProject(_In_ const std::filesystem::path &In_CppPath, _In_ const std::filesystem::path &In_HppPath, _In_ const std::string &In_FilterPath)
{
    // プロジェクトファイルのパス
	std::filesystem::path Directory = std::filesystem::current_path();
    std::filesystem::path ProjectPath = Directory / "DirectX11_Base.vcxproj";
    std::filesystem::path FiltersPath = Directory / "DirectX11_Base.vcxproj.filters";

    // .cpp を追加
    FileEntry CppEntry;
    CppEntry.FilePath = In_CppPath;
    CppEntry.FilterPath = In_FilterPath;
    CppEntry.IsHeader = false;

    if(!AddFileToProject(ProjectPath, CppEntry))
        return false;

    if(std::filesystem::exists(FiltersPath))
    {
        AddFileToFilters(FiltersPath, CppEntry);
    }

    // .hpp を追加
    FileEntry HppEntry;
    HppEntry.FilePath = In_HppPath;
    HppEntry.FilterPath = In_FilterPath;
    HppEntry.IsHeader = true;

    if(!AddFileToProject(ProjectPath, HppEntry))
        return false;

    if(std::filesystem::exists(FiltersPath))
    {
        AddFileToFilters(FiltersPath, HppEntry);
    }

    return true;
}

std::string ProjectFileManager::LoadXmlFile(_In_ const std::filesystem::path &In_Path)
{
    std::ifstream file(In_Path);
    if(!file.is_open())
    {
        DebugManager::GetInstance().DebugLogError("Failed to open file: {}", In_Path.string());
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ProjectFileManager::SaveXmlFile(_In_ const std::filesystem::path &In_Path, _In_ const std::string &In_Content)
{
    std::ofstream file(In_Path);
    if(!file.is_open())
    {
        DebugManager::GetInstance().DebugLogError("Failed to write file: {}", In_Path.string());
        return false;
    }

    file << In_Content;
    file.close();
    return true;
}

std::string ProjectFileManager::ToRelativePath(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_BasePath)
{
    try
    {
        std::filesystem::path RelativePath = std::filesystem::relative(In_FilePath, In_BasePath);
        std::string Result = RelativePath.string();

        // Unix スタイルのパス区切りを Windows スタイルに変換
        std::replace(Result.begin(), Result.end(), '/', '\\');

        return Result;
    }
    catch(...)
    {
        return In_FilePath.string();
    }
}

