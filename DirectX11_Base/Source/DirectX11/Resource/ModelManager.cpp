/*+===================================================================
	File: ModelManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:04:02 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelManager.hpp"

std::shared_ptr<Mesh> ModelManager::GetMesh(const FilePath &In_File) noexcept
{
	auto itr = m_mapModels.find(In_File.data());

	if (itr != m_mapModels.end())
		return itr->second;

	return nullptr;
}

std::shared_ptr<Mesh> ModelManager::CreateMesh(const aiMesh *In_Mesh, const FilePath &In_File, const float &In_Scale)
{
	
}
