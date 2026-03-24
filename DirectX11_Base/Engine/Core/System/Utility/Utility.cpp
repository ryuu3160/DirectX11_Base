/*+===================================================================
	File: Utility.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/3 Wed AM 02:21:35 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Utility.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

namespace Util
{
	std::string WideToUTF8(_In_ const std::wstring_view In_WideStr)
	{
		if(In_WideStr.empty()) return "";

		// まず必要なバッファサイズを取得
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, In_WideStr.data(), -1, nullptr, 0, nullptr, nullptr);
		std::string utf8Str(sizeNeeded, '\0');

		// UTF-8に変換
		WideCharToMultiByte(CP_UTF8, 0, In_WideStr.data(), -1, &utf8Str[0], sizeNeeded, nullptr, nullptr);
		return utf8Str;
	}

	std::wstring UTF8ToWide(_In_ std::string_view In_UTF8Str)
	{
		// まず必要なバッファサイズを取得
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, nullptr, 0);
		std::wstring wideStr(sizeNeeded, L'\0'); // ワイド文字列を作成

		// UTF-8 → Wide文字 に変換
		MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, &wideStr[0], sizeNeeded);
		return wideStr;
	}

	std::string UTF16ToUTF8(_In_ const std::u16string_view In_UTF16Str)
	{
		std::string utf8Str;
		utf8Str.reserve(In_UTF16Str.size() * 2); // サイズをある程度予測して予約
		for(char16_t ch : In_UTF16Str)
		{
			if(ch <= 0x7F)
			{
				utf8Str.push_back(static_cast<char>(ch)); // 1バイト文字
			}
			else if(ch <= 0x7FF)
			{
				utf8Str.push_back(static_cast<char>(0xC0 | ((ch >> 6) & 0x1F))); // 2バイト文字
				utf8Str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
			}
			else
			{
				utf8Str.push_back(static_cast<char>(0xE0 | ((ch >> 12) & 0x0F))); // 3バイト文字
				utf8Str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
				utf8Str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
			}
		}
		return utf8Str;
	}

	std::u16string UTF8ToUTF16(_In_ std::string_view In_UTF8Str)
	{
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, nullptr, 0);
		std::u16string utf16Str(sizeNeeded, u'\0');

		MultiByteToWideChar(CP_UTF8, 0, In_UTF8Str.data(), -1, reinterpret_cast<wchar_t *>(&utf16Str[0]), sizeNeeded);
		return utf16Str;
	}

	std::string ShiftJISToUTF8(_In_ std::string_view In_ShiftJISStr)
	{
		// Shift-JIS → UTF-16 に変換
		int wideCharSize = MultiByteToWideChar(CP_ACP, 0, In_ShiftJISStr.data(), -1, nullptr, 0);
		std::wstring wideStr(wideCharSize, L'\0');
		MultiByteToWideChar(CP_ACP, 0, In_ShiftJISStr.data(), -1, &wideStr[0], wideCharSize);

		// UTF-16 → UTF-8 に変換
		int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string utf8Str(utf8Size, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);

		return utf8Str;
	}

	std::filesystem::path GetExecutableDir()
	{
		wchar_t buf[MAX_PATH]{};
		DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
		if(len == 0 || len == MAX_PATH)
			return std::filesystem::current_path(); // 最悪のフォールバック

		std::filesystem::path exePath(buf);
		return exePath.parent_path();
	}

	void OpenInSystemExplorer(const std::filesystem::path &In_Path)
	{
		try
		{
			// 絶対パスに変換
			std::filesystem::path absolutePath = std::filesystem::absolute(In_Path);

			// 存在確認
			if(!std::filesystem::exists(absolutePath))
			{
				OutputDebugStringA(("Path does not exist: " + absolutePath.string() + "\n").c_str());
				return;
			}

#ifdef _WIN32
			// Windows: パス区切り文字を \ に統一
			std::string pathStr = absolutePath.string();
			std::replace(pathStr.begin(), pathStr.end(), '/', '\\');

			std::string command = "explorer \"" + pathStr + "\"";
			system(command.c_str());
#elif __APPLE__
			// macOS
			std::string command = "open \"" + absolutePath.string() + "\"";
			system(command.c_str());
#elif __linux__
			// Linux
			std::string command = "xdg-open \"" + absolutePath.string() + "\"";
			system(command.c_str());
#endif
		}
		catch(const std::exception &e)
		{
			OutputDebugStringA(("Failed to open in system explorer: " + std::string(e.what()) + "\n").c_str());
		}
	}

	void ShowInSystemExplorer(const std::filesystem::path &In_Path)
	{
		try
		{
			std::filesystem::path absolutePath = std::filesystem::absolute(In_Path);

			if(!std::filesystem::exists(absolutePath))
			{
				OutputDebugStringA(("Path does not exist: " + absolutePath.string() + "\n").c_str());
				return;
			}

#ifdef _WIN32
			std::string pathStr = absolutePath.string();
			std::replace(pathStr.begin(), pathStr.end(), '/', '\\');

			std::string command = "explorer /select,\"" + pathStr + "\"";
			system(command.c_str());
#elif __APPLE__
			std::string command = "open -R \"" + absolutePath.string() + "\"";
			system(command.c_str());
#elif __linux__
			std::string parentPath = absolutePath.parent_path().string();
			std::string command = "xdg-open \"" + parentPath + "\"";
			system(command.c_str());
#endif
		}
		catch(const std::exception &e)
		{
			OutputDebugStringA(("Failed to show in system explorer: " + std::string(e.what()) + "\n").c_str());
		}
	}

	bool GenerateFromTemplate(_In_ const std::filesystem::path &In_TemplatePath, _In_ const std::filesystem::path &In_OutputPath, _In_ const std::map<std::string, std::string> &In_Replacements)
	{
		try
		{
			// テンプレートを読み込み
			std::ifstream templateFile(In_TemplatePath);
			if(!templateFile.is_open())
			{
				OutputDebugStringA(("Failed to open template file: " + In_TemplatePath.string() + "\n").c_str());
				return false;
			}

			std::stringstream buffer;
			buffer << templateFile.rdbuf();
			std::string content = buffer.str();
			templateFile.close();

			// 置換処理
			for(const auto &[key, value] : In_Replacements)
			{
				size_t pos = 0;
				while((pos = content.find(key, pos)) != std::string::npos)
				{
					content.replace(pos, key.length(), value);
					pos += value.length();
				}
			}

			// ファイルに書き込み
			std::ofstream outputFile(In_OutputPath);
			if(!outputFile.is_open())
			{
				OutputDebugStringA(("Failed to create output file: " + In_OutputPath.string() + "\n").c_str());
				return false;
			}

			outputFile << content;
			outputFile.close();

			return true;
		}
		catch(const std::exception &e)
		{
			OutputDebugStringA(("Failed to generate from template: " + std::string(e.what()) + "\n").c_str());
			return false;
		}
	}

	void BuildFilterSpecs(_In_ std::wstring_view In_Filter, _Out_ std::vector<std::wstring> &Out_OwnedStrings, _Out_ std::vector<COMDLG_FILTERSPEC> &Out_Specs)
	{
		Out_OwnedStrings.clear();
		Out_Specs.clear();

		if(In_Filter.empty())
			return;

		// 区切り: "||" で1フィルタ、"|" で名前とパターン
		size_t pos = 0;
		while(pos < In_Filter.size())
		{
			size_t next = In_Filter.find(L"||", pos);
			std::wstring_view token = (next == std::wstring_view::npos)
				? In_Filter.substr(pos)
				: In_Filter.substr(pos, next - pos);

			if(!token.empty())
			{
				size_t bar = token.find(L'|');
				if(bar != std::wstring::npos && bar > 0 && bar + 1 < token.size())
				{
					std::wstring_view name = token.substr(0, bar);
					std::wstring_view spec = token.substr(bar + 1);

					Out_OwnedStrings.emplace_back(name);
					Out_OwnedStrings.emplace_back(spec);
				}
			}

			if(next == std::wstring_view::npos)
				break;

			pos = next + 2;
		}

		for(size_t i = 0; i + 1 < Out_OwnedStrings.size(); i += 2)
		{
			COMDLG_FILTERSPEC fs{};
			fs.pszName = Out_OwnedStrings[i].c_str();
			fs.pszSpec = Out_OwnedStrings[i + 1].c_str();
			Out_Specs.push_back(fs);
		}
	}

	std::optional<std::wstring> PickFolderW(_In_ HWND In_Owner, _In_ std::wstring_view In_Title, _In_ std::wstring_view In_DefaultFolder)
	{
		IFileDialog *pfd = nullptr;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if(FAILED(hr))
			return std::nullopt;

		DWORD opts = 0;
		pfd->GetOptions(&opts);
		pfd->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);

		if(!In_Title.empty())
			pfd->SetTitle(In_Title.data());

		// 初期フォルダ設定(存在する場合のみ)
		if(!In_DefaultFolder.empty())
		{
			IShellItem *psi = nullptr;
			if(SUCCEEDED(SHCreateItemFromParsingName(In_DefaultFolder.data(), nullptr, IID_PPV_ARGS(&psi))))
			{
				pfd->SetFolder(psi);
				psi->Release();
			}
		}

		hr = pfd->Show(In_Owner);
		if(FAILED(hr))
		{
			pfd->Release();
			return std::nullopt; // キャンセル含む
		}

		IShellItem *result = nullptr;
		hr = pfd->GetResult(&result);
		if(FAILED(hr))
		{
			pfd->Release();
			return std::nullopt;
		}

		PWSTR path = nullptr;
		hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

		result->Release();
		pfd->Release();

		if(FAILED(hr) || !path)
			return std::nullopt;

		std::wstring selected(path);
		CoTaskMemFree(path);
		return selected;
	}

	std::optional<std::string> PickFolderUTF8(_In_ HWND In_Owner, _In_ std::string_view In_Title, _In_ std::string_view In_DefaultFolderUtf8)
	{
		std::wstring wtitle = In_Title.data() ? UTF8ToWide(In_Title) : L"Select Folder";
		std::wstring wdefault = UTF8ToWide(In_DefaultFolderUtf8);

		auto picked = PickFolderW(In_Owner, wtitle, wdefault);
		if(!picked)
			return std::nullopt;
		return WideToUTF8(*picked);
	}

	std::optional<std::wstring> PickFileW(_In_ HWND In_Owner, _In_ std::wstring_view In_Title, _In_ std::wstring_view In_DefaultFolder, _In_ std::wstring_view In_Filter, _In_ std::wstring_view In_DefaultExtension)
	{
		// IFileDialog を使用してファイル選択ダイアログを表示
		IFileDialog *pfd = nullptr;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if(FAILED(hr))
			return std::nullopt;

		// オプション設定
		DWORD opts = 0;
		pfd->GetOptions(&opts);
		pfd->SetOptions(opts | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);

		// タイトル
		if(!In_Title.empty())
			pfd->SetTitle(In_Title.data());

		// デフォルトの拡張子
		if(!In_DefaultExtension.empty())
			pfd->SetDefaultExtension(In_DefaultExtension.data());

		// 初期フォルダ
		if(!In_DefaultFolder.empty())
		{
			IShellItem *psi = nullptr;
			if(SUCCEEDED(SHCreateItemFromParsingName(In_DefaultFolder.data(), nullptr, IID_PPV_ARGS(&psi))))
			{
				pfd->SetFolder(psi);
				psi->Release();
			}
		}

		// フィルタ
		std::vector<std::wstring> owned;
		std::vector<COMDLG_FILTERSPEC> specs;
		BuildFilterSpecs(In_Filter, owned, specs);

		// フィルタがある場合は設定
		if(!specs.empty())
		{
			pfd->SetFileTypes((UINT)specs.size(), specs.data());
			pfd->SetFileTypeIndex(1);
		}

		// ダイアログ表示
		hr = pfd->Show(In_Owner);
		if(FAILED(hr))
		{
			pfd->Release();
			return std::nullopt; // キャンセル等
		}

		// 結果取得
		IShellItem *result = nullptr;
		hr = pfd->GetResult(&result);
		if(FAILED(hr))
		{
			pfd->Release();
			return std::nullopt;
		}

		// ファイルシステムパスを取得
		PWSTR path = nullptr;
		hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

		// 後始末
		result->Release();
		pfd->Release();

		if(FAILED(hr) || !path)
			return std::nullopt;

		std::wstring selected(path);
		CoTaskMemFree(path);
		return selected;
	}

	std::optional<std::string> PickFileUTF8(_In_ HWND In_Owner, _In_ std::string_view In_Title, _In_ std::string_view In_DefaultFolder, _In_ std::string_view In_Filter, _In_ std::string_view In_DefaultExtension)
	{
		std::wstring wtitle = In_Title.empty() ? L"Open File" : UTF8ToWide(In_Title);
		std::wstring wdefault = UTF8ToWide(In_DefaultFolder);
		std::wstring wfilter = UTF8ToWide(In_Filter);
		std::wstring wext = UTF8ToWide(In_DefaultExtension);

		auto picked = PickFileW(In_Owner, wtitle, wdefault, wfilter, wext);
		if(!picked)
			return std::nullopt;

		return WideToUTF8(*picked);
	}
}