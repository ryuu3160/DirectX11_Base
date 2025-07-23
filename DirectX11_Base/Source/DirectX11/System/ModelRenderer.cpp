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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

// ==============================
//  静的メンバ変数の定義
// ==============================
std::shared_ptr<VertexShader> ModelRenderer::m_defVS = nullptr;
std::shared_ptr<PixelShader> ModelRenderer::m_defPS = nullptr;

ModelRenderer::ModelRenderer()
	: m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_cModelName{}
	, m_fScale(1.0f)
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = m_defVS.get();
	m_pPS = m_defPS.get();

	m_vecMeshes.clear();
	m_vecMaterials.clear();
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::ExecuteUpdate() noexcept
{
	// 既にメッシュが読み込まれている場合は何もしない
	if (!m_vecMeshes.empty()) return;

	this->Load(m_cModelName, m_fScale);
}

void ModelRenderer::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<FilePath>(&m_cModelName);
}

void ModelRenderer::SetModelPath(_In_ FilePath In_File) noexcept
{
	m_cModelName = In_File;
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

	// メッシュの作成
	aiVector3D zero(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		Mesh mesh = {};

		// 頂点の作成
		std::vector<Vertex> vtx;
		vtx.resize(pScene->mMeshes[i]->mNumVertices);
		for (unsigned int j = 0; j < vtx.size(); ++j)
		{
			// 値の吸出し
			aiVector3D pos = pScene->mMeshes[i]->mVertices[j];
			aiVector3D uv = pScene->mMeshes[i]->HasTextureCoords(0) ?
				pScene->mMeshes[i]->mTextureCoords[0][j] : zero;
			aiVector3D normal = pScene->mMeshes[i]->HasNormals() ?
				pScene->mMeshes[i]->mNormals[j] : zero;
			// 値を設定
			vtx[j] = {
				DirectX::XMFLOAT3(pos.x * In_Scale, pos.y * In_Scale, pos.z * In_Scale),
				DirectX::XMFLOAT3(normal.x, normal.y, normal.z),
				DirectX::XMFLOAT2(uv.x, uv.y),
			};
		}

		// インデックスの作成
		std::vector<unsigned int> idx;
		idx.resize(pScene->mMeshes[i]->mNumFaces * 3); // faceはポリゴンの数を表す(１ポリゴンで3インデックス
		for (unsigned int j = 0; j < pScene->mMeshes[i]->mNumFaces; ++j)
		{
			aiFace face = pScene->mMeshes[i]->mFaces[j];
			int faceIdx = j * 3;
			idx[faceIdx + 0] = face.mIndices[0];
			idx[faceIdx + 1] = face.mIndices[1];
			idx[faceIdx + 2] = face.mIndices[2];
		}

		// マテリアルの割り当て
		mesh.materialID = pScene->mMeshes[i]->mMaterialIndex;

		// メッシュを元に頂点バッファ作成
		MeshBuffer::Description desc = {};
		desc.pVtx = vtx.data();
		desc.vtxSize = sizeof(Vertex);
		desc.vtxCount = static_cast<UINT>(vtx.size());
		desc.pIdx = idx.data();
		desc.idxSize = sizeof(unsigned int);
		desc.idxCount = static_cast<UINT>(idx.size());
		desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mesh.mesh = std::make_shared<MeshBuffer>(desc);

		// メッシュ追加
		m_vecMeshes.push_back(mesh);
	}

	//--- マテリアルの作成
	// ファイルの探索
	std::string dir = std::string(In_File);
	dir = dir.substr(0, dir.find_last_of('/') + 1);
	// マテリアル
	aiColor3D color(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 ambient(0.3f, 0.3f, 0.3f, 1.0f);
	for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
	{
		Material material = {};

		// 各種パラメーター
		float shininess;
		// 色情報
		material.diffuse = pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS ?
			DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f) : diffuse;
		// 環境光の色
		material.ambient = pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS ?
			DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f) : ambient;
		// 鏡面反射の色
		shininess = pScene->mMaterials[i]->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS ? shininess : 0.0f;
		material.specular = pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS ?
			DirectX::XMFLOAT4(color.r, color.g, color.b, shininess) : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, shininess);

		// テクスチャ
		HRESULT hr;
		aiString path;
		if (pScene->mMaterials[i]->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{
			// そのまま探索
			material.texture = std::make_shared<Texture>();
			hr = material.texture->Create(path.C_Str());
			// モデルと同じ階層を探索
			if (FAILED(hr))
			{
				std::string file = dir;
				file += path.C_Str();
				hr = material.texture->Create(file.c_str());
			}
			// ファイル名のみで探索
			if (FAILED(hr))
			{
				std::string file = path.C_Str();
				size_t idx = file.find_last_of('\\');
				if (idx != std::string::npos)
				{
					file = file.substr(idx + 1);
					file = dir + file;
					hr = material.texture->Create(file.c_str());
				}
			}
			// 失敗
			if (FAILED(hr))
			{
				Error(path.C_Str());
				material.texture = nullptr;
			}
		}
		else
		{
			material.texture = nullptr;
		}

		// マテリアル追加
		m_vecMaterials.push_back(material);
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
		if (m_nTexSlot >= 0)
			m_pPS->SetTexture(m_nTexSlot, m_vecMaterials[itr.materialID].texture.get());
		itr.mesh->Draw();
	}
}

void ModelRenderer::RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func)
{
	for (auto &itr : m_vecMeshes)
	{
		// メッシュの頂点バッファの情報を取得
		MeshBuffer::Description desc = itr.mesh->GetDesc();

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
		itr.mesh = std::make_shared<MeshBuffer>(desc);

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
