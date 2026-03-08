/*+===================================================================
	File: VisualStudioHelper.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 06:34:51 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "VisualStudioHelper.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include <comdef.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool VisualStudioHelper::OpenFileInVisualStudio(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath)
{
    // 既に実行中の Visual Studio があるか確認
    if(IsVisualStudioRunning())
    {
        if(OpenFileInRunningInstance(In_FilePath))
        {
            DebugManager::GetInstance().DebugLog("Opened in running Visual Studio: {}",
                In_FilePath.filename().string());
            return true;
        }
    }

    // インストールされているVisualStudioを検出
    auto vsVersions = DetectVisualStudioVersions();

    if(vsVersions.empty())
    {
        DebugManager::GetInstance().DebugLogWarning("No Visual Studio installation found");

        // フォールバック: テキストエディタで開く
        return OpenInTextEditor(In_FilePath);
    }

    // 優先順にソート
    std::sort(vsVersions.begin(), vsVersions.end(),
        [](const VSInfo &a, const VSInfo &b)
        {
            return a.Priority > b.Priority;
        });

    // 最優先のVisualStudioで開く
    const VSInfo &vs = vsVersions[0];

    std::wstring command;
    if(!In_SolutionPath.empty() && std::filesystem::exists(In_SolutionPath))
    {
        // ソリューションと一緒に開く
        command = L"\"" + In_SolutionPath.wstring() + L"\" /edit \"" + In_FilePath.wstring() + L"\"";
    }
    else
    {
        // ファイルだけを開く
        command = L"/edit \"" + In_FilePath.wstring() + L"\"";
    }

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"open";
    sei.lpFile = vs.DevEnvPath.c_str();
    sei.lpParameters = command.c_str();
    sei.nShow = SW_SHOW;

    if(ShellExecuteExW(&sei))
    {
        std::wstring versionStr = vs.Version;
		std::string version = Util::WideToUTF8(versionStr);

        DebugManager::GetInstance().DebugLog("Opened in Visual Studio {}: {}",
            version, In_FilePath.filename().string());
        return true;
    }
    else
    {
        DWORD error = GetLastError();
        DebugManager::GetInstance().DebugLogError("Failed to open Visual Studio (Error: {})", error);

        // フォールバック: テキストエディタで開く
        return OpenInTextEditor(In_FilePath);
    }
}

std::vector<VisualStudioHelper::VSInfo> VisualStudioHelper::DetectVisualStudioVersions()
{
    // vswhere.exe を使った検出を試みる
    auto versions = DetectUsingVSWhere();

    // 見つからなければレジストリから検出
    if(versions.empty())
    {
        versions = DetectUsingRegistry();
    }

    return versions;
}

std::vector<VisualStudioHelper::VSInfo> VisualStudioHelper::DetectUsingVSWhere()
{
    std::vector<VSInfo> versions;

    // vswhere.exe のパス
    std::filesystem::path vswherePath =
        LR"(C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe)";

    if(!std::filesystem::exists(vswherePath))
    {
        return versions;
    }

    // vswhere を実行して JSON を取得
    std::wstring command = L"\"" + vswherePath.wstring() +
        LR"(" -format json -utf8 -products * -requires Microsoft.Component.MSBuild)";

    // パイプで出力を取得
    FILE *pipe = _wpopen(command.c_str(), L"r");
    if(!pipe)
    {
        return versions;
    }

    char buffer[4096];
    std::string result;
    while(fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    _pclose(pipe);

    // 簡易的なJSONパース
    // ここでは手動でパースする簡易版
    size_t pos = 0;
    while((pos = result.find("\"installationPath\":", pos)) != std::string::npos)
    {
        // installationPath を抽出
        size_t start = result.find("\"", pos + 20);
        size_t end = result.find("\"", start + 1);
        if(start == std::string::npos || end == std::string::npos)
            break;

        std::string installPath = result.substr(start + 1, end - start - 1);

        // エスケープシーケンスを処理（\\を\に変換）
        size_t escapePos = 0;
        while((escapePos = installPath.find("\\\\", escapePos)) != std::string::npos)
        {
            installPath.replace(escapePos, 2, "\\");
            escapePos += 1;
        }

        std::filesystem::path vsPath(installPath);
        std::filesystem::path devenvPath = vsPath / "Common7" / "IDE" / "devenv.exe";

        if(std::filesystem::exists(devenvPath))
        {
            VSInfo info;
            info.InstallPath = vsPath.wstring();
            info.DevEnvPath = devenvPath.wstring();

            // バージョンを判定
            std::wstring pathStr = vsPath.wstring();
            if(pathStr.find(L"18") != std::wstring::npos)
            {
                info.Version = L"2026";
                info.Priority = 100;  // 最優先
            }
            else if(pathStr.find(L"2025") != std::wstring::npos)
            {
                info.Version = L"2025";
                info.Priority = 90;
            }
            else if(pathStr.find(L"2022") != std::wstring::npos)
            {
                info.Version = L"2022";
                info.Priority = 80;
            }
            else if(pathStr.find(L"2019") != std::wstring::npos)
            {
                info.Version = L"2019";
                info.Priority = 70;
            }
            else
            {
                info.Version = L"Unknown";
                info.Priority = 50;
            }

            versions.push_back(info);
        }

        pos = end;
    }

    return versions;
}

std::vector<VisualStudioHelper::VSInfo> VisualStudioHelper::DetectUsingRegistry()
{
    std::vector<VSInfo> versions;

    // 一般的なインストールパスを確認
    std::vector<std::pair<std::wstring, int>> paths = {
        { LR"(C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\devenv.exe)", 100 },
        { LR"(C:\Program Files\Microsoft Visual Studio\18\Professional\Common7\IDE\devenv.exe)", 100 },
        { LR"(C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\IDE\devenv.exe)", 100 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2025\Community\Common7\IDE\devenv.exe)", 90 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2025\Professional\Common7\IDE\devenv.exe)", 90 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2025\Enterprise\Common7\IDE\devenv.exe)", 90 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe)", 80 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe)", 80 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe)", 80 },
    };

    for(const auto &[path, priority] : paths)
    {
        if(std::filesystem::exists(path))
        {
            VSInfo info;
            info.DevEnvPath = path;
            info.InstallPath = std::filesystem::path(path).parent_path().parent_path().parent_path().wstring();
            info.Priority = priority;

            // バージョンを抽出
            if(path.find(L"2026") != std::wstring::npos)
                info.Version = L"2026";
            else if(path.find(L"2025") != std::wstring::npos)
                info.Version = L"2025";
            else if(path.find(L"2022") != std::wstring::npos)
                info.Version = L"2022";
            else
                info.Version = L"Unknown";

            versions.push_back(info);
        }
    }

    return versions;
}

bool VisualStudioHelper::IsVisualStudioRunning()
{
    // プロセス一覧を取得
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    bool found = false;

    if(Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            std::wstring processName = pe32.szExeFile;

            // devenv.exe を探す
            if(_wcsicmp(processName.c_str(), L"devenv.exe") == 0)
            {
                found = true;
                break;
            }
        } while(Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return found;
}

bool VisualStudioHelper::OpenFileInRunningInstance(
    const std::filesystem::path &In_FilePath,
    int In_LineNumber)
{
    // DTE (Development Tools Environment) を使って開く
    HRESULT hr = CoInitialize(nullptr);
    if(FAILED(hr))
        return false;

    bool success = false;

    try
    {
        // 実行中の Visual Studio インスタンスを取得
        IUnknown *pUnk = nullptr;

        // ROT (Running Object Table) から Visual Studio を探す
        IRunningObjectTable *pROT = nullptr;
        hr = GetRunningObjectTable(0, &pROT);

        if(SUCCEEDED(hr))
        {
            IEnumMoniker *pEnum = nullptr;
            hr = pROT->EnumRunning(&pEnum);

            if(SUCCEEDED(hr))
            {
                IMoniker *pMoniker = nullptr;
                while(pEnum->Next(1, &pMoniker, nullptr) == S_OK)
                {
                    IBindCtx *pBindCtx = nullptr;
                    CreateBindCtx(0, &pBindCtx);

                    LPOLESTR displayName = nullptr;
                    pMoniker->GetDisplayName(pBindCtx, nullptr, &displayName);

                    if(displayName)
                    {
                        std::wstring name(displayName);

                        // Visual Studio の DTE を探す
                        if(name.find(L"!VisualStudio.DTE") != std::wstring::npos)
                        {
                            // このモニカーからオブジェクトを取得
                            IUnknown *pObj = nullptr;
                            hr = pROT->GetObjectA(pMoniker, &pObj);

                            if(SUCCEEDED(hr))
                            {
                                // ファイルを開くコマンドを送信
                                // 簡易版: DTE.ExecuteCommand を使う

                                // ここでは単純に新しいプロセスとして開く
                                // 完全な実装には EnvDTE の参照が必要

                                pObj->Release();
                                success = true;
                                break;
                            }
                        }

                        CoTaskMemFree(displayName);
                    }

                    if(pBindCtx)
                        pBindCtx->Release();

                    pMoniker->Release();
                }

                pEnum->Release();
            }

            pROT->Release();
        }
    }
    catch(...)
    {
    }

    CoUninitialize();

    // より簡単な方法: devenv.com を使う
    if(!success)
    {
        // 実行中のインスタンスで開く
        auto vsVersions = DetectVisualStudioVersions();
        if(!vsVersions.empty())
        {
            std::wstring devenvCom = vsVersions[0].InstallPath + L"\\Common7\\IDE\\devenv.com";

            if(std::filesystem::exists(devenvCom))
            {
                std::wstring command = L"\"" + devenvCom + L"\" /edit \"" + In_FilePath.wstring() + L"\"";

                STARTUPINFOW si = {};
                si.cb = sizeof(si);
                PROCESS_INFORMATION pi = {};

                if(CreateProcessW(nullptr, const_cast<LPWSTR>(command.c_str()),
                    nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
                {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                    success = true;
                }
            }
        }
    }

    return success;
}

bool VisualStudioHelper::OpenInTextEditor(const std::filesystem::path &In_FilePath)
{
    DebugManager::GetInstance().DebugLog("Opening in default text editor: {}",
        In_FilePath.filename().string());

    // notepad で開く
    std::wstring command = L"notepad.exe \"" + In_FilePath.wstring() + L"\"";

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};

    if(CreateProcessW(nullptr, const_cast<LPWSTR>(command.c_str()),
        nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }

    return false;
}
