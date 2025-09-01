/*+===================================================================
	File: InstancedMesh.cpp
	Summary: インスタンシング用のメッシュクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 04:01:07 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InstancedMesh.hpp"

InstancedMesh::InstancedMesh()
	: m_spMeshBuffer(nullptr), m_spMaterial(nullptr)
{
}

InstancedMesh::~InstancedMesh()
{
}

void InstancedMesh::Load(_In_ const aiMesh *In_Mesh, _In_ const float &In_Scale, _In_ const AlignInstanceData &In_InstanceData, _In_ std::shared_ptr<Material> In_Material) noexcept
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

	// インスタンシングの設定
	std::vector<PerInstanceData> instance;
	instance.resize(In_InstanceData.CountX * In_InstanceData.CountY * In_InstanceData.CountZ);
	for (int y = 0; y < In_InstanceData.CountY; ++y)
	{
		for (int z = 0; z < In_InstanceData.CountZ; ++z)
		{
			for (int x = 0; x < In_InstanceData.CountX; ++x)
			{
				int Idx = x + (z * In_InstanceData.CountX) + (y * In_InstanceData.CountX * In_InstanceData.CountZ);
				instance[Idx].pos = {
					In_InstanceData.StartPos.x + (In_InstanceData.ShiftPosOffset.x * static_cast<float>(x)),
					In_InstanceData.StartPos.y + (In_InstanceData.ShiftPosOffset.y * static_cast<float>(y)),
					In_InstanceData.StartPos.z + (In_InstanceData.ShiftPosOffset.z * static_cast<float>(z))
				};
				instance[Idx].scale = In_InstanceData.Scale;
				instance[Idx].quaternion = In_InstanceData.Quaternion;
			}
		}
	}

	// メッシュを元に頂点バッファ作成
	InstancedMeshBuffer::InstancingDesc desc = {};
	desc.pVtx = vtx.data();
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = static_cast<UINT>(vtx.size());
	desc.pIdx = idx.data();
	desc.idxSize = sizeof(unsigned int);
	desc.idxCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// インスタンシング用の情報
	desc.instanceSize = sizeof(PerInstanceData);
	desc.instanceCount = static_cast<UINT>(instance.size());
	desc.isInstanceWrite = In_InstanceData.IsWrite;
	desc.pInstance = instance.data();

	m_spMeshBuffer = std::make_shared<InstancedMeshBuffer>(desc);
}

std::shared_ptr<InstancedMeshBuffer> InstancedMesh::GetMesh() const noexcept
{
	return m_spMeshBuffer;
}

std::shared_ptr<Material> InstancedMesh::GetMaterial() const noexcept
{
	return m_spMaterial;
}

void InstancedMesh::ReplaceMeshBuffer(_In_ std::shared_ptr<InstancedMeshBuffer> In_MeshBuffer) noexcept
{
	if (In_MeshBuffer)
	{
		m_spMeshBuffer = In_MeshBuffer;
	}
	else
	{
		Error("MeshBuffer is nullptr.");
	}
}
