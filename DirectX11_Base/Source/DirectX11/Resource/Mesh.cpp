/*+===================================================================
	File: Mesh.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:04:14 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Mesh.hpp"
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

void Mesh::Load(_In_ const aiMesh *In_Mesh, _In_ const float &In_Scale, _In_ std::shared_ptr<Material> In_Material) noexcept
{
	// メッシュの作成
	aiVector3D zero(0.0f, 0.0f, 0.0f);

	// 頂点の作成
	std::vector<Vertex> vtx;
	vtx.resize(In_Mesh->mNumVertices);
	for (unsigned int j = 0; j < vtx.size(); ++j)
	{
		// 値の吸出し
		aiVector3D pos = In_Mesh->mVertices[j];
		aiVector3D uv = In_Mesh->HasTextureCoords(0) ? In_Mesh->mTextureCoords[0][j] : zero;
		aiVector3D normal = In_Mesh->HasNormals() ? In_Mesh->mNormals[j] : zero;
		// 値を設定
		vtx[j] = {
			DirectX::XMFLOAT3(pos.x * In_Scale, pos.y * In_Scale, pos.z * In_Scale),
			DirectX::XMFLOAT3(normal.x, normal.y, normal.z),
			DirectX::XMFLOAT2(uv.x, uv.y),
		};
	}

	// マテリアルの設定
	m_spMaterial = In_Material;

	std::vector<unsigned int> idx;
	idx.resize(In_Mesh->mNumFaces * 3); // faceはポリゴンの数を表す(１ポリゴンで3インデックス
	for (unsigned int j = 0; j < In_Mesh->mNumFaces; ++j)
	{
		aiFace face = In_Mesh->mFaces[j];
		int faceIdx = j * 3;
		idx[faceIdx + 0] = face.mIndices[0];
		idx[faceIdx + 1] = face.mIndices[1];
		idx[faceIdx + 2] = face.mIndices[2];
	}

	// メッシュを元に頂点バッファ作成
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx.data();
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = static_cast<UINT>(vtx.size());
	desc.pIdx = idx.data();
	desc.idxSize = sizeof(unsigned int);
	desc.idxCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_spMeshBuffer = std::make_shared<MeshBuffer>(desc);
}

Mesh::Mesh()
	: m_spMeshBuffer(nullptr)
	, m_spMaterial(nullptr)
{
}

Mesh::~Mesh()
{
}
