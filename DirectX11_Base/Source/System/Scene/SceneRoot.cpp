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
#include "DirectX11/Resource/TextureManager.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "DirectX11/System/InstancedModelRenderer.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/Mesh.hpp"

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
	GameObject *pInstanced = CreateObject<GameObject>("Instanced");
	pInstanced->SetPos({ 0.0f,0.0f,0.0f });
	pInstanced->SetScale({ 1.0f,1.0f,1.0f });
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Assets/Model/plane/plane.fbx");
	InstancedComp->SetCamera(pCamera);
	InstancedComp->SetVertexShader(ShaderM.GetShader("IVS_InstancedObject"));
	InstancedComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 1000;
	instanceData.CountZ = 1000;
	instanceData.CountY = 1;
	instanceData.StartPos = pInstanced->GetPos();
	instanceData.Scale = pInstanced->GetScale();
	instanceData.Quaternion = pInstanced->GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };
	instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

	InstancedComp->SetAlignInstanceData(instanceData);
}

void SceneRoot::Uninit()
{
	SpriteManager::GetInstance().DeleteAll();
}

void SceneRoot::Update()
{
	/*ResourceSetting::LightParam light;
	light.Direction = GetObject<CameraDCC>("Camera")->GetFront();
	light.Dummy = 0.0f;
	light.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	light.Ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	ResourceSetting::PBR_Param pbr;
	pbr.Metallic = 0.8f;
	pbr.Smooth = 0.3f;
	pbr.dummy = { 0.0f,0.0f };
	ResourceSetting::POM_Param pom;
	pom.HeightScale = 0.1f;
	pom.NumSteps = 100;
	pom.dummy = {};
	ResourceSetting::LightParam lights[] = { 
		light 
	};
	ResourceSetting::PBR_Param pbrs[] = {
		pbr
	};
	ResourceSetting::POM_Param poms[] = {
		pom
	};
	auto *LightParam = ResourceSetting::CreateShaderParam(lights,std::size(lights));
	auto *PBRParam = ResourceSetting::CreateShaderParam(pbrs, std::size(pbrs));
	auto *POMParam = ResourceSetting::CreateShaderParam(poms, std::size(poms));

	auto pModel = GetObject<GameObject>("Instanced");*/
	/*auto Component = pModel->GetComponent<InstancedModelRenderer>();
	Component->SetWriteParam(LightParam);
	Component->SetWriteParam(PBRParam);
	Component->SetWriteParam(POMParam);*/
	//pModel->SetPos(pModel->GetPos() + DirectX::XMFLOAT3(0.0f, 0.0f, 0.01f));
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
