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
	std::vector<PerInstanceData> instance = CreateAlignInstanceData(In_InstanceData);

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
		DebugManager::GetInstance().DebugLogError("MeshBuffer is nullptr.");
	}
}

std::vector<InstancedMesh::PerInstanceData> InstancedMesh::CreateAlignInstanceData(_In_ const AlignInstanceData &In_InstanceData) const noexcept
{
	std::vector<PerInstanceData> instance;
	instance.resize(In_InstanceData.CountX * In_InstanceData.CountY * In_InstanceData.CountZ);

	// オフセットの計算
	float StartY, MaxY;
	float StartZ, MaxZ;
	float StartX, MaxX;

	switch (In_InstanceData.AnchorPoint.y)
	{
	default:
	case AnchorY::Bottom:
		StartY = 0;
		MaxY = static_cast<float>(In_InstanceData.CountY);
		break;
	case AnchorY::Center:
		StartY = -static_cast<float>(In_InstanceData.CountY / 2.0f);
		StartY += 0.5f;
		MaxY = (static_cast<float>(In_InstanceData.CountY / 2.0f)) - 0.5f;
		break;
	case AnchorY::Top:
		StartY = static_cast<float>(-In_InstanceData.CountY + 1);
		MaxY = 1;
		break;
	}
	switch (In_InstanceData.AnchorPoint.z)
	{
	default:
	case AnchorZ::Back:
		StartZ = 0;
		MaxZ = static_cast<float>(In_InstanceData.CountZ);
		break;
	case AnchorZ::Center:
		StartZ = -static_cast<float>(In_InstanceData.CountY / 2.0f);
		StartZ += 0.5f;
		MaxZ = (static_cast<float>(In_InstanceData.CountY / 2.0f)) - 0.5f;
		break;
	case AnchorZ::Front:
		StartZ = static_cast<float>(-In_InstanceData.CountZ + 1);
		MaxZ = 1;
		break;
	}
	switch (In_InstanceData.AnchorPoint.x)
	{
	default:
	case AnchorX::Left:
		StartX = 0;
		MaxX = static_cast<float>(In_InstanceData.CountX);
		break;
	case AnchorX::Center:
		StartX = -static_cast<float>(In_InstanceData.CountY / 2.0f);
		StartX += 0.5f;
		MaxX = (static_cast<float>(In_InstanceData.CountY / 2.0f)) - 0.5f;
		break;
	case AnchorX::Right:
		StartX = static_cast<float>(-In_InstanceData.CountX + 1);
		MaxX = 1;
		break;
	}
	int IdxY = 0, IdxZ = 0, IdxX = 0;
	for (float y = StartY,IdxY = 0; y < MaxY; ++y,++IdxY)
	{
		for (float z = StartZ,IdxZ = 0; z < MaxZ; ++z, ++IdxZ)
		{
			for (float x = StartX,IdxX = 0; x < MaxX; ++x, ++IdxX)
			{
				int Idx = static_cast<int>(IdxX + (IdxZ * In_InstanceData.CountX) + (IdxY * In_InstanceData.CountX * In_InstanceData.CountZ));
				instance[Idx].pos = {
					In_InstanceData.StartPos.x + (In_InstanceData.ShiftPosOffset.x * x),
					In_InstanceData.StartPos.y + (In_InstanceData.ShiftPosOffset.y * y),
					In_InstanceData.StartPos.z + (In_InstanceData.ShiftPosOffset.z * z)
				};
				instance[Idx].scale = In_InstanceData.Scale;
				instance[Idx].quaternion = In_InstanceData.Quaternion;
			}
		}
	}

	return instance;
}
