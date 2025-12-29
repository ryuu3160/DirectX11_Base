/*+===================================================================
	File: ShaderManager.cpp
	Summary: シェーダーを管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/17 Sun AM 01:33:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ShaderManager.hpp"

void ShaderManager::SetupShaders(_In_ const std::vector<std::string> &In_FileNames) noexcept
{
	// 非同期でシェーダーをセットアップ
	std::vector<std::future<void>> futures;

	for (const auto &itr : In_FileNames)
	{
		auto async = std::async(std::launch::async, &ShaderManager::SetupShader, this, std::string_view(itr));
		futures.push_back(std::move(async));
	}
	// 全ての非同期処理が完了するまで待機
	for (auto &future : futures)
		future.get();
}

void ShaderManager::SetupShader(_In_ std::string_view In_FileName) noexcept
{
	Shader *shader = nullptr;
	// シェーダーの種類を判定
	if(In_FileName.find("PS_") != std::string::npos)
	{
		shader = CreateShader<PixelShader>(In_FileName);
	}
	else if(In_FileName.find("IVS_") != std::string::npos)
	{
		shader = CreateShader<InstancedVertexShader>(In_FileName);
	}
	else if(In_FileName.find("VS_") != std::string::npos)
	{
		shader = CreateShader<VertexShader>(In_FileName);
	}
	else if(In_FileName.find("GS_") != std::string::npos)
	{
		shader = CreateShader<GeometryShader>(In_FileName);
	}
	else if(In_FileName.find("HS_") != std::string::npos)
	{
		shader = CreateShader<HullShader>(In_FileName);
	}
	else if(In_FileName.find("DS_") != std::string::npos)
	{
		shader = CreateShader<DomainShader>(In_FileName);
	}
	else if(In_FileName.find("CS_") != std::string::npos)
	{
		shader = CreateShader<ComputeShader>(In_FileName);
	}
	else
	{
		DebugManager::GetInstance().DebugLogError("Shader name [VS_ / PS_] : {}", In_FileName);
	}
	// csoファイルのパスを設定
	std::string path = "Assets/Shader/";
	path += In_FileName;
	path += ".cso";
	if(shader && FAILED(shader->Load(path.c_str())))
	{
		DebugManager::GetInstance().DebugLogError("Shader Load Error : {}", In_FileName);
	}
}

Shader *ShaderManager::GetShader(_In_ const FilePath &In_FileName) noexcept
{
	// ファイル名で検索
	auto itr = m_mapShaders.find(In_FileName.data());

	// ヒットしたら返す
	if (itr != m_mapShaders.end())
		return itr->second;// 既存のシェーダーを返す

	// ヒットしなかったらnullptrを返す
	return nullptr;
}

void ShaderManager::ReleaseShader(_In_ const FilePath &In_FileName) noexcept
{
	// ファイル名で検索
	auto itr = m_mapShaders.find(In_FileName.data());
	// 見つかったら削除
	if (itr != m_mapShaders.end())
	{
		// シェーダーを解放
		if (itr->second)
		{
			delete itr->second;
			itr->second = nullptr;
		}
		m_mapShaders.erase(itr);
	}
}

ShaderManager::ShaderManager()
{
	// シェーダーのマップを初期化
	m_mapShaders.clear();
}

ShaderManager::~ShaderManager()
{
	// シェーダーを開放
	for (auto &itr : m_mapShaders)
	{
		if (itr.second)
		{
			delete itr.second;
			itr.second = nullptr;
		}
	}
	m_mapShaders.clear();
}
