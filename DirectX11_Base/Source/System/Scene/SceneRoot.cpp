/*+===================================================================
	File: SceneRoot.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneRoot.hpp"
#include "Source/App/Main.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "DirectX11/System/InstancedModelRenderer.hpp"
#include "System/SpriteManager/SpriteManager.hpp"

// ==============================
//  定数
// ==============================
namespace
{
	constexpr const float cx_fGridSize = 20.0f; // グリッドのサイズ
	constexpr const float cx_fGridMargin = 1.0f; // グリッドの間隔
	constexpr const DirectX::XMFLOAT4 cx_GridColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // グリッドの色
	constexpr const bool cx_bGridAxis = true; // グリッドの軸を表示するかどうか
}

void SceneRoot::Init()
{
	// オブジェクトの作成
	CameraDCC *pCamera = CreateObject<CameraDCC>("Camera");
	auto pCameraComp = pCamera->GetComponent<Camera>();

	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(pCameraComp);
	SpriteManager::GetInstance().SetCameraObject(pCamera);

	Setup(2);

	auto &ShaderM = ShaderManager::GetInstance();

	// オブジェクトの設定
	GameObject *pModel = GetObject<GameObject>("RootModel0");
	auto Component1 = pModel->GetComponent<ModelRenderer>();
	Component1->SetAssetPath("Assets/Model/spot/spot.fbx");
	Component1->SetCamera(pCamera);
	Component1->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component1->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	
	pModel->SetPos({ 0.0f, 1.0f, 0.0f });

	auto child = pModel->AddChildObject<GameObject>("RootModel0Child");
	auto comp = child->AddComponent<ModelRenderer>();
	comp->SetAssetPath("Assets/Model/spot/spot.fbx");
	comp->SetCamera(pCamera);
	comp->SetVertexShader(ShaderM.GetShader("VS_Object"));
	comp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	child->SetPos({ 1.0f, 0.0f, 0.0f });

	// F15Eのモデルを読み込む
	GameObject *pModel2 = GetObject<GameObject>("RootModel1");
	auto Component2 = pModel2->GetComponent<ModelRenderer>();
	Component2->SetAssetPath("Assets/Model/F15E.fbx");
	Component2->SetCamera(pCamera);
	Component2->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component2->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	Component2->IsUseMaterialShader(true); // マテリアルシェーダーを使用する

	pModel2->SetPos({ -2.0f, 0.0f, 0.0f });
	pModel2->SetScale({ 0.005f, 0.005f, 0.005f });

	// スプライトの作成
	GameObject *pSpriteObj1 = CreateObject<GameObject>("SpriteObj1");
	auto SpriteComp1 = pSpriteObj1->AddComponent<SpriteRenderer>();
	SpriteComp1->SetAssetPath("Assets/Texture/TestTexture.png");
	SpriteComp1->SetCamera(pCamera);
	SpriteComp1->Set3D(true);
	SpriteComp1->SetBillBoard(true);
	pSpriteObj1->SetPos({ 5.0f, 0.0f, 0.0f });
	SpriteComp1->SetLayer(-1); // レイヤーを設定

	GameObject *pSpriteObj2 = CreateObject<GameObject>("SpriteObj2");
	auto SpriteComp2 = pSpriteObj2->AddComponent<SpriteRenderer>();
	SpriteComp2->SetAssetPath("Assets/Texture/TestTexture.png");
	SpriteComp2->SetCamera(pCamera);
	SpriteComp2->Set3D(false);
	pSpriteObj2->SetRotation({ 0.0f, 0.0f, 90.0f });

	//// スカイボックスを作成
	//SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	//pSkyBox->SetCamera(pCamera);

	// インスタンシングテスト

	struct TangentVtx {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 tangent; // ワールド空間上でテクスチャを張る際の方向
	};
	// モデルに接ベクトルを追加する処理
	auto calcTangent = [](InstancedModelRenderer::RemakeInfo &In_Data)
		{
			// 既存データ(接ベクトル以外)は共通なので値のコピーを行う
			TangentVtx *destVtx = reinterpret_cast<TangentVtx *>(In_Data.dest);
			const Model::Vertex *srcVtx = reinterpret_cast<const Model::Vertex *>(In_Data.source);
			for (UINT i = 0; i < In_Data.vtxNum; ++i)
			{
				destVtx[i].pos = srcVtx[i].pos;
				destVtx[i].uv = srcVtx[i].uv;
				destVtx[i].normal = srcVtx[i].normal;
				destVtx[i].tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			// --- 接ベクトルの計算
			// 計算途中では1頂点に接ベクトルが複数含まれる
			using TanVecAry = std::vector<DirectX::XMFLOAT3>;
			// それが、頂点数分必要
			std::vector<TanVecAry> tangentVtx;
			tangentVtx.resize(In_Data.vtxNum);
			// 接ベクトルの計算単位は三角形
			// 三角形を構成するインデックスを取得
			const UINT *idx = reinterpret_cast<const UINT *>(In_Data.idx);

			// 1面(三頂点、三角形)ずつ処理
			for (UINT i = 0; i < In_Data.idxNum; i += 3)
			{
				// 三角形を構成する頂点情報の取得
				UINT idx0 = idx[i + 0];
				UINT idx1 = idx[i + 1];
				UINT idx2 = idx[i + 2];
				// 頂点
				DirectX::XMFLOAT3 p[] = {
					destVtx[idx0].pos,
					destVtx[idx1].pos,
					destVtx[idx2].pos
				};
				// UV
				DirectX::XMFLOAT2 uv[] = {
					destVtx[idx0].uv,
					destVtx[idx1].uv,
					destVtx[idx2].uv
				};
				// 頂点同士を結ぶベクトル
				DirectX::XMFLOAT3 V[] = {
					{p[1].x - p[0].x, p[1].y - p[0].y, p[1].z - p[0].z},
					{p[2].x - p[0].x, p[2].y - p[0].y, p[2].z - p[0].z}
				};
				// UV同士を結ぶベクトル
				DirectX::XMFLOAT2 ST[] = {
					{uv[1].x - uv[0].x, uv[1].y - uv[0].y},
					{uv[2].x - uv[0].x, uv[2].y - uv[0].y}
				};

				// --- 接ベクトルの計算
				float factor = ST[0].x * ST[1].y - ST[0].y * ST[1].x;
				DirectX::XMFLOAT3 T(
					(ST[1].y * V[0].x - ST[0].y * V[1].x) / factor,
					(ST[1].y * V[0].y - ST[0].y * V[1].y) / factor,
					(ST[1].y * V[0].z - ST[0].y * V[1].z) / factor
				);

				// 接ベクトルの正規化
				float length = sqrtf(T.x * T.x + T.y * T.y + T.z * T.z);
				T.x /= length;
				T.y /= length;
				T.z /= length;
				// 頂点に接ベクトルの情報を追加
				tangentVtx[idx0].push_back(T);
				tangentVtx[idx1].push_back(T);
				tangentVtx[idx2].push_back(T);
			}

			// 頂点に割り当てられた接ベクトルの平均を求める
			for (UINT i = 0; i < In_Data.vtxNum; ++i)
			{
				DirectX::XMFLOAT3 total(0.0f, 0.0f, 0.0f);
				for (const auto &itr : tangentVtx[i])
				{
					total.x += itr.x;
					total.y += itr.y;
					total.z += itr.z;
				}
				// 合計から平均を求める
				total.x /= tangentVtx[i].size();
				total.y /= tangentVtx[i].size();
				total.z /= tangentVtx[i].size();
				// 平均結果を該当頂点接ベクトルとする
				destVtx[i].tangent = total;
			}
		};

	GameObject *pInstanced = CreateObject<GameObject>("Instanced");
	pInstanced->SetPos({ 0.0f,0.0f,0.0f });
	pInstanced->SetScale({ 1.0f,1.0f,1.0f });
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Assets/Model/plane/plane.fbx");
	InstancedComp->SetCamera(pCamera);
	InstancedComp->SetVertexShader(ShaderM.GetShader("IVS_InstancedObject"));
	InstancedComp->SetPixelShader(ShaderM.GetShader("PS_POM"));
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 100;
	instanceData.CountZ = 100;
	instanceData.CountY = 1;
	instanceData.StartPos = pInstanced->GetPos();
	instanceData.Scale = pInstanced->GetScale();
	instanceData.Quaternion = pInstanced->GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };

	InstancedComp->SetAlignInstanceData(instanceData);
	InstancedComp->IsEnablePS_WriteCamera(true);
	InstancedComp->IsEnablePS_WriteParam(ResourceSetting::ShaderParam_Light, true);
	InstancedComp->IsEnablePS_WriteParam(ResourceSetting::ShaderParam_PBR, true);
	InstancedComp->IsEnablePS_WriteParam(ResourceSetting::ShaderParam_POM, true);

	InstancedComp->ExecuteUpdate();

	pInstanced-

	// テクスチャの設定
	Texture *normal = CreateObject<Texture>("Normal");
	normal->Create("Assets/Model/plane/normal.png");
	Texture *height = CreateObject<Texture>("Height");
	height->Create("Assets/Model/plane/height.png");
	InstancedComp->GetMesh(0)->GetMaterial()->SetTexture(ResourceSetting::TextureType_Normal, std::shared_ptr<Texture>(normal));
	InstancedComp->GetMesh(0)->GetMaterial()->SetTexture(ResourceSetting::TextureType_Height, std::shared_ptr<Texture>(height));
}

void SceneRoot::Uninit()
{
	SpriteManager::GetInstance().DeleteAll();
}

void SceneRoot::Update()
{
	ResourceSetting::LightParam light;
	light.Direction = { 0.0f, -1.0f, 0.0f };
	light.Dummy = 0.0f;
	light.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	light.Specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	ResourceSetting::PBR_Param pbr;
	pbr.Metallic = 0.5f;
	pbr.Smooth = 0.5f;
	pbr.dummy = { 0.0f,0.0f };
	ResourceSetting::POM_Param pom;
	pom.HeightScale = 0.01;
	pom.NumSteps = 100;
	pom.dummy = { 0.0f,0.0f };
	ResourceSetting::LightParam lights[] = { 
		light 
	};
	ResourceSetting::PBR_Param pbrs[] = {
		pbr
	};
	ResourceSetting::POM_Param poms[] = {
		pom
	};
	auto *LightParam = ResourceSetting::CreateShaderParam(lights,_countof(lights));
	auto *PBRParam = ResourceSetting::CreateShaderParam(pbrs, _countof(pbrs));
	auto *POMParam = ResourceSetting::CreateShaderParam(poms, _countof(poms));

	auto pModel = GetObject<GameObject>("Instanced");
	auto Component = pModel->GetComponent<InstancedModelRenderer>();
	Component->SetWriteParam(LightParam);
	Component->SetWriteParam(PBRParam);
	Component->SetWriteParam(POMParam);
}

void SceneRoot::Draw()
{
#ifdef _DEBUG
	Geometory &geometory = Geometory::GetInstance();
	Camera *pCamera = GetObject<CameraDCC>("Camera")->GetComponent<Camera>();

	geometory.SetView(pCamera->GetView());
	geometory.SetProjection(pCamera->GetProj());
	pCamera->Draw();

	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, DirectX::XMMatrixIdentity());
	geometory.SetWorld(fmat);

	// 網掛け描画
	float GridSize = cx_fGridSize;
	GridSize *= 0.5f;
	float GridMargin = cx_fGridMargin;
	DirectX::XMFLOAT4 color;
	color = cx_GridColor;
	geometory.SetColor(color);
	float d = GridMargin;
	float s = GridSize;
	for (;s >= GridMargin && GridMargin > 0.0f;d += GridMargin,s -= GridMargin)
	{
		geometory.AddLine(DirectX::XMFLOAT3(d, 0.0f, -GridSize), DirectX::XMFLOAT3(d, 0.0f, GridSize));
		geometory.AddLine(DirectX::XMFLOAT3(-d, 0.0f, -GridSize), DirectX::XMFLOAT3(-d, 0.0f, GridSize));
		geometory.AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, d), DirectX::XMFLOAT3(GridSize, 0.0f, d));
		geometory.AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, -d), DirectX::XMFLOAT3(GridSize, 0.0f, -d));
	}
	// 軸描画
	if (cx_bGridAxis)
	{
		geometory.SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		geometory.AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(GridSize, 0.0f, 0.0f));
		geometory.SetColor(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		geometory.AddLine(DirectX::XMFLOAT3(0.0f, -GridSize, 0.0f), DirectX::XMFLOAT3(0.0f, GridSize, 0.0f));
		geometory.SetColor(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		geometory.AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, GridSize));
	}
	else
	{
		geometory.AddLine(DirectX::XMFLOAT3(-GridSize, 0.0f, 0.0f), DirectX::XMFLOAT3(GridSize, 0.0f, 0.0f));
		geometory.AddLine(DirectX::XMFLOAT3(0.0f, 0.0f, -GridSize), DirectX::XMFLOAT3(0.0f, 0.0f, GridSize));
	}
	geometory.DrawLines();

#endif
}

void SceneRoot::ChangeScene()
{
}
