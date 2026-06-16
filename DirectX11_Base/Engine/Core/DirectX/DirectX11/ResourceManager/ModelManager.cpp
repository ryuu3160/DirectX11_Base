/*+===================================================================
	File: ModelManager.cpp
	Summary: メッシュを管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:04:02 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelManager.hpp"
#include "Core/System/Utility/Utility.hpp"

#include <assimp/scene.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

std::shared_ptr<Mesh> ModelManager::GetMesh(_In_ std::string_view In_MeshName) noexcept
{
	auto itr = m_mapMeshes.find(In_MeshName.data());

	if (itr != m_mapMeshes.end())
		return itr->second;

	return nullptr;
}

std::shared_ptr<Mesh> ModelManager::CreateMesh(_In_ const aiMesh *In_Mesh, _In_ const FilePath &In_File, _In_ const float &In_Scale, _In_ const int In_MeshIndex, _In_ std::shared_ptr<Material> In_Material)
{
	std::string MeshName;
	std::string MatName;
	std::string FbxName = std::string(In_File);
	std::string MeshIndexStr;
	FbxName = FbxName.substr(FbxName.find_last_of('/') + 1);
	MatName = In_Material->GetMaterialName();
	MatName = MatName.substr(MatName.find_first_of('_') + 1); // マテリアル名からFBX名を除去
	MeshIndexStr = ToString(In_MeshIndex);
	MeshName = FbxName + "_" + MatName + "_" + MeshIndexStr;

	std::shared_ptr<Mesh> mesh = GetMesh(MeshName);

	if (mesh == nullptr)
	{
		mesh = std::make_shared<Mesh>();
		mesh->Load(In_Mesh, In_Scale, In_Material);
		m_mapMeshes[MeshName] = mesh;
	}

	return mesh;
}
