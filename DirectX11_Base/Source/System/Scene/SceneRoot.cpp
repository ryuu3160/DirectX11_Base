/*+===================================================================
	File: SceneRoot.cpp
	Summary: ルートシーン
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DirectX11/System/RenderManager.hpp"
#include "DirectX11/ResourceManager/ShaderManager.hpp"
#include "DirectX11/Resource/Shaders/ShaderParam.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "DirectX11/Renderer/InstancedModelRenderer.hpp"
#include "DirectX11/Renderer/RTSpriteRenderer.hpp"
#include "SceneRoot.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
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
	CameraDCC *pCamera = GetObject<CameraDCC>("MainCamera");
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
	Component1->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component1->SetPixelShader(ShaderM.GetShader("PS_TexColor"));

	pModel->SetPosition({ 0.0f, 1.0f, 0.0f });

	auto child = pModel->AddChildObject<GameObject>("RootModel0Child");
	auto comp = child->AddComponent<ModelRenderer>();
	comp->SetAssetPath("Assets/Model/spot/spot.fbx");
	comp->SetVertexShader(ShaderM.GetShader("VS_Object"));
	comp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	child->SetPosition({ 1.0f, 0.0f, 0.0f });

	// F15Eのモデルを読み込む
	GameObject *pModel2 = GetObject<GameObject>("RootModel1");
	auto Component2 = pModel2->GetComponent<ModelRenderer>();
	Component2->SetAssetPath("Assets/Model/F15E.fbx");
	Component2->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component2->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	Component2->IsUseMaterialShader(true); // マテリアルシェーダーを使用する

	pModel2->SetPosition({ -2.0f, 0.0f, 0.0f });
	pModel2->SetScale({ 0.005f, 0.005f, 0.005f });

	// スカイボックスを作成
	SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	pSkyBox->SetCamera(pCamera);

	// インスタンシングテスト
	/*GameObject *pInstanced = CreateObject<GameObject>("Instanced");
	pInstanced->SetPosition({ 0.0f,0.0f,0.0f });
	pInstanced->SetScale({ 1.0f,1.0f,1.0f });
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Assets/Model/plane/plane.fbx");
	InstancedComp->SetVertexShader(ShaderM.GetShader("IVS_InstancedObject"));
	InstancedComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 200;
	instanceData.CountZ = 200;
	instanceData.CountY = 1;
	instanceData.StartPos = pInstanced->GetPosition();
	instanceData.Scale = pInstanced->GetScale();
	instanceData.Quaternion = pInstanced->GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };
	instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

	InstancedComp->SetAlignInstanceData(instanceData);*/

	// パターンスケールテスト
	GameObject *pPatternScale = CreateObject<GameObject>("PatternScale");
	auto PatternScaleComp = pPatternScale->AddComponent<ModelRenderer>();
	PatternScaleComp->SetAssetPath("Assets/Model/plane/plane.fbx");
	PatternScaleComp->SetVertexShader(ShaderM.GetShader("VS_Object"));
	PatternScaleComp->SetPixelShader(ShaderM.GetShader("PS_PatternScale"));
	pPatternScale->SetPosition({ 0.0f, -0.5f, 0.0f });
	pPatternScale->SetScale({ 1000.0f, 1.0f, 1000.0f });

	FadeManager::GetInstance().AddFade("TestFade", 5.0f, Ease::EasingType::MAX, true);
	FadeManager::GetInstance().StartFadeIn("TestFade");

	// レンダーターゲットスプライトテスト
	auto RTV = RenderTargetManager::GetInstance().CreateRenderTarget("GameRTV", DXGI_FORMAT_R8G8B8A8_UNORM, 1280, 720);
	auto DSV = RenderTargetManager::GetInstance().CreateDepthStencil("GameDSV", 1280, 720,false);

	CameraBaseObj *pGameCam = CreateObject<CameraBaseObj>("GameCamera");
	auto GameCamCmp = pGameCam->GetComponent<Camera>();
	pGameCam->SetPosition({ 0.0f,5.0f,0.0f });
	pGameCam->SetRotation({ 90.0f,180.0f,0.0f});
	auto RenderCtx = RenderManager::GetInstance().CreateRenderContext("Game", GameCamCmp, RTV, DSV);

	GameObject *pRTSprite = CreateObject<GameObject>("RTSprite");
	auto RTSpriteCmp = pRTSprite->AddComponent<RTSpriteRenderer>();
	RTSpriteCmp->SetRenderContext(RenderCtx);
	pRTSprite->SetPosition({ 5.0f,2.0f,0.0f });
	pRTSprite->SetScale({ 5.0f,5.0f,1.0f });
}

void SceneRoot::Uninit()
{
}

void SceneRoot::Update()
{
	struct PaternScale
	{
		DirectX::XMFLOAT2 scale;
		DirectX::XMFLOAT2 dummy;
	};
	PaternScale ps;
	ps.scale = { 1000.0f,1000.0f };
	ps.dummy = { 0.0f,0.0f };
	std::shared_ptr<ShaderParam> pPatternScaleParam = std::make_shared<ShaderParam>("PatternScale", 0, &ps, 1);
	auto pPatternScale = GetObject<GameObject>("PatternScale");
	auto PatternScaleComp = pPatternScale->GetComponent<ModelRenderer>();
	PatternScaleComp->SetWriteParam(pPatternScaleParam);

	//ResourceSetting::PBR_Param pbr;
	//pbr.Metallic = 0.8f;
	//pbr.Smooth = 0.3f;
	//pbr.dummy = { 0.0f,0.0f };
	//ResourceSetting::POM_Param pom;
	//pom.HeightScale = 0.1f;
	//pom.NumSteps = 100;
	//pom.dummy = {};
	//ResourceSetting::PBR_Param pbrs[] = {
	//	pbr
	//};
	//ResourceSetting::POM_Param poms[] = {
	//	pom
	//};
	//auto *LightParam = ResourceSetting::CreateShaderParam(lights,std::size(lights));
	//auto *PBRParam = ResourceSetting::CreateShaderParam(pbrs, std::size(pbrs));
	//auto *POMParam = ResourceSetting::CreateShaderParam(poms, std::size(poms));

	auto pModel = GetObject<GameObject>("Instanced");
	/*auto Component = pModel->GetComponent<InstancedModelRenderer>();
	Component->SetWriteParam(LightParam);
	Component->SetWriteParam(PBRParam);
	Component->SetWriteParam(POMParam);*/
	//pModel->SetPosition(pModel->GetPosition() + DirectX::XMFLOAT3(0.0f, 0.0f, 0.01f));
}

void SceneRoot::Draw()
{
#ifdef _DEBUG
	Geometory &geometory = Geometory::GetInstance();
	Camera *pCamera = GetObject<CameraDCC>("MainCamera")->GetComponent<Camera>();

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
