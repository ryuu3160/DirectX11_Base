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
#include "Core/System/Component/Camera/Camera.hpp"
#include "Core/DirectX11/System/RenderManager.hpp"
#include "Core/DirectX11/ResourceManager/MaterialManager.hpp"
#include "Core/DirectX11/ResourceManager/TextureManager.hpp"
#include "Core/DirectX11/ResourceManager/ModelManager.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include "Core/System/Object/GameObject.hpp"

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
	: RenderComponent("ModelRenderer")
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_fScale(1.0f)
	, m_bUseMaterialShader(false)
	, m_pShaderParamsPS{}, m_pShaderParamsVS{}
{
}

ModelRenderer::~ModelRenderer()
{
	m_pShaderParamsPS.clear();
	m_pShaderParamsVS.clear();
	m_vecMeshes.clear();
}

void ModelRenderer::Awake() noexcept
{
	if(!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = reinterpret_cast<VertexShader *>(m_defVS.get());
	m_pPS = m_defPS.get();

	m_vecMeshes.clear();

	m_pShaderParamsPS.clear();
	m_pShaderParamsVS.clear();
}

void ModelRenderer::Update(_In_ float In_DeltaTime) noexcept
{
	// 既にメッシュが読み込まれている場合は何もしない
	if (!m_vecMeshes.empty()) return;

	this->Load(m_AssetPath, m_fScale);
}

void ModelRenderer::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	VertexShader *cast = reinterpret_cast<VertexShader *>(In_Vs);
	if(cast)
	{
		m_pVS = cast;

	}
}

void ModelRenderer::SetPixelShader(_In_ Shader *In_pShader) noexcept
{
	PixelShader *cast = reinterpret_cast<PixelShader *>(In_pShader);
	if (cast)
		m_pPS = cast;
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
		DebugManager::GetInstance().DebugLogError(importer.GetErrorString());
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

void ModelRenderer::Draw(_In_ RenderContext *In_RenderContext) noexcept
{
	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = In_RenderContext->GetView(false);
	mat[2] = In_RenderContext->GetProj(false);

	// 単位行列でワールド行列を作成
	mat[0] = m_pGameObject->GetWorld(false);

	// マテリアルのシェーダーを使用しない場合は、デフォルトのシェーダーを使用
	if (!m_bUseMaterialShader)
	{
		// メモリ上の行列をグラフィックスメモリへコピー
		// 1つ目の引数はバッファの番号
		m_pVS->WriteBuffer(0, mat);

		// 設定されたパラメーターをVSに書き込む
		for (auto &itr : m_pShaderParamsVS)
		{
			if (itr)
			{
				m_pVS->WriteBuffer(itr->GetSlotNum(), itr->GetParam());
			}
		}
		m_pVS->Bind();
		m_pShaderParamsVS.clear();

		// 設定されたパラメーターをPSに書き込む
		for (auto &itr : m_pShaderParamsPS)
		{
			if (itr)
			{
				m_pPS->WriteBuffer(itr->GetSlotNum(), itr->GetParam());
			}
		}
		m_pPS->Bind();
		m_pShaderParamsPS.clear();
	}

	for (auto &itr : m_vecMeshes)
	{
		// マテリアルのシェーダーを使用する場合は、マテリアルのシェーダーをバインド
		if (m_bUseMaterialShader)
		{
			VertexShader *pVS = itr->GetMaterial()->GetVertexShader();
			PixelShader *pPS = itr->GetMaterial()->GetPixelShader();
			if (pVS)
			{
				pVS->WriteBuffer(0, mat);
				// ToDo: マテリアルごとのVSにパラメーターを書き込む

				pVS->Bind();
			}
			if (pPS)
			{
				// マテリアルごとのシェーダーにパラメーターを書き込む
				for (auto &PsPara : m_pShaderParamsPS)
				{
					std::string_view name = PsPara->GetParamName();
					int slot = PsPara->GetSlotNum();
					for (auto &info : itr->GetMaterial()->GetShaderParamList())
					{
						// パラメーター名とスロット番号が一致したら書き込み
						if (name == info.ParamName && slot == info.SlotNum)
						{
							pPS->WriteBuffer(slot, PsPara->GetParam());
							break;
						}
					}
				}

				pPS->Bind();
				m_pShaderParamsPS.clear();
			}
		}

		// 設定されているテクスチャをシェーダーに設定
		if (itr->GetMaterial()->GetTextureNum() > 0)
		{
			const auto &Textures = itr->GetMaterial()->GetTextures();
			for(int i = 0; i < ResourceSetting::TextureType_Max; ++i)
			{
				// テクスチャが設定されていない場合はスキップ
				if (!Textures[i])
					continue;

				// 使用するシェーダーがマテリアルのシェーダーかどうかを判定
				if (m_bUseMaterialShader)
				{
					// マテリアルのシェーダーを使用する場合は、マテリアルのシェーダーにテクスチャを設定
					itr->GetMaterial()->GetPixelShader()->SetTexture(i, Textures[i].get());
				}
				else
				{
					// モデル全体のシェーダーを使用する場合は、モデルのピクセルシェーダーにテクスチャを設定
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
	m_defVS->Load("Binaries/Shader/VS_Model.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Binaries/Shader/PS_Model.cso");
}
