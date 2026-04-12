/*+===================================================================
	File: VisualStudioHelper.cpp
	Summary: VisualStudioでファイルを開くためのヘルパークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 06:34:51 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "VisualStudioHelper.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include "ryuu_lib/ThreadPool/ThreadPool.hpp"
#include <comdef.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool VisualStudioHelper::OpenFileInVisualStudio(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath)
{
    // 既に実行中のVisualStudioがあるか確認
    if(IsVisualStudioRunning())
    {
        if(OpenFileInRunningInstance(In_FilePath, In_SolutionPath))
        {
            return true;
        }
    }

    // インストールされているVisualStudioを検出
    auto VsVersions = DetectVisualStudioVersions();

    if(VsVersions.empty())
    {
        DebugManager::GetInstance().DebugLogWarning("No Visual Studio installation found");

		// VisualStudioが見つからなかった場合は、テキストエディタで開く
        return OpenInTextEditor(In_FilePath);
    }

	// 実行中のVisualStudioで開けなかった、又は実行中のVisualStudioが無かった場合、ソリューションパスが有効ならば
	// ソリューションを指定してVisualStudioを起動し、起動したインスタンスでファイルを開く方法を試す
	if(!In_SolutionPath.empty() && std::filesystem::exists(In_SolutionPath))
	{
		if(OpenFileInLaunchedInstance(In_FilePath, In_SolutionPath))
			return true;
	}

	// VisualStudioで開けなかった場合は、テキストエディタで開く
	DWORD error = GetLastError();
	DebugManager::GetInstance().DebugLogError("Failed to open Visual Studio (Error: {})", error);

	// フォールバック: テキストエディタで開く
	return OpenInTextEditor(In_FilePath);
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

    // 見つかった場合は、バージョンが大きい順にソート
    if(!versions.empty())
    {
        std::sort(versions.begin(), versions.end(),
            [](const VSInfo &a, const VSInfo &b)
            {
                return a.Priority > b.Priority;
			});
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
            std::wstring ProcessName = pe32.szExeFile;

            // devenv.exe を探す
            if(_wcsicmp(ProcessName.c_str(), L"devenv.exe") == 0)
            {
                found = true;
                break;
            }
        } while(Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return found;
}

bool VisualStudioHelper::OpenFileInRunningInstance(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_LineNumber)
{
    bool success = false;

	// まずはDTEを使って開く方法を試す
	success = OpenFileInRunningInstanceWithDTE(In_FilePath, In_SolutionPath, In_LineNumber);

	// DTEで開けなかった場合、ソリューションパスが有効ならば、
	// ソリューションを指定してVisualStudioを起動し、起動したインスタンスでファイルを開く方法を試す
	if(!success && !In_SolutionPath.empty() && std::filesystem::exists(In_SolutionPath))
	{
		success = OpenFileInLaunchedInstance(In_FilePath, In_SolutionPath, In_LineNumber);
	}

    // devenv.com を使う
    if(!success)
    {
		success = OpenFileInRunningInstanceWithDevenv(In_FilePath, In_LineNumber);
    }

    return success;
}

bool VisualStudioHelper::OpenFileInRunningInstanceWithDTE(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_LineNumber)
{
    bool success = false;

    // DTE(DevelopmentToolsEnvironment)を使って開く
    HRESULT hr = CoInitialize(nullptr);
    if(FAILED(hr))
        return false;

    try
    {
        // 実行中のVisualStudioインスタンスを取得
        IUnknown *pUnk = nullptr;

        // ROT(RunningObjectTable)からVisualStudioを探す
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
                    static_cast<void>(CreateBindCtx(0, &pBindCtx));

                    LPOLESTR DisplayName = nullptr;
                    pMoniker->GetDisplayName(pBindCtx, nullptr, &DisplayName);

                    if(DisplayName)
                    {
                        std::wstring name(DisplayName);

                        if(name.find(L"!VisualStudio.DTE") != std::wstring::npos)
                        {
                            IUnknown *pObj = nullptr;
                            hr = pROT->GetObjectA(pMoniker, &pObj);

                            if(SUCCEEDED(hr) && pObj)
                            {
                                IDispatch *pDTE = nullptr;
                                hr = pObj->QueryInterface(IID_IDispatch, (void **)&pDTE);
                                if(SUCCEEDED(hr) && pDTE)
                                {
                                    // Solution.FullNameを取得して、In_SolutionPathと比較する
                                    DISPID DispidSolution;
                                    std::wstring SolutionStr = L"Solution";
                                    LPOLESTR NameSolution = SolutionStr.data();
                                    HRESULT hr2 = pDTE->GetIDsOfNames(IID_NULL, &NameSolution, 1, LOCALE_USER_DEFAULT, &DispidSolution);
                                    if(SUCCEEDED(hr2))
                                    {
                                        VARIANT VarSolution;
                                        VariantInit(&VarSolution);
                                        DISPPARAMS NoArgs = { nullptr, nullptr, 0, 0 };
                                        hr2 = pDTE->Invoke(DispidSolution, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarSolution, nullptr, nullptr);
                                        if(SUCCEEDED(hr2) && VarSolution.vt == VT_DISPATCH && VarSolution.pdispVal)
                                        {
                                            IDispatch *pSolution = VarSolution.pdispVal;
                                            // FullNameプロパティ
                                            DISPID DispidFullName;
                                            std::wstring FullNameStr = L"FullName";
                                            LPOLESTR NameFull = FullNameStr.data();
                                            hr2 = pSolution->GetIDsOfNames(IID_NULL, &NameFull, 1, LOCALE_USER_DEFAULT, &DispidFullName);
                                            if(SUCCEEDED(hr2))
                                            {
                                                VARIANT VarFullName;
                                                VariantInit(&VarFullName);
                                                hr2 = pSolution->Invoke(DispidFullName, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarFullName, nullptr, nullptr);
                                                if(SUCCEEDED(hr2) && VarFullName.vt == VT_BSTR)
                                                {
                                                    std::wstring SolutionFullName(VarFullName.bstrVal);
                                                    // 比較(大文字小文字無視)
                                                    if(In_SolutionPath.empty() || _wcsicmp(SolutionFullName.c_str(), In_SolutionPath.wstring().c_str()) == 0)
                                                    {
                                                        // ItemOperations.OpenFileを呼ぶ
                                                        DISPID DispidItemOps;
                                                        std::wstring ItemOpsStr = L"ItemOperations";
                                                        LPOLESTR NameItemOps = ItemOpsStr.data();
                                                        hr2 = pDTE->GetIDsOfNames(IID_NULL, &NameItemOps, 1, LOCALE_USER_DEFAULT, &DispidItemOps);
                                                        if(SUCCEEDED(hr2))
                                                        {
                                                            VARIANT VarItemOps;
                                                            VariantInit(&VarItemOps);
                                                            hr2 = pDTE->Invoke(DispidItemOps, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarItemOps, nullptr, nullptr);
                                                            if(SUCCEEDED(hr2) && VarItemOps.vt == VT_DISPATCH && VarItemOps.pdispVal)
                                                            {
                                                                IDispatch *pItemOps = VarItemOps.pdispVal;
                                                                DISPID DispidOpenFile;
                                                                std::wstring DispatchStr = L"OpenFile";
                                                                LPOLESTR NameOpenFile = DispatchStr.data();
                                                                if(SUCCEEDED(pItemOps->GetIDsOfNames(IID_NULL, &NameOpenFile, 1, LOCALE_USER_DEFAULT, &DispidOpenFile)))
                                                                {
                                                                    VARIANTARG arg;
                                                                    VariantInit(&arg);
                                                                    arg.vt = VT_BSTR;
                                                                    arg.bstrVal = SysAllocString(In_FilePath.wstring().c_str());

                                                                    DISPPARAMS params = { &arg, nullptr, 1, 0 };
                                                                    hr2 = pItemOps->Invoke(DispidOpenFile, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);

                                                                    SysFreeString(arg.bstrVal);
                                                                    if(SUCCEEDED(hr2))
                                                                    {
                                                                        success = true;
																		ActivateIDEMainWindow(pDTE);
                                                                    }
                                                                }
                                                                pItemOps->Release();
                                                            }
                                                            VariantClear(&VarItemOps);
                                                        }
                                                    }
                                                }
                                                VariantClear(&VarFullName);
                                            }
                                            pSolution->Release();
                                        }
                                        VariantClear(&VarSolution);
                                    }

                                    pDTE->Release();
                                }
                                pObj->Release();
                            }
                            if(success)
                            {
								// 開けたのでループを抜ける
								break;
                            }
                        }

                        CoTaskMemFree(DisplayName);
                    }
                    if(pBindCtx) pBindCtx->Release();
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

    return success;
}

bool VisualStudioHelper::OpenFileInLaunchedInstance(_In_ const std::filesystem::path &In_FilePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_LineNumber)
{
	ThreadPool &Pool = ThreadPool::GetInstance();

	auto VsVersions = DetectVisualStudioVersions();
	if(!VsVersions.empty())
	{
		const VSInfo &vs = VsVersions[0];
		std::wstring DevenvExe = vs.DevEnvPath; // 既にDetectで得ているdevenv.exe のパス

		// 別スレッドでVisualStudioを起動してROTをポーリングするタスクを生成
		auto handle = Pool.AddTask([DevenvExe, In_FilePath, In_SolutionPath]()
			{
				IStream *stream = nullptr;
				stream = LaunchVisualStudioForSolution(DevenvExe, In_SolutionPath, 5000, 100);

				bool opened = UseMarshaledDTEAndOpen(stream, In_FilePath);
			});

		if(handle != std::nullopt)
			return true;
		else
			return false;
	}
	return false;
}

bool VisualStudioHelper::OpenFileInRunningInstanceWithDevenv(_In_ const std::filesystem::path &In_FilePath, _In_ int In_LineNumber)
{
	bool success = false;
    // 実行中のインスタンスで開く
    auto VsVersions = DetectVisualStudioVersions();
    if(!VsVersions.empty())
    {
        const VSInfo &vs = VsVersions[0];
        std::wstring DevenvExe = vs.DevEnvPath; // 既にDetectで得ているdevenv.exe のパス
        if(std::filesystem::exists(DevenvExe))
        {
            std::wstring params;
            params = L"/edit \"" + In_FilePath.wstring() + L"\"";

            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            sei.lpVerb = L"open";
            sei.lpFile = DevenvExe.c_str();
            sei.lpParameters = params.c_str();
            sei.nShow = SW_HIDE;

            if(ShellExecuteExW(&sei))
            {
                // 必要なら多少待ってから(あるいはROTを再チェックして)Open成功を確認する
                bool tes = CloseHandle(sei.hProcess);
                success = true;
            }
        }
    }
    return success;
}

IStream *VisualStudioHelper::LaunchVisualStudioForSolution(_In_ std::wstring_view In_DevenExePath, _In_ const std::filesystem::path &In_SolutionPath, _In_ int In_TimeoutMs, _In_ int In_PoolIntervalMs)
{
    std::wstring params;
	std::wstring DevenExePath(In_DevenExePath);
    params = L"\"" + In_SolutionPath.wstring() + L"\"";

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"open";
	sei.lpFile = DevenExePath.c_str();
    sei.lpParameters = params.c_str();
    sei.nShow = SW_SHOW;

    if(ShellExecuteExW(&sei))
    {
		// まず WaitForInputIdleを試す
		if(sei.hProcess)
			DWORD WaitRet = WaitForInputIdle(sei.hProcess, 5000); // 5秒

		// ROT ポーリング
		int waited = 0;

		// COM 初期化(呼び出し側ですでにCoInitializeEx済みなら不要)
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		bool DidCoInit = SUCCEEDED(hr);

		IStream *pMarshalStream = nullptr;

		while(waited < In_TimeoutMs)
		{
			IRunningObjectTable *pROT = nullptr;
			if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
			{
				IEnumMoniker *pEnum = nullptr;
				if(SUCCEEDED(pROT->EnumRunning(&pEnum)))
				{
					IMoniker *pMoniker = nullptr;
					while(pEnum->Next(1, &pMoniker, nullptr) == S_OK)
					{
						IBindCtx *pBindCtx = nullptr;
						static_cast<void>(CreateBindCtx(0, &pBindCtx));

						LPOLESTR DisplayName = nullptr;
						pMoniker->GetDisplayName(pBindCtx, nullptr, &DisplayName);
						if(DisplayName)
						{
							std::wstring name(DisplayName);
							if(name.find(L"!VisualStudio.DTE") != std::wstring::npos)
							{
								IUnknown *pUnk = nullptr;
								if(SUCCEEDED(pROT->GetObjectA(pMoniker, &pUnk)) && pUnk)
								{
									IDispatch *pDTE = nullptr;
									if(SUCCEEDED(pUnk->QueryInterface(IID_IDispatch, (void **)&pDTE)) && pDTE)
									{
										// pDTE->Solution.FullName を読む
										DISPID DispidSolution;
										std::wstring SolutionStr = L"Solution";
										LPOLESTR NameSolution = SolutionStr.data();
										if(SUCCEEDED(pDTE->GetIDsOfNames(IID_NULL, &NameSolution, 1, LOCALE_USER_DEFAULT, &DispidSolution)))
										{
											VARIANT VarSolution; VariantInit(&VarSolution);
											DISPPARAMS NoArgs = { nullptr, nullptr, 0, 0 };
											if(SUCCEEDED(pDTE->Invoke(DispidSolution, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarSolution, nullptr, nullptr))
												&& VarSolution.vt == VT_DISPATCH && VarSolution.pdispVal)
											{
												IDispatch *pSolution = VarSolution.pdispVal;
												DISPID DispidFullName;
												std::wstring FullNameStr = L"FullName";
												LPOLESTR NameFull = FullNameStr.data();
												if(SUCCEEDED(pSolution->GetIDsOfNames(IID_NULL, &NameFull, 1, LOCALE_USER_DEFAULT, &DispidFullName)))
												{
													VARIANT VarFull;
													VariantInit(&VarFull);
													if(SUCCEEDED(pSolution->Invoke(DispidFullName, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarFull, nullptr, nullptr))
														&& VarFull.vt == VT_BSTR)
													{
														std::wstring FullNow(VarFull.bstrVal);
														if(_wcsicmp(FullNow.c_str(), In_SolutionPath.wstring().c_str()) == 0)
														{
															// ソリューションが一致したので、pDTEをマシュリングしてIStreamを作る
															hr = CoMarshalInterThreadInterfaceInStream(IID_IDispatch, pDTE, &pMarshalStream);
															
															VariantClear(&VarFull);
															pSolution->Release();
															VariantClear(&VarSolution);
															pDTE->Release();
															pUnk->Release();
															CoTaskMemFree(DisplayName);
															if(pBindCtx)
																pBindCtx->Release();
															pMoniker->Release();
															pEnum->Release();
															pROT->Release();
															if(DidCoInit)
																CoUninitialize();

															return pMarshalStream;
														}
													}
													VariantClear(&VarFull);
												}
												pSolution->Release();
											}
											VariantClear(&VarSolution);
										}
										pDTE->Release();
									}
									pUnk->Release();
								}
							}
							CoTaskMemFree(DisplayName);
						}
						if(pBindCtx) pBindCtx->Release();
						pMoniker->Release();
					}
					pEnum->Release();
				}
				pROT->Release();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(In_PoolIntervalMs));
			waited += In_PoolIntervalMs;
		}

		if(DidCoInit)
			CoUninitialize();
		return nullptr;
    }

    return nullptr;
}

bool VisualStudioHelper::UseMarshaledDTEAndOpen(_In_opt_ IStream *In_pStream, _In_ const std::filesystem::path &In_FilePath)
{
	if(!In_pStream)
		return false;

	bool success = false;
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(FAILED(hr))
		return false;

	IDispatch *pDTE = nullptr;
	hr = CoGetInterfaceAndReleaseStream(In_pStream, IID_IDispatch, (void **)&pDTE);
	// CoGetInterfaceAndReleaseStreamは成功すればIn_pStreamを解放してくれる

	if(SUCCEEDED(hr) && pDTE)
	{
		DISPPARAMS noArgs = { nullptr, nullptr, 0, 0 };
		DISPID DispidItemOps;
		std::wstring ItemOpsStr = L"ItemOperations";
		LPOLESTR NameItemOps = ItemOpsStr.data();
		HRESULT hr2 = pDTE->GetIDsOfNames(IID_NULL, &NameItemOps, 1, LOCALE_USER_DEFAULT, &DispidItemOps);
		VARIANT VarItemOps;
		VariantInit(&VarItemOps);
		hr2 = pDTE->Invoke(DispidItemOps, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &noArgs, &VarItemOps, nullptr, nullptr);
		if(SUCCEEDED(hr2) && VarItemOps.vt == VT_DISPATCH && VarItemOps.pdispVal)
		{
			IDispatch *pItemOps = VarItemOps.pdispVal;
			DISPID DispidOpenFile;
			std::wstring DispatchStr = L"OpenFile";
			LPOLESTR NameOpenFile = DispatchStr.data();
			if(SUCCEEDED(pItemOps->GetIDsOfNames(IID_NULL, &NameOpenFile, 1, LOCALE_USER_DEFAULT, &DispidOpenFile)))
			{
				VARIANTARG arg;
				VariantInit(&arg);
				arg.vt = VT_BSTR;
				arg.bstrVal = SysAllocString(In_FilePath.wstring().c_str());

				DISPPARAMS params = { &arg, nullptr, 1, 0 };
				hr2 = pItemOps->Invoke(DispidOpenFile, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);

				SysFreeString(arg.bstrVal);
				if(SUCCEEDED(hr2))
				{
					success = true;
				}
			}
			pItemOps->Release();
		}
		VariantClear(&VarItemOps);

		// 最後に Release
		pDTE->Release();
	}

	CoUninitialize();
	return success && pDTE != nullptr;
}

std::vector<VisualStudioHelper::VSInfo> VisualStudioHelper::DetectUsingVSWhere()
{
    std::vector<VSInfo> versions;

    // vswhere.exeのパス
    std::filesystem::path VswherePath =
        LR"(C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe)";

    if(!std::filesystem::exists(VswherePath))
        return versions;

    // vswhereを実行してJSONを取得
    std::wstring command = L"\"" + VswherePath.wstring() +
        LR"(" -format json -utf8 -products * -requires Microsoft.Component.MSBuild)";

    // パイプで出力を取得
    FILE *pipe = _wpopen(command.c_str(), L"r");
    if(!pipe)
        return versions;

    char buffer[4096];
    std::string result;
    while(fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    _pclose(pipe);

    // 簡易的なJSONパース
    size_t pos = 0;
    while((pos = result.find("\"installationPath\":", pos)) != std::string::npos)
    {
        // installationPath を抽出
        size_t start = result.find("\"", pos + 20);
        size_t end = result.find("\"", start + 1);
        if(start == std::string::npos || end == std::string::npos)
            break;

        std::string InstallPath = result.substr(start + 1, end - start - 1);

        // エスケープシーケンスを処理(\\を\に変換)
        size_t EscapePos = 0;
        while((EscapePos = InstallPath.find("\\\\", EscapePos)) != std::string::npos)
        {
            InstallPath.replace(EscapePos, 2, "\\");
            EscapePos += 1;
        }

        std::filesystem::path VsPath(InstallPath);
        std::filesystem::path DevenvPath = VsPath / "Common7" / "IDE" / "devenv.exe";

        if(std::filesystem::exists(DevenvPath))
        {
            VSInfo info;
            info.InstallPath = VsPath.wstring();
            info.DevEnvPath = DevenvPath.wstring();

            // バージョンを判定
            std::wstring PathStr = VsPath.wstring();
            if(PathStr.find(L"18") != std::wstring::npos) // VisualStudio2026は18
            {
                info.Version = L"2026";
                info.Priority = 100;  // 最優先
            }
            else if(PathStr.find(L"2022") != std::wstring::npos)
            {
                info.Version = L"2022";
                info.Priority = 90;
            }
            else if(PathStr.find(L"2019") != std::wstring::npos)
            {
                info.Version = L"2019";
                info.Priority = 80;
            }
            else
            {
                info.Version = L"Unknown";
                info.Priority = 60;
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
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe)", 90 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe)", 90 },
        { LR"(C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe)", 90 },
		{ LR"(C:\Program Files\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe)", 80 },
		{ LR"(C:\Program Files\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe)", 80 },
		{ LR"(C:\Program Files\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv.exe)", 80  },
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
            else if(path.find(L"2022") != std::wstring::npos)
                info.Version = L"2022";
			else if(path.find(L"2019") != std::wstring::npos)
				info.Version = L"2019";
            else
                info.Version = L"Unknown";

            versions.push_back(info);
        }
    }

    return versions;
}

bool VisualStudioHelper::OpenInTextEditor(_In_ const std::filesystem::path &In_FilePath)
{
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

void VisualStudioHelper::ActivateIDEMainWindow(_In_opt_ IDispatch *In_pDTE)
{
	if(!In_pDTE)
		return;

	// DTE.MainWindow(プロパティ)を取得
	DISPID DispidMain;
	LPOLESTR NameMain = const_cast<LPOLESTR>(L"MainWindow");
	if(FAILED(In_pDTE->GetIDsOfNames(IID_NULL, &NameMain, 1, LOCALE_USER_DEFAULT, &DispidMain)))
		return;

	VARIANT VarMain;
	VariantInit(&VarMain);
	DISPPARAMS NoArgs = { nullptr, nullptr, 0, 0 };

	if(FAILED(In_pDTE->Invoke(DispidMain, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &NoArgs, &VarMain, nullptr, nullptr)))
	{
		VariantClear(&VarMain);
		return;
	}

	if(VarMain.vt != VT_DISPATCH || !VarMain.pdispVal)
	{
		VariantClear(&VarMain);
		return;
	}

	IDispatch *pMainWindow = VarMain.pdispVal; // 参照カウントはpMainWindowにある
	// Window.Activate()を呼ぶ
	DISPID DispidActivate;
	LPOLESTR NameActivate = const_cast<LPOLESTR>(L"Activate");
	if(SUCCEEDED(pMainWindow->GetIDsOfNames(IID_NULL, &NameActivate, 1, LOCALE_USER_DEFAULT, &DispidActivate)))
	{
		DISPPARAMS params = { nullptr, nullptr, 0, 0 };
		pMainWindow->Invoke(DispidActivate, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}

	pMainWindow->Release();
	VariantClear(&VarMain);
}