/*+===================================================================
	File: InstancedModelRenderer.cpp
	Summary: インスタンシング用のモデルレンダラークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 05:06:05 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InstancedModelRenderer.hpp"
#include "System/Component/Camera.hpp"
#include "DirectX11/ResourceManager/MaterialManager.hpp"
#include "DirectX11/ResourceManager/TextureManager.hpp"
#include "DirectX11/ResourceManager/InstancedModelManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

InstancedModelRenderer::InstancedModelRenderer()
	: RenderComponent("InstancedModelRenderer")
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_fScale(1.0f)
	, m_bUseMaterialShader(false)
	, m_pShaderParams{}
	, m_AlignInstanceData{}
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = reinterpret_cast<InstancedVertexShader *>(m_defVS.get());
	m_pPS = m_defPS.get();

	m_vecMeshes.clear();

	m_pShaderParams.clear();
}

InstancedModelRenderer::~InstancedModelRenderer()
{
	m_vecMeshes.clear();
	m_pShaderParams.clear();
}

void InstancedModelRenderer::Update(_In_ float In_Tick) noexcept
{
	// 既にメッシュが読み込まれている場合は何もしない
	if (!m_vecMeshes.empty()) return;

	this->Load(m_AssetPath,m_AlignInstanceData, m_fScale);
}

void InstancedModelRenderer::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	InstancedVertexShader *cast = reinterpret_cast<InstancedVertexShader *>(In_Vs);
	if (cast)
		m_pVS = cast;
}

void InstancedModelRenderer::SetPixelShader(_In_ Shader *In_pShader) noexcept
{
	PixelShader *cast = reinterpret_cast<PixelShader *>(In_pShader);
	if (cast)
		m_pPS = cast;
}

bool InstancedModelRenderer::Load(_In_ const FilePath &In_File, _In_ const InstancedMesh::AlignInstanceData &In_InstanceData, _In_ const float &In_Scale, _In_ const bool &In_IsFlip) noexcept
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
		aiMaterial *aiMat = pScene->mMaterials[aiMesh->mMaterialIndex];
		auto material = MaterialManager::GetInstance().GetMaterial(aiMat, In_File);
		material->SetIsInstancedVertexShader(true);

		// モデルの作成
		auto Mesh = InstancedModelManager::GetInstance().CreateMesh(aiMesh, In_File, In_Scale, In_InstanceData, i, material);

		// メッシュ追加
		m_vecMeshes.push_back(Mesh);
	}

	return true;
}

void InstancedModelRenderer::Draw(_In_ RenderContext *In_RenderContext) noexcept
{
	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = In_RenderContext->GetView(false);
	mat[2] = In_RenderContext->GetProj(false);

	// 単位行列でワールド行列を作成
	mat[0] = m_pTransform->GetWorld(false);

	for (auto &itr : m_vecMeshes)
	{
		// マテリアルのシェーダーを使用する場合は、マテリアルのシェーダーをバインド
		if (m_bUseMaterialShader)
		{
			InstancedVertexShader *pVS = itr->GetMaterial()->GetInstancedVertexShader();
			PixelShader *pPS = itr->GetMaterial()->GetPixelShader();
			if (pVS)
			{
				pVS->WriteBuffer(0, mat);
				pVS->SetInstanceSRV(itr->GetMesh()->GetInstanceSRV());
				pVS->Bind();
			}
			if (pPS)
			{
				// マテリアルごとのシェーダーにパラメーターを書き込む
				for (auto &PsPara : m_pShaderParams)
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
				m_pShaderParams.clear();
			}
		}
		else
		{
			// メモリ上の行列をグラフィックスメモリへコピー
			// 1つ目の引数はバッファの番号
			m_pVS->WriteBuffer(0, mat);
			m_pVS->SetInstanceSRV(itr->GetMesh()->GetInstanceSRV());
			m_pVS->Bind();

			// 設定されたパラメーターをPSに書き込む
			for (auto &itr : m_pShaderParams)
			{
				if (itr)
				{
					m_pPS->WriteBuffer(itr->GetSlotNum(), itr->GetParam());
				}
			}
			m_pPS->Bind();
			m_pShaderParams.clear();
		}

		// 設定されているテクスチャをシェーダーに設定
		if (itr->GetMaterial()->GetTextureNum() > 0)
		{
			const auto &Textures = itr->GetMaterial()->GetTextures();
			for (int i = 0; i < ResourceSetting::TextureType_Max; ++i)
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

void InstancedModelRenderer::RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func)
{
	for (auto &itr : m_vecMeshes)
	{
		// メッシュの頂点バッファの情報を取得
		InstancedMeshBuffer::InstancingDesc desc = itr->GetMesh()->GetDesc();

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
		itr->ReplaceMeshBuffer(std::make_shared<InstancedMeshBuffer>(desc));

		// 使用したメモリを解放
		delete[] newVtx;
	}
}

void InstancedModelRenderer::MakeDefaultShader()
{
	m_defVS = std::make_shared<InstancedVertexShader>();
	m_defVS->Load("Assets/Shader/IVS_InstancedObject.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Model.cso");
}
