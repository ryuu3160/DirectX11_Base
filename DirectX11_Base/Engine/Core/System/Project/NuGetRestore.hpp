/*+===================================================================
	File: NuGetRestore.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/14 Sat AM 07:22:29 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

namespace NuGet
{
	/// <summary>
	/// NuGetパッケージを復元します
	/// </summary>
	/// <param name="[In_NugetExe]">NuGet実行可能ファイルへのパス</param>
	/// <param name="[In_SolutionPath]">ソリューションファイルへのパス</param>
	/// <param name="[Out_Message]">操作に関するメッセージを受け取るためのオプションの出力パラメータ</param>
	/// <returns>復元が成功した場合はtrue、それ以外の場合はfalse</returns>
	bool Restore(_In_ const std::filesystem::path &In_NugetExe, _In_ const std::filesystem::path &In_SolutionPath, _Out_ std::wstring *Out_Message = nullptr);
}
