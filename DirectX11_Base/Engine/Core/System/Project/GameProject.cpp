/*+===================================================================
	File: GameProject.cpp
	Summary: ゲームプロジェクト管理
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/12 Thu AM 01:36:20 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GameProject.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include "Core/System/Managers/DebugManager/Window/DebugWindow.hpp"
#include "Core/System/Managers/DebugManager/Item/DebugItem.hpp"
#include "Core/System/Managers/DebugManager/Item/SystemItem.hpp"
#include "Core/System/Component/ComponentRegistry.hpp"
#include "Core/System/Input/Input.hpp"
#include "NuGetRestore.hpp"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <combaseapi.h>

// エンジンAPI
#include "EngineAPI/EngineAPI_Implementation.hpp"

// DLLからエクスポートされる関数の型定義
using RegisterAllGameComponentsFn = void(*)(void(*)(const ComponentInfo &));
// コンポーネント登録関数のプロトタイプ宣言
static void RegisterIntoEngineRegistry(const ComponentInfo &info)
{
    ComponentRegistry::GetInstance().RegisterComponent(info);
}

// エディタ構成名を取得する関数
static std::string GetEditorConfigName()
{
#ifdef _DEBUG
    return "Debug";
#else
    return "Release";
#endif
}
// ファイル/フォルダの最終更新日時を取得する関数
static std::filesystem::file_time_type SafeLastWriteTime(_In_ const std::filesystem::path &In_Path)
{
    std::error_code ec;
    auto time = std::filesystem::last_write_time(In_Path, ec);
    if(ec)
        return std::filesystem::file_time_type::min();
    return time;
}
// ファイルが共有読み取り可能か確認する関数
static bool CanOpenForReadShared(_In_ const std::filesystem::path &In_Path)
{
#ifdef _WIN32
    const auto w = In_Path.wstring();
    HANDLE h = CreateFileW(
        w.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // 共有許可(読めるかだけ見たい)
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if(h == INVALID_HANDLE_VALUE)
        return false;

    CloseHandle(h);
    return true;
#else
    // 非Windows
    std::error_code ec;
    return std::filesystem::exists(In_Path, ec);
#endif
}

bool GameProjectManager::CreateNewProject(_In_ std::string_view In_ProjectName, _In_ const std::filesystem::path &In_Location)
{
    std::string ProjectName(In_ProjectName);

    // プロジェクト名の検証
    if(ProjectName.empty())
    {
        DebugManager::GetInstance().DebugLogError("Project name cannot be empty");
        return false;
    }

    // プロジェクトルートパス
    std::filesystem::path ProjectRoot = In_Location / ProjectName;

    // 既に存在するかチェック
    if(std::filesystem::exists(ProjectRoot))
    {
        DebugManager::GetInstance().DebugLogError("Project already exists: {}", ProjectRoot.string());
        return false;
    }

    DebugManager::GetInstance().DebugLog("========================================");
    DebugManager::GetInstance().DebugLog("Creating new project: {}", ProjectName);
    DebugManager::GetInstance().DebugLog("========================================");

    // フォルダ構造を作成
    if(!CreateProjectStructure(ProjectRoot))
    {
        DebugManager::GetInstance().DebugLogError("Failed to create project structure");
        return false;
    }

    // プロジェクトファイルを生成
    if(!GenerateProjectFiles(ProjectRoot, ProjectName))
    {
        DebugManager::GetInstance().DebugLogError("Failed to generate project files");
        return false;
    }

    // 現在ロードされているプロジェクトがある場合は閉じる
    if(m_CurrentProject.has_value())
        CloseProject();

    // プロジェクト情報を設定
    GameProjectInfo info;
    info.Name = ProjectName;
    info.Version = "1.0.0";
    info.RootPath = ProjectRoot;
    info.SolutionPath = ProjectRoot / (ProjectName + ".sln");
    info.ProjectPath = ProjectRoot / (ProjectName + ".vcxproj");
    info.ScriptsPath = ProjectRoot / "Assets" / "Scripts";
    info.AssetsPath = ProjectRoot / "Assets";

    m_CurrentProject = info;

    DebugManager::GetInstance().DebugLog("  Project created successfully!");
    DebugManager::GetInstance().DebugLog("  Location: {}", ProjectRoot.string());
    DebugManager::GetInstance().DebugLog("========================================");

	// プロジェクトウィンドウでプロジェクトを開く
	OpenProjectInProjectWindow(ProjectName, ProjectRoot);

	// ビルド監視を開始
    StartWatching();

    return true;
}

bool GameProjectManager::CreateProjectStructure(_In_ const std::filesystem::path &In_ProjectRoot)
{
    try
    {
        // ルートフォルダ
        std::filesystem::create_directories(In_ProjectRoot);

        // Assets フォルダ
        std::filesystem::create_directory(In_ProjectRoot / "Assets");
        std::filesystem::create_directory(In_ProjectRoot / "Assets" / "Scripts");
        std::filesystem::create_directory(In_ProjectRoot / "Assets" / "Textures");
        std::filesystem::create_directory(In_ProjectRoot / "Assets" / "Scenes");

        // Binaries フォルダ
        std::filesystem::create_directory(In_ProjectRoot / "Binaries");
        std::filesystem::create_directory(In_ProjectRoot / "Binaries" / "Debug");
        std::filesystem::create_directory(In_ProjectRoot / "Binaries" / "Release");
        std::filesystem::create_directory(In_ProjectRoot / "Binaries" / "Debug" / "Builds");
        std::filesystem::create_directory(In_ProjectRoot / "Binaries" / "Release" / "Builds");

        // Intermediate フォルダ
        std::filesystem::create_directory(In_ProjectRoot / "Intermediate");

        // Generated フォルダ
        std::filesystem::create_directory(In_ProjectRoot / "Generated");

        DebugManager::GetInstance().DebugLog("Project structure created");
        return true;
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to create project structure: {}", e.what());
        return false;
    }
}

bool GameProjectManager::GenerateProjectFiles(_In_ const std::filesystem::path &In_ProjectRoot, _In_ std::string_view In_ProjectName)
{
    // テンプレートパス
    std::filesystem::path TemplateDir = "Engine/Templates/ProjectTemplate";

    if(!std::filesystem::exists(TemplateDir))
    {
        DebugManager::GetInstance().DebugLogError("Project template not found: {}", TemplateDir.string());
        return false;
    }

    // エンジンパス(絶対パス)
    std::filesystem::path EnginePath = std::filesystem::current_path().string();
    std::string EnginePathStr = EnginePath.string();
    std::replace(EnginePathStr.begin(), EnginePathStr.end(), '/', '\\');

    // GUIDを生成
    std::string ProjectGuid = GenerateGuid();

    // 置換マップ
    std::map<std::string, std::string> replacements;
    replacements["{PROJECT_NAME}"] = std::string(In_ProjectName);
    replacements["{PROJECT_GUID}"] = ProjectGuid;
    replacements["{ENGINE_PATH}"] = EnginePathStr;

    // .vcxprojを生成
    std::filesystem::path TemplateVcxproj = TemplateDir / "GameProject.vcxproj";
    std::filesystem::path OutputVcxproj = In_ProjectRoot / (std::string(In_ProjectName) + ".vcxproj");

    if(!GenerateFromTemplate(TemplateVcxproj, OutputVcxproj, replacements))
        return false;

    // .vcxproj.filtersを生成
    std::filesystem::path TemplateFilters = TemplateDir / "GameProject.vcxproj.filters";
    std::filesystem::path OutputFilters = In_ProjectRoot / (std::string(In_ProjectName) + ".vcxproj.filters");

    if(std::filesystem::exists(TemplateFilters))
        GenerateFromTemplate(TemplateFilters, OutputFilters, replacements);

    // .slnを生成
    std::filesystem::path TemplateSln = TemplateDir / "GameProject.sln";
    std::filesystem::path OutputSln = In_ProjectRoot / (std::string(In_ProjectName) + ".sln");

    if(!GenerateFromTemplate(TemplateSln, OutputSln, replacements))
        return false;

	// DllMain.cppを生成
    std::filesystem::path TemplateDllMain = TemplateDir / "DllMain.cpp";
    std::filesystem::path OutputDllMain = In_ProjectRoot / "DllMain.cpp";

    if(std::filesystem::exists(TemplateDllMain))
    {
        if(!Util::GenerateFromTemplate(TemplateDllMain, OutputDllMain, replacements))
            DebugManager::GetInstance().DebugLogWarning("Failed to generate DllMain.cpp");
    }

	// pch.h/cppを生成
	std::filesystem::path TemplatePchH = TemplateDir / "pch.h";
	std::filesystem::path OutputPchH = In_ProjectRoot / "pch.h";

	if(std::filesystem::exists(TemplatePchH))
    {
        if(!Util::GenerateFromTemplate(TemplatePchH, OutputPchH, replacements))
            DebugManager::GetInstance().DebugLogWarning("Failed to generate pch.h");
    }

	std::filesystem::path TemplatePchCpp = TemplateDir / "pch.cpp";
	std::filesystem::path OutputPchCpp = In_ProjectRoot / "pch.cpp";

	if(std::filesystem::exists(TemplatePchCpp))
    {
        if(!Util::GenerateFromTemplate(TemplatePchCpp, OutputPchCpp, replacements))
            DebugManager::GetInstance().DebugLogWarning("Failed to generate pch.cpp");
    }

	// packages.configを生成
	std::filesystem::path TemplatePackages = TemplateDir / "packages.config";
	std::filesystem::path OutputPackages = In_ProjectRoot / "packages.config";

	if(std::filesystem::exists(TemplatePackages))
    {
        if(!Util::GenerateFromTemplate(TemplatePackages, OutputPackages, replacements))
            DebugManager::GetInstance().DebugLogWarning("Failed to generate packages.config");
    }

	// NuGetで依存関係を復元
    std::filesystem::path NuGetExe = EnginePath / "Engine" / "ThirdParty" / "NuGet" / "nuget.exe";
    std::filesystem::path SlnPath = EnginePath / OutputSln;

    std::wstring msg;
    if(!NuGet::Restore(NuGetExe, SlnPath, &msg))
    {
        DebugManager::GetInstance().DebugLogError("NuGet restore failed: " + Util::WideToUTF8(msg));
        return false;
    }

	// vcxprojにNuGetのインポートを追加
    if(!PatchVcxprojWithNuGetImports(In_ProjectRoot, OutputVcxproj))
    {
        DebugManager::GetInstance().DebugLogWarning("Failed to patch vcxproj with NuGet imports");
    }

    DebugManager::GetInstance().DebugLog("Project files generated");
    return true;
}

std::string GameProjectManager::GenerateGuid()
{
    GUID guid;
    HRESULT hr = CoCreateGuid(&guid);

	if(FAILED(hr))
    {
        return "";
    }

    char buffer[64];
    sprintf_s(buffer, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    return std::string(buffer);
}

std::filesystem::path GameProjectManager::GenerateBuildRootPath() const
{
    const auto *info = GetCurrentProject();
    if(!info)
        return {};

    const std::string cfg = GetEditorConfigName(); // Debug/Release

	return info->RootPath / "Binaries" / cfg / "Builds";
}

void GameProjectManager::OpenProjectInProjectWindow(_In_ std::string_view In_ProjectName, _In_ const std::filesystem::path &In_Location)
{
	auto ProjectWindow = DebugManager::GetInstance().GetDebugWindow("View", "Project");
	auto Item = ProjectWindow->GetItemPtr("ProjectWindow");
    ItemProjectWindow *Project = nullptr;
	if(Item)
		Project = dynamic_cast<ItemProjectWindow *>(Item);
    if(!Project)
        return;

	std::string Location = In_Location.string();
	Location += "\\Assets";
	Project->SetProjectName(In_ProjectName);
	Project->SetRootPath(Location);
}

bool GameProjectManager::OpenProject(_In_ const std::filesystem::path &In_ProjectPath)
{
    if(In_ProjectPath.empty())
    {
        DebugManager::GetInstance().DebugLogError("OpenProject: path is empty");
        return false;
    }

    std::error_code ec;
    std::filesystem::path abs = std::filesystem::weakly_canonical(In_ProjectPath, ec);
    if(ec)
        abs = std::filesystem::absolute(In_ProjectPath, ec);

    if(!std::filesystem::exists(abs))
    {
        DebugManager::GetInstance().DebugLogError("OpenProject: path not found: {}", abs.string());
        return false;
    }

    if(abs.extension() != ".sln")
    {
        DebugManager::GetInstance().DebugLogError("OpenProject: only .sln is supported: {}", abs.string());
        return false;
    }

    const auto projectRoot = abs.parent_path();
    const std::string projectName = abs.stem().string();

    GameProjectInfo info;
    info.Name = projectName;
    info.Version = "1.0.0";
    info.RootPath = projectRoot;
    info.SolutionPath = abs;
    info.ProjectPath = projectRoot / (projectName + ".vcxproj");
    info.ScriptsPath = projectRoot / "Assets" / "Scripts";
    info.AssetsPath = projectRoot / "Assets";

    // すでに開いてるなら閉じる
    if(m_CurrentProject.has_value())
        CloseProject();

    m_CurrentProject = info;

    DebugManager::GetInstance().DebugLog("========================================");
    DebugManager::GetInstance().DebugLog("Project opened: {}", info.Name);
    DebugManager::GetInstance().DebugLog("Location: {}", info.RootPath.string());
    DebugManager::GetInstance().DebugLog("========================================");

    OpenProjectInProjectWindow(info.Name, info.RootPath);

	// ProjectWindow切り替えが完了してからDLLをロードする
    if(!LoadProjectDll())
    {
        DebugManager::GetInstance().DebugLogWarning("Project opened, but DLL load failed. Build the project first.");
    }

	// DLLのロードに成功したら監視を開始する
    StartWatching();

    SaveProjectInfo();
    return true;
}

void GameProjectManager::CloseProject()
{
	// ビルド監視を停止
    StopWatching();

    auto &registry = ComponentRegistry::GetInstance();
    registry.Clear();
    RegisterAllEngineComponents();

    UnloadProjectDll();
    m_CurrentProject.reset();
}

const GameProjectInfo *GameProjectManager::GetCurrentProject() const
{
    if(!m_CurrentProject.has_value())
        return nullptr;

    return &m_CurrentProject.value();
}

bool GameProjectManager::LoadProjectDll()
{
    if(m_hProjectDll)
        return true;

    const auto DllPath = GetProjectDllPathForEditorConfig();
    if(DllPath.empty() || !std::filesystem::exists(DllPath))
    {
        DebugManager::GetInstance().DebugLogWarning("Project DLL not found: {}", DllPath.string());
        return false;
    }

    // 直ロード
    m_hProjectDll = LoadLibraryW(DllPath.wstring().c_str());
    if(!m_hProjectDll)
    {
        DebugManager::GetInstance().DebugLogError("LoadLibrary failed: {}", DllPath.string());
        return false;
    }

	// エンジンAPIをDLLにセット
	if(!SetEngineAPIForDll(m_hProjectDll))
	{
		return false;
	}

    m_LoadedDllPath = DllPath;

    DebugManager::GetInstance().DebugLog("Loaded game DLL: {}", DllPath.string());

    if(!RegisterComponentsFromLoadedDll())
        return false;

    return true;
}

void GameProjectManager::UnloadProjectDll()
{
    if(!m_hProjectDll)
        return;

    // TODO: ゲーム由来コンポーネントが残っていないことを保証してから解放する
    DebugManager::GetInstance().DebugLogWarning(
        "UnloadProjectDll: FreeLibrary is unsafe if game components are still alive. Proceeding anyway (Phase5-1).");

    FreeLibrary(m_hProjectDll);
    m_hProjectDll = nullptr;
    m_LoadedDllPath.clear();
}

bool GameProjectManager::ReloadProjectDll()
{
    auto &registry = ComponentRegistry::GetInstance();

    // DLLを解放する前にゲーム登録を捨て、エンジン登録だけ復元
    registry.Clear();
    RegisterAllEngineComponents();

    UnloadProjectDll();
    return LoadProjectDll();
}

std::filesystem::path GameProjectManager::GetLoadedBuildFolder() const
{
    if(m_LoadedDllPath.empty())
        return {};

    // ...\Builds\<id>\TestGame.dll の親がビルドフォルダ
    return m_LoadedDllPath.parent_path();
}

GameProjectManager::GameProjectManager()
	: m_CurrentProject(std::nullopt), m_hProjectDll(nullptr)
	, m_ReloadingWindow(nullptr), m_IsWatching(false), m_IsReloading(false)
    , m_MainViewport(nullptr)
	, m_hStopEvent(nullptr)
{
    // 停止イベント
    m_hStopEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

    ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
    m_Overlapped.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

    LoadProjectInfo();
	SyncPackagesConfigToTemplate(std::filesystem::current_path());

    // Dllリロード時に表示するUIを作成
    CreateReloadingUI();

    // プロジェクトが開かれている場合Dllロード
    if(LoadProjectDll())
    {
        // ビルド監視を開始
		StartWatching();
    }
}

GameProjectManager::~GameProjectManager()
{
	auto &Registry = ComponentRegistry::GetInstance();
    Registry.Clear();

    // 監視を停止
    StopWatching();

    // イベントハンドルを閉じる
    if(m_Overlapped.hEvent != nullptr)
    {
        CloseHandle(m_Overlapped.hEvent);
        m_Overlapped.hEvent = nullptr;
    }

    if(m_hStopEvent != nullptr)
    {
        CloseHandle(m_hStopEvent);
        m_hStopEvent = nullptr;
    }

    UnloadProjectDll();
    SaveProjectInfo();
}

void GameProjectManager::CreateReloadingUI()
{
    auto &debug = DebugManager::GetInstance();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    m_ReloadingWindow = debug.CreateDebugModal("File", "DllReloading", false, flags);
    m_ReloadingWindow->SetShowCloseButton(false);
    m_ReloadingWindow->SetShowDefaultButtons(false);
    m_ReloadingWindow->CreateItem<ItemPushFontSize>(30.0f);
    auto ReloadingText = m_ReloadingWindow->CreateItem<ItemText>("Reloading Project Dll...");
    ReloadingText->SetTextPosCenterX();
    m_ReloadingWindow->CreateItem<ItemPopFontSize>();
	DirectX::XMFLOAT4 color = { 0.0f, 120.0f / 255.0f, 215.0f / 255.0f, 1.0f };
	m_ReloadingWindow->CreateItem<ItemLoadingIcon>("LoadingIcon", 12.0f, 3.0f,4.0f, color);

	m_MainViewport = ImGui::GetMainViewport();

    m_ReloadingWindow->SetOpenCallback([this]()
        {
            // ダイアログのサイズと位置
		    ImVec2 center = m_MainViewport->GetCenter();
		    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		    ImGui::SetNextWindowSize(ImVec2(300, 135), ImGuiCond_Appearing);
			ImGui::SetNextWindowViewport(m_MainViewport->ID);
            Input::SetInputEnabled(false);

            m_IsReloading = true;
            m_ReloadingThread = std::thread([this]()
                {
                    std::chrono::milliseconds delay(0);
                    bool result = WaitForLatestBuildOutput(delay);

                    if(result)
                    {
                        std::lock_guard<std::mutex> lock(m_ReloadingUiMutex);
                        ReloadProjectDll();
                    }
                    else
                    {
                        DebugManager::GetInstance().DebugLogError("Failed to find latest build output within timeout. Please check if the project is being built correctly.");
                    }
                    m_ReloadingWindow->Close();
                });
        });
    m_ReloadingWindow->SetCloseCallback([this]()
        {
		    m_IsReloading = false;
            Input::SetInputEnabled(true);

            if(m_ReloadingThread.joinable())
				m_ReloadingThread.join();
        });
}

void GameProjectManager::SaveProjectInfo()
{
    // 開いているプロジェクトの情報を保存するフォルダを検索
    std::filesystem::path SaveDir = std::filesystem::current_path() / "Engine" / "Assets" / "Data" / "Project";

	// フォルダが存在しない場合は作成
	if(!std::filesystem::exists(SaveDir))
    {
		std::filesystem::create_directories(SaveDir);
    }

    // フォルダが存在する場合は、プロジェクト情報を保存
    std::filesystem::path SavePath = SaveDir / "CurrentProject.cpon";
    if(m_CurrentProject.has_value())
    {
        // プロジェクト情報をCPON形式で保存
        cpon info;
        auto obj = info.CreateObject("ProjectInfo");
        auto block = obj->CreateDataBlock();
        block->SetValue("Name", m_CurrentProject->Name);
        block->SetValue("Version", m_CurrentProject->Version);
        block->SetValue("RootPath", m_CurrentProject->RootPath.string());
        block->SetValue("SolutionPath", m_CurrentProject->SolutionPath.string());
        block->SetValue("ProjectPath", m_CurrentProject->ProjectPath.string());
        block->SetValue("ScriptsPath", m_CurrentProject->ScriptsPath.string());
        block->SetValue("AssetsPath", m_CurrentProject->AssetsPath.string());

        info.WriteToFile(SavePath.string());
    }
    else
    {
        // プロジェクトが開いていない場合は、保存ファイルを削除
        std::filesystem::remove(SavePath);
    }
}

void GameProjectManager::LoadProjectInfo()
{
	// プロジェクト情報が保存されているフォルダを検索
	std::filesystem::path SaveDir = std::filesystem::current_path() / "Engine" / "Assets" / "Data" / "Project" / "CurrentProject.cpon";

    // ファイルが存在する時のみロードする
    if(std::filesystem::exists(SaveDir))
    {
        // プロジェクト情報をCPON形式で読み取る
        cpon info;
        if(info.LoadFromFile(SaveDir.string()))
        {
			// 読み込み成功であればプロジェクト情報を設定
            auto obj = info["ProjectInfo"];
            m_CurrentProject = GameProjectInfo{
				obj[1]->GetValue<std::string>("Name"),
				obj[1]->GetValue<std::string>("Version"),
				obj[1]->GetValue<std::string>("RootPath"),
				obj[1]->GetValue<std::string>("SolutionPath"),
				obj[1]->GetValue<std::string>("ProjectPath"),
				obj[1]->GetValue<std::string>("ScriptsPath"),
				obj[1]->GetValue<std::string>("AssetsPath")
			};
        }
    }
}

bool GameProjectManager::SyncPackagesConfigToTemplate(_In_ const std::filesystem::path &In_EngineRoot)
{
    const auto src = In_EngineRoot / "packages.config";
    const auto dst = In_EngineRoot / "Engine" / "Templates" / "ProjectTemplate" / "packages.config";

    if(!std::filesystem::exists(src))
        return false;

    // テンプレフォルダが無いなら作る
    std::error_code ec;
    std::filesystem::create_directories(dst.parent_path(), ec);

    // 既に同じ内容ならコピーしない(更新日時で雑に判定)
    if(std::filesystem::exists(dst))
    {
        auto srcTime = std::filesystem::last_write_time(src, ec);
        auto dstTime = std::filesystem::last_write_time(dst, ec);
        if(!ec && dstTime >= srcTime)
            return false;
    }

    std::filesystem::copy_file(
        src,
        dst,
        std::filesystem::copy_options::overwrite_existing,
        ec);

    return !ec;
}

bool GameProjectManager::PatchVcxprojWithNuGetImports(_In_ const std::filesystem::path &In_ProjectRoot, _In_ const std::filesystem::path &In_VcxprojPath)
{
    const auto packagesDir = In_ProjectRoot / "packages";
    auto props = FindNuGetBuildFiles(packagesDir, L".props");
    auto targets = FindNuGetBuildFiles(packagesDir, L".targets");

    if(props.empty() && targets.empty())
    {
        DebugManager::GetInstance().DebugLogWarning("NuGet build files not found under: {}", packagesDir.string());
        return true;
    }

    std::ifstream in(In_VcxprojPath);
    if(!in.is_open())
    {
        DebugManager::GetInstance().DebugLogError("Failed to open vcxproj for patch: {}", In_VcxprojPath.string());
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    const std::string markerPropsBegin = "  <!-- NuGet auto-imports (props) BEGIN -->\n";
    const std::string markerPropsEnd = "  <!-- NuGet auto-imports (props) END -->\n";
    const std::string markerTargetsBegin = "  <!-- NuGet auto-imports (targets) BEGIN -->\n";
    const std::string markerTargetsEnd = "  <!-- NuGet auto-imports (targets) END -->\n";

    auto buildPropsBlock = [&]()
        {
            std::string block;
            block += markerPropsBegin;
            for(const auto &p : props)
                block += MakeImportLineForVcxproj(In_ProjectRoot, p);
            block += markerPropsEnd;
            return block;
        };

    auto buildTargetsBlock = [&]()
        {
            std::string block;
            block += markerTargetsBegin;
            for(const auto &t : targets)
                block += MakeImportLineForVcxproj(In_ProjectRoot, t);
            block += markerTargetsEnd;
            return block;
        };

    auto replaceOrInsertBlock = [&](const std::string &begin, const std::string &end,
        const std::string &newBlock,
        const std::string &insertAfterToken) -> bool
        {
            size_t b = content.find(begin);
            size_t e = (b != std::string::npos) ? content.find(end, b) : std::string::npos;
            if(b != std::string::npos && e != std::string::npos)
            {
                e += end.size();
                content.replace(b, e - b, newBlock);
                return true;
            }

            // 挿入（insertAfterToken の直後）
            size_t ip = content.find(insertAfterToken);
            if(ip == std::string::npos)
                return false;

            // token の行末まで進めて、その次に挿入
            size_t lineEnd = content.find('\n', ip);
            if(lineEnd == std::string::npos)
                lineEnd = ip + insertAfterToken.size();
            else
                lineEnd += 1;

            content.insert(lineEnd, newBlock);
            return true;
        };

    // propsをCpp.props の直後へ
    if(!props.empty())
    {
        const std::string afterCppProps = "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"";
        if(!replaceOrInsertBlock(markerPropsBegin, markerPropsEnd, buildPropsBlock(), afterCppProps))
        {
            DebugManager::GetInstance().DebugLogError("Failed to insert NuGet props imports into vcxproj: {}", In_VcxprojPath.string());
            return false;
        }
    }
    else
    {
        // props が無い場合、既存 props ブロックがあれば消す（任意）
        // 今回は触らない
    }

    // targetsをCpp.targets の直後へ
    if(!targets.empty())
    {
        const std::string afterCppTargets = "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"";
        if(!replaceOrInsertBlock(markerTargetsBegin, markerTargetsEnd, buildTargetsBlock(), afterCppTargets))
        {
            DebugManager::GetInstance().DebugLogError("Failed to insert NuGet targets imports into vcxproj: {}", In_VcxprojPath.string());
            return false;
        }
    }

    std::ofstream out(In_VcxprojPath, std::ios::trunc);
    if(!out.is_open())
    {
        DebugManager::GetInstance().DebugLogError("Failed to write patched vcxproj: {}", In_VcxprojPath.string());
        return false;
    }
    out << content;
    out.close();

    DebugManager::GetInstance().DebugLog("Patched vcxproj with NuGet imports (props/targets): {}", In_VcxprojPath.string());
    return true;
}

std::vector<std::filesystem::path> GameProjectManager::FindNuGetBuildFiles(_In_ const std::filesystem::path &In_PackagesDir, _In_ const std::wstring &In_Ext) // L".props" / L".targets"
{
    std::vector<std::filesystem::path> results;
    std::error_code ec;

    if(!std::filesystem::exists(In_PackagesDir, ec) || !std::filesystem::is_directory(In_PackagesDir, ec))
        return results;

    for(const auto &PkgDir : std::filesystem::directory_iterator(In_PackagesDir, ec))
    {
        if(ec) break;
        if(!PkgDir.is_directory(ec)) continue;

        auto NativeDir = PkgDir.path() / "build" / "native";
        if(!std::filesystem::exists(NativeDir, ec) || !std::filesystem::is_directory(NativeDir, ec))
            continue;

        for(const auto &f : std::filesystem::directory_iterator(NativeDir, ec))
        {
            if(ec) break;
            if(!f.is_regular_file(ec)) continue;

            if(f.path().extension().wstring() == In_Ext)
                results.push_back(f.path());
        }
    }

    std::sort(results.begin(), results.end());
    return results;
}

std::string GameProjectManager::MakeImportLineForVcxproj(_In_ const std::filesystem::path &In_ProjectRoot, _In_ const std::filesystem::path &In_FilePath)
{
    // vcxprojから見た相対パス(例:packages\assimp-v143.5.4.3\build\native\assimp-v143.targets)
    std::error_code ec;
    auto rel = std::filesystem::relative(In_FilePath, In_ProjectRoot, ec);
    if(ec)
        rel = In_FilePath.filename();

    std::string relStr = rel.generic_string();      // '/'区切り
    std::replace(relStr.begin(), relStr.end(), '/', '\\'); // vcxproj用に'\'

    // Exists条件も同じ相対パス
    return "  <Import Project=\"" + relStr + "\" Condition=\"Exists('" + relStr + "')\" />\n";
}

bool GameProjectManager::RegisterComponentsFromLoadedDll() const
{
    if(!m_hProjectDll)
        return false;

    using RegisterAllGameComponentsFn = void(*)(void(*)(const ComponentInfo &));
    auto registerGame = reinterpret_cast<RegisterAllGameComponentsFn>(
        GetProcAddress(m_hProjectDll, "RegisterAllGameComponents")
        );

    if(!registerGame)
    {
        DebugManager::GetInstance().DebugLogError(
            "RegisterComponentsFromLoadedDll: GetProcAddress failed: RegisterAllGameComponents");
        return false;
    }

    auto &registry = ComponentRegistry::GetInstance();
    registry.Clear();
    RegisterAllEngineComponents();
    registerGame(&RegisterIntoEngineRegistry);

    DebugManager::GetInstance().DebugLog(
        "Components refreshed (engine + game). Total: {}", registry.GetComponentCount());
    return true;
}

bool GameProjectManager::SetEngineAPIForDll(_In_ HMODULE In_Dll) const
{
	using SetEngineAPIFn = void(*)(const ForgeXEngineAPI_POD *);

	auto setApi = reinterpret_cast<SetEngineAPIFn>(
		GetProcAddress(In_Dll, "SetEngineAPI"));

	if(setApi)
	{
		static ForgeXEngineAPI_POD api{};
		api.version = 1;
		api.LogV = &Engine_LogV;

		setApi(&api);
		return true;
	}
	else
	{
		DebugManager::GetInstance().DebugLogWarning("Game DLL does not export SetEngineAPI (logging API not injected).");
	}
	return false;
}

std::filesystem::path GameProjectManager::GetProjectDllPathForEditorConfig() const
{
    std::filesystem::path BuildRoot = GenerateBuildRootPath();

    if(BuildRoot.empty())
        return {};

    // 古いビルドを掃除（最新10個保持）
    PruneOldBuildFolders(BuildRoot, 10);

    // 最新ビルドのDLLを探す
    return FindLatestBuiltDllPath(BuildRoot);
}

std::filesystem::path GameProjectManager::FindLatestBuiltDllPath(_In_ std::filesystem::path In_BuildRootPath) const
{
    const auto *info = GetCurrentProject();
    if(!info)
        return {};

    const auto BuildsRoot = In_BuildRootPath;
    std::error_code ec;

    if(!std::filesystem::exists(BuildsRoot, ec) || !std::filesystem::is_directory(BuildsRoot, ec))
        return {};

    const std::string DllName = info->Name + ".dll";

    std::filesystem::path BestDll;
    auto BestTime = std::filesystem::file_time_type::min();

    for(const auto &DirEnt : std::filesystem::directory_iterator(BuildsRoot, ec))
    {
        if(ec) break;
        if(!DirEnt.is_directory(ec))
            continue;

        const auto folder = DirEnt.path();
        if(!IsBuildOutputUsable(folder, info->Name))
            continue;

        const auto DllPath = folder / (info->Name + ".dll");

        // DLLの更新日時で最新を決める(フォルダ名ではなく確実性重視)
        const auto t = SafeLastWriteTime(DllPath);
        if(t > BestTime)
        {
            BestTime = t;
            BestDll = DllPath;
        }
    }

    return BestDll;
}

void GameProjectManager::PruneOldBuildFolders(_In_ std::filesystem::path In_BuildRootPath, _In_ size_t In_KeepCount) const
{
    const std::filesystem::path BuildsRoot = In_BuildRootPath;
    std::error_code ec;

    if(!std::filesystem::exists(BuildsRoot, ec) || !std::filesystem::is_directory(BuildsRoot, ec))
        return;

    const auto LoadedFolder = GetLoadedBuildFolder(); // 空なら除外なし

    struct FolderEntry
    {
        std::filesystem::path path;
        std::filesystem::file_time_type time;
        bool IsLoaded = false;
    };

    std::vector<FolderEntry> folders;

    for(const auto &dirEnt : std::filesystem::directory_iterator(BuildsRoot, ec))
    {
        if(ec)
            break;
        if(!dirEnt.is_directory(ec))
            continue;

        // フォルダの更新日時で新旧判断
        FolderEntry fe;
        fe.path = dirEnt.path();
        fe.time = SafeLastWriteTime(dirEnt.path());
        fe.IsLoaded = (!LoadedFolder.empty() && std::filesystem::equivalent(fe.path, LoadedFolder, ec) && !ec);
        folders.push_back(std::move(fe));
    }

    // 新しい順
    std::sort(folders.begin(), folders.end(),[](const FolderEntry &a, const FolderEntry &b)
        {
            return a.time > b.time;
        });

    // loaded は強制保持
    size_t kept = 0;
    for(const auto &f : folders)
        if(f.IsLoaded)
            kept++;

    if(folders.size() <= In_KeepCount)
        return;

    for(const auto &folder : folders)
    {
        if(folder.IsLoaded)
            continue;

        if(kept < In_KeepCount)
        {
            kept++;
            continue;
        }

        ec.clear();
        std::filesystem::remove_all(folder.path, ec);

        // 削除失敗は通常あり得るのでWarningじゃなくDebugLogに落とす
        if(ec)
        {
            DebugManager::GetInstance().DebugLog(
                "PruneOldBuildFolders: could not remove: {} ({})",
                folder.path.string(), ec.message());
        }
        else
        {
            DebugManager::GetInstance().DebugLog(
                "PruneOldBuildFolders: removed: {}", folder.path.string());
        }
    }
}

bool GameProjectManager::IsBuildOutputUsable(_In_ const std::filesystem::path &In_BuildFolder, _In_ std::string_view In_ProjectName) const
{
    std::error_code ec;

    const auto DllPath = In_BuildFolder / (std::string(In_ProjectName) + ".dll");

    if(!std::filesystem::exists(DllPath, ec))
        return false;

    // 書き込み中で排他などになってないか
    if(!CanOpenForReadShared(DllPath))
        return false;

    // Debug時はPDBも必要
#ifdef _DEBUG
    const auto PdbPath = In_BuildFolder / (std::string(In_ProjectName) + ".pdb");
    if(!std::filesystem::exists(PdbPath, ec))
        return false;

    if(!CanOpenForReadShared(PdbPath))
        return false;
#endif

    return true;
}

bool GameProjectManager::WaitForLatestBuildOutput(_In_ std::chrono::milliseconds In_TimeoutMs) const
{
    std::filesystem::path BuildRoot = GenerateBuildRootPath();

    const auto *info = GetCurrentProject();
    if(!info)
        return {};

    std::error_code ec;

    if(!std::filesystem::exists(BuildRoot, ec) || !std::filesystem::is_directory(BuildRoot, ec))
        return {};

    const std::string DllName = info->Name + ".dll";

    std::filesystem::path BestDll;
    auto BestTime = std::filesystem::file_time_type::min();

    std::filesystem::path LatestFolder;

    for(const auto &DirEnt : std::filesystem::directory_iterator(BuildRoot, ec))
    {
        if(ec) break;
        if(!DirEnt.is_directory(ec))
            continue;

        const auto folder = DirEnt.path();

		// フォルダの更新日時で新旧判断
        const auto t = SafeLastWriteTime(folder);
        if(t > BestTime)
        {
            BestTime = t;
            LatestFolder = folder;
        }
    }

	// 最新フォルダが見つからないなら失敗
    if(LatestFolder.empty())
        return false;

	// 時間計測開始
	bool IsInfiniteWait = (In_TimeoutMs == std::chrono::milliseconds::zero());
	auto StartTime = std::chrono::steady_clock::now();
	auto EndTime = StartTime + In_TimeoutMs;

	// 最新フォルダにDLLができるのを待つ
    for(;;)
    {
		// タイムアウトチェック
        if(EndTime < std::chrono::steady_clock::now() && !IsInfiniteWait)
            return false;

        if(IsBuildOutputUsable(LatestFolder, info->Name))
            break;

		// スリープ処理
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return true;
}

void GameProjectManager::WatchProjectBuild()
{
    const DWORD BufferSize = 4096;
    BYTE buffer[BufferSize];

    while(m_IsWatching)
    {
        std::filesystem::path WatchPath = GenerateBuildRootPath();

        // パスが変わったらすぐに新しい監視を開始
        if(m_hDirectory != INVALID_HANDLE_VALUE && WatchPath != GenerateBuildRootPath())
        {
            CancelIo(m_hDirectory);
            CloseHandle(m_hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            continue;  // 新しいパスで監視を開始
        }

        HANDLE hDirectory = INVALID_HANDLE_VALUE;

        // ディレクトリハンドルを開く
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            // ディレクトリハンドルを開く(非同期モード)
            hDirectory = CreateFileW(
                WatchPath.wstring().c_str(),
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,  // 非同期フラグ
                NULL
            );

			// ハンドルのオープンに失敗した場合はリトライ
            if(hDirectory == INVALID_HANDLE_VALUE)
            {
				auto &debug = DebugManager::GetInstance();
				debug.DebugLogError("Failed to open build output directory for watching: {} (Error {})", WatchPath.string(), GetLastError());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            m_hDirectory = hDirectory;
        }

        // OVERLAPPEDをリセット
        ResetEvent(m_Overlapped.hEvent);

        DWORD BytesReturned = 0;

        // 非同期で変更を監視
        BOOL result = ReadDirectoryChangesW(
            hDirectory,
            buffer,
            BufferSize,
			FALSE, // サブディレクトリは監視しない
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &BytesReturned,
            &m_Overlapped,  // OVERLAPPEDを渡す
            NULL
        );

        if(!result && GetLastError() != ERROR_IO_PENDING)
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            DebugManager::GetInstance().DebugLogError("ReadDirectoryChangesW failed: {}", GetLastError());
            CloseHandle(hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        // イベントを待つ(停止イベントまたは変更イベント)
        HANDLE WaitHandles[2] = { m_hStopEvent, m_Overlapped.hEvent };
        DWORD WaitResult = WaitForMultipleObjects(2, WaitHandles, FALSE, INFINITE);

        if(WaitResult == WAIT_OBJECT_0)
        {
            // 停止イベントがシグナルされた->終了
            CancelIoEx(hDirectory, &m_Overlapped);
            // I/Oのキャンセルを待つ
            DWORD BytesTransferred = 0;
            GetOverlappedResult(hDirectory, &m_Overlapped, &BytesTransferred, TRUE);

            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            CloseHandle(hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            break;
        }
        else if(WaitResult == WAIT_OBJECT_0 + 1)
        {
            // 変更イベントがシグナルされた->変更を処理
            DWORD BytesTransferred = 0;
            if(GetOverlappedResult(hDirectory, &m_Overlapped, &BytesTransferred, FALSE))
            {
                if(BytesTransferred > 0)
                {
                    FILE_NOTIFY_INFORMATION *info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buffer);

                    for(;;)
                    {
                        bool IsBreak = false;
                        std::wstring filename(info->FileName, info->FileNameLength / sizeof(WCHAR));

                        switch(info->Action)
                        {
							// フォルダが追加された場合は、ビルド出力の新しいフォルダができた可能性があるのでリロードフラグを立てる
                        case FILE_ACTION_ADDED:
                        {
                            if(!m_IsReloading && !m_ReloadingThread.joinable())
                            {
                                std::lock_guard<std::mutex> lock(m_ReloadingUiMutex);
                                m_ReloadingWindow->Open();
                                IsBreak = true;
                            }
                            break;
                        }
                        default: break;
                        }

                        if(info->NextEntryOffset == 0 || IsBreak)
                            break;
                        info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(
                            reinterpret_cast<BYTE *>(info) + info->NextEntryOffset
                            );

                    }
                }
            }
        }

        // ハンドルを閉じる
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            CloseHandle(m_hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
        }

        // パスが変更されたかチェック
        if(WatchPath != GenerateBuildRootPath())
            continue;

        // 監視継続フラグをチェック
        if(!m_IsWatching)
            break;
    }
}

void GameProjectManager::StartWatching()
{
    if(m_IsWatching)
        return;

    m_IsWatching = true;
    m_WatcherThread = std::thread(&GameProjectManager::WatchProjectBuild, this);
}

void GameProjectManager::StopWatching()
{
    if(m_ReloadingThread.joinable())
    {
        m_ReloadingThread.join();
    }

    if(!m_IsWatching)
        return;

    m_IsWatching = false;

    // 監視スレッドに停止を通知
    if(m_hStopEvent != nullptr)
    {
        SetEvent(m_hStopEvent);
    }

    // ハンドルをキャンセル
    {
        std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
        if(m_hDirectory != INVALID_HANDLE_VALUE)
        {
            CancelIoEx(m_hDirectory, &m_Overlapped);
        }
    }

    // スレッドの終了を待つ
    if(m_WatcherThread.joinable())
    {
        m_WatcherThread.join();
    }
}

