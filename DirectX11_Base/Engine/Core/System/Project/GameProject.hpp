/*+===================================================================
	File: GameProject.hpp
	Summary: ゲームプロジェクト管理
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu AM 01:36:15 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/ryuu_lib/Singleton/Singleton.hpp"

// ==============================
//  前方宣言
// ==============================
class DebugModal;

/// <summary>
/// ゲームプロジェクト情報
/// </summary>
struct GameProjectInfo
{
	std::string Name;                           // プロジェクト名
	std::string Version;                        // バージョン
	std::filesystem::path RootPath;             // プロジェクトルート
	std::filesystem::path SolutionPath;         // .sln パス
	std::filesystem::path ProjectPath;          // .vcxproj パス
	std::filesystem::path ScriptsPath;          // スクリプトフォルダ
	std::filesystem::path AssetsPath;           // アセットフォルダ
};

/// <summary>
/// GameProjectクラス
/// </summary>
class GameProjectManager : public Singleton<GameProjectManager>
{
	friend class Singleton<GameProjectManager>;
public:
    /// <summary>
    /// 新規プロジェクトを作成
    /// </summary>
    bool CreateNewProject(_In_ std::string_view In_ProjectName, _In_ const std::filesystem::path &In_Location);

    /// <summary>
    /// プロジェクトを開く
    /// </summary>
    bool OpenProject(_In_ const std::filesystem::path &In_ProjectPath);

    /// <summary>
    /// 現在のプロジェクトを閉じる
    /// </summary>
    void CloseProject();

    /// <summary>
    /// 現在のプロジェクト情報を取得
    /// </summary>
    const GameProjectInfo *GetCurrentProject() const;

    /// <summary>
    /// プロジェクトがロードされているか
    /// </summary>
    bool IsProjectLoaded() const { return m_CurrentProject.has_value(); }

    /// <summary>
    /// DLL をロード
    /// </summary>
    bool LoadProjectDll();

    /// <summary>
    /// DLL をアンロード
    /// </summary>
    void UnloadProjectDll();

    /// <summary>
    /// DLL をリロード
    /// </summary>
    bool ReloadProjectDll();

    /// <summary>
	/// ロードされたプロジェクトのビルドフォルダを取得
    /// </summary>
    std::filesystem::path GetLoadedBuildFolder() const;

private:
	GameProjectManager();
	~GameProjectManager();

    /// <summary>
	/// リロード中に表示するUIを作成
    /// </summary>
    void CreateReloadingUI();

    /// <summary>
    /// プロジェクト情報を保存します
    /// </summary>
    void SaveProjectInfo();

    /// <summary>
    /// プロジェクト情報を読み込みます
    /// </summary>
    void LoadProjectInfo();

    /// <summary>
    /// フォルダ構造を作成
    /// </summary>
    bool CreateProjectStructure(_In_ const std::filesystem::path &In_ProjectRoot);

    /// <summary>
    /// テンプレートからプロジェクトファイルを生成
    /// </summary>
    bool GenerateProjectFiles(_In_ const std::filesystem::path &In_ProjectRoot, _In_ std::string_view In_ProjectName);

    /// <summary>
    /// GUID を生成
    /// </summary>
    std::string GenerateGuid();

    /// <summary>
    /// ビルドルートパスを生成します
    /// </summary>
    /// <returns>ビルドルートパス</returns>
    std::filesystem::path GenerateBuildRootPath() const;

    /// <summary>
    /// プロジェクトウィンドウでプロジェクトを開きます
    /// </summary>
    /// <param name="[In_ProjectName]">開くプロジェクトの名前</param>
    /// <param name="[In_Location]">プロジェクトのファイルシステムパス</param>
    void OpenProjectInProjectWindow(_In_ std::string_view In_ProjectName, _In_ const std::filesystem::path &In_Location);

    /// <summary>
    /// パッケージ構成をテンプレートに同期します
    /// </summary>
    /// <param name="[In_EngineRoot]">エンジンのルートパス</param>
    /// <returns>同期操作が成功した場合はtrue、失敗した場合はfalse</returns>
    bool SyncPackagesConfigToTemplate(_In_ const std::filesystem::path &In_EngineRoot);

    /// <summary>
    /// VisualC++プロジェクトファイルにNuGetインポートのパッチを適用します
    /// </summary>
    /// <param name="[In_ProjectRoot]">プロジェクトのルートディレクトリのパス</param>
    /// <param name="[In_VcxprojPath]">.vcxproj ファイルのパス</param>
    /// <returns>パッチの適用に成功した場合はtrue、失敗した場合はfalse</returns>
    bool PatchVcxprojWithNuGetImports(_In_ const std::filesystem::path &In_ProjectRoot, _In_ const std::filesystem::path &In_VcxprojPath);

    /// <summary>
    /// <para>PatchVcxprojWithNuGetImportsのヘルパー関数</para>
	/// <para>指定されたディレクトリ内で、NuGetのビルドファイル（.targetsや.props）を検索し、そのパスを返します</para>
    /// </summary>
    std::vector<std::filesystem::path> FindNuGetBuildFiles(_In_ const std::filesystem::path &In_PackagesDir, _In_ const std::wstring &In_Ext);

	/// <summary>
	/// <para>PatchVcxprojWithNuGetImportsのヘルパー関数</para>
	/// <para>指定されたプロジェクトルートとビルドファイルのパスから、.vcxprojに追加するためのインポート行を生成します</para>
	/// </summary>
    std::string MakeImportLineForVcxproj(_In_ const std::filesystem::path &In_ProjectRoot, _In_ const std::filesystem::path &In_FilePath);

    /// <summary>
	/// DLLからコンポーネントを登録します
    /// </summary>
    bool RegisterComponentsFromLoadedDll() const;

    /// <summary>
    /// エディター設定用のプロジェクトDLLパスを取得します
    /// </summary>
    /// <returns>エディター設定用のプロジェクトDLLパス</returns>
    std::filesystem::path GetProjectDllPathForEditorConfig() const;

    /// <summary>
    /// 最新のビルドされたDLLのパスを検索します
    /// </summary>
    /// <param name="[In_BuildRootPath]">ビルドルートへのパス</param>
    /// <returns>最新のビルドされたDLLのファイルシステムパス</returns>
    std::filesystem::path FindLatestBuiltDllPath(_In_ std::filesystem::path In_BuildRootPath) const;

    /// <summary>
    /// 古いビルドフォルダを削除します
    /// </summary>
    /// <param name="[In_BuildRootPath]">ビルドルートへのパス</param>
    /// <param name="[In_KeepCount]">保持するビルドフォルダの数</param>
    void PruneOldBuildFolders(_In_ std::filesystem::path In_BuildRootPath, _In_ size_t In_KeepCount) const;

    /// <summary>
    /// ビルド出力が使用可能かどうかを判定
    /// </summary>
    /// <param name="[In_BuildFolder]">ビルドフォルダーのパス</param>
    /// <param name="[In_ProjectName]">プロジェクトの名前</param>
    /// <returns>ビルド出力が使用可能な場合はtrue、それ以外の場合はfalse</returns>
    bool IsBuildOutputUsable(_In_ const std::filesystem::path &In_BuildFolder, _In_ std::string_view In_ProjectName) const;

	/// <summary>
	/// 最新のビルド出力を待機します
	/// </summary>
	/// <param name="[In_TimeoutMs]">待機する最大時間（ミリ秒）</param>
	/// <returns>最新のビルド出力が見つかった場合はtrue、タイムアウトした場合はfalseを返します</returns>
	bool WaitForLatestBuildOutput(_In_ std::chrono::milliseconds In_TimeoutMs) const;

    /// <summary>
	/// プロジェクトのビルドを監視します
    /// </summary>
    void WatchProjectBuild();

    /// <summary>
    /// ファイル監視を開始
    /// </summary>
    void StartWatching();

    /// <summary>
    /// ファイル監視を停止
    /// </summary>
    void StopWatching();

private:
    std::optional<GameProjectInfo> m_CurrentProject;
    HMODULE m_hProjectDll;
    std::filesystem::path m_LoadedDllPath;

	// リロード中のUI
    DebugModal *m_ReloadingWindow;
    ImGuiViewport *m_MainViewport;

    // ファイル監視&自動リロード関連
    std::thread m_WatcherThread;
	std::thread m_ReloadingThread;
    HANDLE m_hDirectory;
    OVERLAPPED m_Overlapped;  // 非同期I/O用
    HANDLE m_hStopEvent;      // 停止イベント
    std::mutex m_DirectoryHandleMutex;
    std::mutex m_ReloadingUiMutex;
    std::atomic<bool> m_IsWatching;
    std::atomic<bool> m_IsReloading;
};
