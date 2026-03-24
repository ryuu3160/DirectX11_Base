/*+===================================================================
	File: NuGetRestore.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/14 Sat AM 07:22:36 初回作成
===================================================================+*/
#define WIN32_LEAN_AND_MEAN
// ==============================
//	include
// ==============================
#include "NuGetRestore.hpp"

namespace
{
    bool RunProcessCapture(const std::wstring &cmdLine,
        const std::filesystem::path &workDir,
        DWORD *outExitCode)
    {
        // 子プロセスの出力を拾いたいならパイプが必要だけど、
        // まずは exit code のみで十分。必要になったら拡張でOK。
        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};

        std::wstring mutableCmd = cmdLine; // CreateProcessが書き換える

        BOOL ok = CreateProcessW(
            nullptr,
            mutableCmd.data(),
            nullptr, nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            workDir.wstring().c_str(),
            &si,
            &pi);

        if(!ok) return false;

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 1;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        if(outExitCode) *outExitCode = exitCode;
        return true;
    }

    static std::wstring Quote(const std::filesystem::path &p)
    {
        return L"\"" + p.wstring() + L"\"";
    }
}

namespace NuGet
{
    bool Restore(_In_ const std::filesystem::path &In_NugetExe, _In_ const std::filesystem::path &In_SolutionPath, _Out_ std::wstring *Out_Message)
    {
        if(!std::filesystem::exists(In_NugetExe))
            return false;
        if(!std::filesystem::exists(In_SolutionPath))
            return false;

        const auto workDir = In_SolutionPath.parent_path();

        // -NonInteractive: 対話無し
        std::wstring cmd =
            Quote(In_NugetExe) + L" restore " + Quote(In_SolutionPath) + L" -NonInteractive";

        DWORD exitCode = 1;
        if(!RunProcessCapture(cmd, workDir, &exitCode)) return false;

        return exitCode == 0;
    }
}
