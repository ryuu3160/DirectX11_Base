/*+===================================================================
	File: SceneRoot.cpp
	Summary: ルートシーン
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneRoot.hpp"
#include "Engine.hpp"

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
	CameraDCC *pCamera = GetObject<CameraDCC>("EditorCamera");
	auto pCameraComp = pCamera->GetComponent<Camera>();

	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(pCameraComp);
	SpriteManager::GetInstance().SetCameraObject(pCamera);

	Setup(2);

	auto &ShaderM = ShaderManager::GetInstance();

	// オブジェクトの設定
	GameObject *pModel = GetObject<GameObject>("RootModel0");
	auto Component1 = pModel->GetComponent<ModelRenderer>();
	Component1->SetAssetPath("Engine/Assets/Model/spot/spot.fbx");
	Component1->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component1->SetPixelShader(ShaderM.GetShader("PS_TexColor"));

	//pModel->SetPosition({0.0f, 1.0f, 0.0f});
	pModel->AddComponent<SphereCollider>();

	auto child = CreateObject<GameObject>("RootModel0Child",pModel->GetTransform());
	auto comp = child->AddComponent<ModelRenderer>();
	comp->SetAssetPath("Engine/Assets/Model/spot/spot.fbx");
	comp->SetVertexShader(ShaderM.GetShader("VS_Object"));
	comp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	child->SetPosition({ 1.0f, 0.0f, 0.0f });

	// F15Eのモデルを読み込む
	GameObject *pModel2 = GetObject<GameObject>("RootModel1");
	auto Component2 = pModel2->GetComponent<ModelRenderer>();
	Component2->SetAssetPath("Engine/Assets/Model/F15E.fbx");
	Component2->SetVertexShader(ShaderM.GetShader("VS_Object"));
	Component2->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	Component2->IsUseMaterialShader(false); // マテリアルシェーダーを使用する
	Component2->SetScale(0.005f);
	pModel2->AddComponent<CapsuleCollider>();

	// F15Eの移動処理
	pModel2->AddComponent<MovementComponent>();
	//auto input = pModel2->AddComponent<PlayerController>();
	/*input->RegisterKeyCallBack('W', InputSystem::KeyState::Press, [pModel2]() {
		auto pos = pModel2->GetPosition();
		pos.z += 0.1f;
		pModel2->SetPosition(pos);
		});
	input->RegisterKeyCallBack('S', InputSystem::KeyState::Press, [pModel2]()
		{
			auto pos = pModel2->GetPosition();
			pos.z -= 0.1f;
			pModel2->SetPosition(pos);
		});
	input->RegisterKeyCallBack('A', InputSystem::KeyState::Press, [pModel2]()
		{
			auto pos = pModel2->GetPosition();
			pos.x -= 0.1f;
			pModel2->SetPosition(pos);
		});
	input->RegisterKeyCallBack('D', InputSystem::KeyState::Press, [pModel2]()
		{
			auto pos = pModel2->GetPosition();
			pos.x += 0.1f;
			pModel2->SetPosition(pos);
		});
	input->RegisterKeyCallBack('E', InputSystem::KeyState::Press, [pModel2]()
		{
			pModel2->Rotate(1.0f, 0.0f, 0.0f);
		});
	input->RegisterKeyCallBack('Q', InputSystem::KeyState::Press, [pModel2]()
		{
			pModel2->Rotate(-1.0f, 0.0f, 0.0f);
		});*/

	//pModel2->SetPosition({ -2.0f, 0.0f, 0.0f });
	pModel2->SetScale({ 1.0f, 1.0f, 1.0f });

	// パーティクルテスト
	auto ParticleObj = CreateObject<GameObject>("ParticleTest");
	auto ParticleCmp = ParticleObj->AddComponent<ParticleEffect>();
	ParticleObj->AddComponent<BoxCollider>();
	ParticleCmp->SetTexture("Engine/Assets/Texture/TestTexture.png");

	// エミッター設定
	EmitterSettings settings;
	settings.EmitRate = 100.0f;              // 1秒間に100個放出
	settings.MaxParticles = 1000;             // 最大500個
	settings.Duration = 2.0f;                // 2秒間だけ放出
	settings.IsLooping = true;              // ループしない

	// 速度の範囲
	settings.VelocityMin = DirectX::XMFLOAT3(-3, -3, -3);
	settings.VelocityMax = DirectX::XMFLOAT3(3, 3, 3);

	// 色の変化 (オレンジ → 透明な赤)
	settings.ColorStart = DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);
	settings.ColorEnd = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);

	// サイズの変化
	settings.SizeStart = DirectX::XMFLOAT2(1.0f, 1.0f);
	settings.SizeEnd = DirectX::XMFLOAT2(0.2f, 0.2f);

	// 寿命
	settings.LifeTimeMin = 0.5f;
	settings.LifeTimeMax = 1.5f;

	// 重力 (上に吹き上がる)
	settings.GravityScale = -0.3f;

	// 放出形状 (球状)
	settings.EmitShape = EmitterSettings::Shape::Sphere;
	settings.ShapeRadius = 0.5f;

	ParticleCmp->AddEmitter(settings);
	ParticleCmp->Play();
	

	// スカイボックスを作成
	SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	pSkyBox->SetCamera(pCamera);

	// インスタンシングテスト
	/*GameObject *pInstanced = CreateObject<GameObject>("Instanced");
	pInstanced->SetPosition({ 0.0f,0.0f,0.0f });
	pInstanced->SetScale({ 1.0f,1.0f,1.0f });
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Engine/Assets/Model/plane/plane.fbx");
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
	PatternScaleComp->SetAssetPath("Engine/Assets/Model/plane/plane.fbx");
	PatternScaleComp->SetVertexShader(ShaderM.GetShader("VS_Object"));
	PatternScaleComp->SetPixelShader(ShaderM.GetShader("PS_PatternScale"));
	pPatternScale->SetPosition({ 0.0f, -0.5f, 0.0f });
	pPatternScale->SetScale({ 1000.0f, 1.0f, 1000.0f });

	// レンダーターゲットスプライトテスト
	auto RTV = RenderTargetManager::GetInstance().CreateRenderTarget("GameRTV", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1280, 720);
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

	FadeManager::GetInstance().AddFade("TestFade", 5.0f, Ease::EasingType::MAX, true);
	FadeManager::GetInstance().StartFadeIn("TestFade");
}

void SceneRoot::Uninit()
{
}

void SceneRoot::Update(_In_ float In_DeltaTime)
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
	PatternScaleComp->SetWriteParamForPS(pPatternScaleParam);

	//GetObject<GameObject>("ParticleTest")->GetComponent<ParticleEffect>()->Play();

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
	Component->SetWriteParamForPS(LightParam);
	Component->SetWriteParamForPS(PBRParam);
	Component->SetWriteParamForPS(POMParam);*/
	//pModel->SetPosition(pModel->GetPosition() + DirectX::XMFLOAT3(0.0f, 0.0f, 0.01f));
}

void SceneRoot::Draw()
{
}

void SceneRoot::ChangeScene()
{
}
