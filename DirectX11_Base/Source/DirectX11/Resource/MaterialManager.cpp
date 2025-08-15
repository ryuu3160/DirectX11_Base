/*+===================================================================
	File: MaterialManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:03:28 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MaterialManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

std::shared_ptr<Material> MaterialManager::GetMaterial(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File) noexcept
{
	std::string MaterialName;

	aiString MatName;
	std::string FbxName = std::string(In_File);
	FbxName = FbxName.substr(FbxName.find_last_of('/') + 1);
	MatName = In_pMaterial->GetName();
	MaterialName = FbxName + "_" + MatName.C_Str();

	// マテリアル名で検索
	auto itr = m_mapMaterials.find(MaterialName);
	if (itr == m_mapMaterials.end())
	{
		// マテリアルが存在しない場合は新規作成
		auto Material = CreateMaterial(In_pMaterial, In_File, MaterialName);
		if (Material)
			return Material;
		else
			return nullptr; // マテリアルの作成に失敗した場合はnullptrを返す
	}
	else
	{
		return itr->second; // 既存のマテリアルを返す
	}
}

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

std::shared_ptr<Material> MaterialManager::CreateMaterial(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File, _In_ const std::string In_MaterialName) noexcept
{
	// マテリアルの作成
	auto Mat = std::make_shared<Material>();

	if (Mat)
	{
		// マテリアルのロード
		Mat->Load(In_pMaterial, In_File, In_MaterialName);
		m_mapMaterials[In_MaterialName] = Mat;
		return Mat;
	}

	return nullptr; // マテリアルの作成に失敗した場合はnullptrを返す
}
