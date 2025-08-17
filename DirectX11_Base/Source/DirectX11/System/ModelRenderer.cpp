/*+===================================================================
	File: ModelRenderer.cpp
	Summary: モデルレンダラー 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/01 Tue AM 10:45:21 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelRenderer.hpp"
#include "System/Component/Camera.hpp"
#include "DirectX11/Resource/MaterialManager.hpp"
#include "DirectX11/Resource/TextureManager.hpp"
#include "DirectX11/Resource/ModelManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

ModelRenderer::ModelRenderer()
	: m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_fScale(1.0f)
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = m_defVS.get();
	m_pPS = m_defPS.get();

	m_vecMeshes.clear();
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::ExecuteUpdate() noexcept
{
	// 既にメッシュが読み込まれている場合は何もしない
	if (!m_vecMeshes.empty()) return;

	this->Load(m_AssetPath, m_fScale);
}

void ModelRenderer::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<FilePath>(&m_AssetPath);
}

void ModelRenderer::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	VertexShader *cast = reinterpret_cast<VertexShader *>(In_Vs);
	if (cast)
		m_pVS = cast;
}

void ModelRenderer::SetPixelShader(_In_ Shader *In_pShader) noexcept
{
	PixelShader *cast = reinterpret_cast<PixelShader *>(In_pShader);
	if (cast)
		m_pPS = cast;
}

void ModelRenderer::SetTextureSlot(_In_ const int &In_TexSlot) noexcept
{
	m_nTexSlot = In_TexSlot;
}

bool ModelRenderer::Load(_In_ const FilePath &In_File, _In_ const float &In_Scale, _In_ const bool &In_IsFlip)
{
	// assimpの設定
	Assimp::Importer importer;
	int flag = 0;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_JoinIdenticalVertices;
	flag |= aiProcess_FlipUVs;
	flag |= aiProcess_PreTransformVertices;

	// 反転フラグの設定
	if (In_IsFlip) flag |= aiProcess_MakeLeftHanded;

	// assimpで読み込み
	const aiScene *pScene = importer.ReadFile(In_File.data(), flag);
	if (!pScene)
	{
		Error(importer.GetErrorString());
		return false;
	}

	// テクスチャ読み込み
	TextureManager::GetInstance().LoadTextures(pScene, In_File);

	//--- マテリアルの作成
	for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
	{
		// マテリアルを作成
		MaterialManager::GetInstance().GetMaterial(pScene->mMaterials[i], In_File);
	}

	// メッシュの作成
	aiVector3D zero(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh *aiMesh = pScene->mMeshes[i];

		// このメッシュが参照するマテリアルを取得
		aiMaterial* aiMat = pScene->mMaterials[aiMesh->mMaterialIndex];
		auto material = MaterialManager::GetInstance().GetMaterial(aiMat, In_File);

		// モデルの作成
		auto Mesh = ModelManager::GetInstance().CreateMesh(aiMesh, In_File, In_Scale,i,material);

		// メッシュ追加
		m_vecMeshes.push_back(Mesh);
	}

	return true;
}

void ModelRenderer::Draw() noexcept
{
	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = m_pViewCamera->GetView(false);
	mat[2] = m_pViewCamera->GetProj(false);

	// カメラの情報を定数バッファで渡す
	DirectX::XMFLOAT3 CamPos = m_pCameraObj->GetPos();
	DirectX::XMFLOAT4 CameraParam[] = {
		{CamPos.x,CamPos.y,CamPos.z,0.0f}
	};

	// 単位行列でワールド行列を作成
	mat[0] = m_pTransform->GetWorld(false);

	// メモリ上の行列をグラフィックスメモリへコピー
	// 1つ目の引数はバッファの番号
	m_pVS->WriteBuffer(0, mat);

	m_pVS->Bind();
	m_pPS->Bind();
	auto it = m_vecMeshes.begin();
	for (auto &itr : m_vecMeshes)
	{
		if (itr->GetMaterial()->GetTextureNum() > 0)
		{
			const auto &Textures = itr->GetMaterial()->GetTextures();
			for(int i = 0; i < ResourceSetting::TextureType_Max; ++i)
			{
				if (Textures[i])
				{
					// テクスチャが存在する場合はシェーダーに設定
					m_pPS->SetTexture(i, Textures[i].get());
				}
			}
		}
		itr->GetMesh()->Draw();
	}
}

void ModelRenderer::RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func)
{
	for (auto &itr : m_vecMeshes)
	{
		// メッシュの頂点バッファの情報を取得
		MeshBuffer::Description desc = itr->GetMesh()->GetDesc();

		// 新しい頂点バッファのメモリを確保
		char *newVtx = new char[In_VtxSize * desc.vtxCount];

		// 新しい頂点バッファの情報を設定
		RemakeInfo data = {};
		data.vtxNum = desc.vtxCount;
		data.dest = newVtx;
		data.source = desc.pVtx;
		data.idxNum = desc.idxCount;
		data.idx = desc.pIdx;
		// 頂点データの再生成
		In_Func(data);

		// 既存の頂点バッファを置き換え
		desc.pVtx = newVtx;
		desc.vtxSize = In_VtxSize;
		itr->ReplaceMeshBuffer(std::make_shared<MeshBuffer>(desc));

		// 使用したメモリを解放
		delete[] newVtx;
	}
}

void ModelRenderer::MakeDefaultShader()
{
	m_defVS = std::make_shared<VertexShader>();
	m_defVS->Load("Assets/Shader/VS_Model.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Model.cso");
}
