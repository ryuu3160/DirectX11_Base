// Engine/Templates/ProjectTemplate/DllMain.cpp

#include <Windows.h>

// DLL エントリポイント
BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // DLL がロードされたとき
        break;
        
    case DLL_THREAD_ATTACH:
        // 新しいスレッドが作成されたとき
        break;
        
    case DLL_THREAD_DETACH:
        // スレッドが終了したとき
        break;
        
    case DLL_PROCESS_DETACH:
        // DLL がアンロードされたとき
        break;
    }
    
    return TRUE;
}

// ゲームプロジェクト情報
extern "C" __declspec(dllexport) const char* GetGameProjectName()
{
    return "{PROJECT_NAME}";
}

extern "C" __declspec(dllexport) const char* GetGameProjectVersion()
{
    return "1.0.0";
}
