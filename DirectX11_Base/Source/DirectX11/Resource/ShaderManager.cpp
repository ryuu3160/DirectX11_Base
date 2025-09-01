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
	for (const auto &itr : In_FileNames)
	{
		Shader *shader = nullptr;
		// シェーダーの種類を判定
		if (itr.find("PS_") != std::string::npos)
		{
			shader = CreateShader<PixelShader>(itr);
		}
		else if (itr.find("IVS_") != std::string::npos)
		{
			shader = CreateShader<InstancedVertexShader>(itr);
		}
		else if(itr.find("VS_") != std::string::npos)
		{
			shader = CreateShader<VertexShader>(itr);
		}
		else if (itr.find("GS_") != std::string::npos)
		{
			shader = CreateShader<GeometryShader>(itr);
		}
		else if (itr.find("HS_") != std::string::npos)
		{
			shader = CreateShader<HullShader>(itr);
		}
		else if (itr.find("DS_") != std::string::npos)
		{
			shader = CreateShader<DomainShader>(itr);
		}
		else if (itr.find("CS_") != std::string::npos)
		{
			shader = CreateShader<ComputeShader>(itr);
		}
		else
		{
			MessageBoxA(NULL, itr.c_str(), "Shader name [VS_ / PS_]", MB_OK);
		}
		// csoファイルのパスを設定
		std::string path = "Assets/Shader/";
		path += itr;
		path += ".cso";
		if (shader && FAILED(shader->Load(path.c_str())))
		{
			MessageBoxA(NULL, itr.c_str(), "Shader Error", MB_OK);
		}
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
