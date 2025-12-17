/*+===================================================================
	File: SceneTitle.cpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/17 Wed AM 02:24:55 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneTitle.hpp"

#include "DirectX11/System/RenderManager.hpp"
#include "DirectX11/ResourceManager/ShaderManager.hpp"
#include "DirectX11/Resource/Shaders/ShaderParam.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "DirectX11/Renderer/InstancedModelRenderer.hpp"
#include "DirectX11/Renderer/RTSpriteRenderer.hpp"
#include "System/Object/CameraDCC.hpp"
#include "System/Object/SkyBoxObj.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "System/Component/InputSystem.hpp"
#include "System/Component/Collider/SphereCollider.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

void SceneTitle::Init()
{
	// ランダムモジュール作成
	m_Random = new Random();
	m_Random->enableMT();

	auto &Sound = SoundManager::GetInstance();
	Sound.Load("TitleBGM", "Assets/Sound/BGM/TitleBGM.mp3", false, true);

	// カメラ取得
#ifdef _DEBUG
	CameraDCC *pCamera = GetObject<CameraDCC>("EditorCamera");
#else
	auto pCamera = GetObject<CameraBaseObj>("MainCamera");
#endif
	auto pCameraComp = pCamera->GetComponent<Camera>();

	// カメラのトランスフォーム設定
	pCamera->SetPosition({ 0.0f,1.5f,-1.0f });

	auto &ShaderM = ShaderManager::GetInstance();

	// シェーダー読み込み
	ShaderM.SetupShader("VS_Wave");
	ShaderM.SetupShader("IVS_Wave");

	// 波のインスタンシングオブジェクト作成x2
	for(int i = 0; i < 2; ++i)
	{
		std::string NumStr = ToString(i);
		GameObject *pWaveInstanced = CreateObject<GameObject>("WaveInstancedObject" + NumStr);

		float FirstPosZ = -2.0f;
		float AddPosZ = 100.0f;
		FirstPosZ += i * AddPosZ;

		pWaveInstanced->SetPosition({ 0.0f,0.0f,FirstPosZ });
		pWaveInstanced->SetScale({ 1.0f,1.0f,1.0f });
		pWaveInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });
		auto WaveInstancedComp = pWaveInstanced->AddComponent<InstancedModelRenderer>();
		WaveInstancedComp->SetAssetPath("Assets/Model/Ground/Ocean.fbx");
		WaveInstancedComp->SetVertexShader(ShaderM.GetShader("IVS_Wave"));
		WaveInstancedComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
		WaveInstancedComp->SetScale(1.0f);
		WaveInstancedComp->IsUseMaterialShader(false);
		InstancedMesh::AlignInstanceData waveInstanceData;
		waveInstanceData.CountX = 100;
		waveInstanceData.CountZ = 100;
		waveInstanceData.CountY = 1;
		waveInstanceData.StartPos = pWaveInstanced->GetPosition();
		waveInstanceData.Scale = pWaveInstanced->GetScale();
		waveInstanceData.Quaternion = pWaveInstanced->GetQuat();
		waveInstanceData.IsWrite = true;
		waveInstanceData.ShiftPosOffset = { 1.0f,0.0f,1.0f };
		waveInstanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Back };

		WaveInstancedComp->SetAlignInstanceData(waveInstanceData);
	}

	// 戦闘機のインスタンシングオブジェクト作成
	GameObject *pInstanced = CreateObject<GameObject>("F15EGroup");
	pInstanced->SetQuat({ 0.0f,0.0f,0.0f,0.0f });

	auto InstancedComp = pInstanced->AddComponent<InstancedModelRenderer>();
	InstancedComp->SetAssetPath("Assets/Model/Character/F15E/F15E.fbx");
	InstancedComp->SetVertexShader(ShaderM.GetShader("IVS_InstancedObject"));
	InstancedComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
	InstancedComp->SetScale(0.0005f);
	InstancedComp->IsUseMaterialShader(false);
	InstancedMesh::AlignInstanceData instanceData;
	instanceData.CountX = 2;
	instanceData.CountZ = 1;
	instanceData.CountY = 1;
	instanceData.StartPos = pInstanced->GetPosition();
	instanceData.Scale = pInstanced->GetScale();
	instanceData.Quaternion = pInstanced->GetQuat();
	instanceData.IsWrite = true;
	instanceData.ShiftPosOffset = { 0.8f,0.0f,1.0f };
	instanceData.AnchorPoint = { InstancedMesh::AnchorX::Center, InstancedMesh::AnchorY::Bottom, InstancedMesh::AnchorZ::Center };

	InstancedComp->SetAlignInstanceData(instanceData);

	// 雲
	for(int i = 0; i < 10; ++i)
	{
		GameObject *pCloud = CreateObject<GameObject>("CloudObject_" + ToString(i));
		float PosX = m_Random->GetDecimalRange(40.0f, -40.0f, 2);
		float PosY = m_Random->GetDecimalRange(5.0f, 0.1f, 2);
		float Scale = m_Random->GetDecimalRange(7.0f, 3.0f, 2);
		pCloud->SetPosition({ PosX ,PosY,60.0f * i });
		pCloud->SetScale({ Scale,Scale,Scale });
		pCloud->SetRotation({ 0.0f,90.0f,0.0f });
		auto CloudComp = pCloud->AddComponent<ModelRenderer>();
		CloudComp->SetAssetPath("Assets/Model/Cloud.fbx");
		CloudComp->SetVertexShader(ShaderM.GetShader("VS_Object"));
		CloudComp->SetPixelShader(ShaderM.GetShader("PS_TexColor"));
		CloudComp->SetScale(1.0f);
		CloudComp->IsUseMaterialShader(true);
	}

	// スカイボックスを作成
	SkyBoxObj *pSkyBox = CreateObject<SkyBoxObj>("SkyBox");
	pSkyBox->SetCamera(pCamera);

	Sound.SetVolume("TitleBGM", 0.5f);
	Sound.Play("TitleBGM");
}

void SceneTitle::Uninit()
{
	SoundManager::GetInstance().Stop("TitleBGM");

	delete m_Random;
	m_Random = nullptr;
}

void SceneTitle::Update(_In_ float In_Tick)
{
	static float Time = 0.0f;
	Time += In_Tick;
	float WaveParam[] = {
		Time,
		0.05f, // 波の振幅
		10.0f,// 波の周期
		1.0// 波の速度
	};
	// 波のインスタンシングオブジェクトにパラメータを設定&座標更新
	for(int i = 0; i < 2; ++i)
	{
		// パラメータ設定
		std::string NumStr = ToString(i);
		auto WaveInstancedObj = GetObject<GameObject>("WaveInstancedObject" + NumStr);
		auto WaveInstancedRenderer = WaveInstancedObj->GetComponent<InstancedModelRenderer>();
		auto WaveParamVS = std::make_shared<ShaderParam>("WaveParam", 1, WaveParam, 4);
		WaveInstancedRenderer->SetWriteParamForVS(WaveParamVS);

		// 座標更新
		float MoveSpeedZ = -1.0f;
		auto Pos = WaveInstancedObj->GetPosition();
		Pos.z += MoveSpeedZ * In_Tick;
		if (Pos.z <= -100.0f)
		{
			Pos.z += 199.0f;
		}
		WaveInstancedObj->SetPosition(Pos);
	}

	// 雲オブジェクトの座標更新
	for(int i = 0; i < 10; ++i)
	{
		auto CloudObj = GetObject<GameObject>("CloudObject_" + ToString(i));
		auto Pos = CloudObj->GetPosition();
		float Scale = CloudObj->GetScale().x;
		float MoveSpeedZ = -20.0f;
		Pos.z += MoveSpeedZ * In_Tick;
		if (Pos.z <= -5.0f)
		{
			Pos.z = 100.0f;
			Pos.x = m_Random->GetDecimalRange(40.0f, -40.0f, 2);
			Pos.y = m_Random->GetDecimalRange(5.0f, 0.1f, 2);
			Scale = m_Random->GetDecimalRange(7.0f, 2.0f, 2);
		}
		CloudObj->SetPosition(Pos);
		CloudObj->SetScale({ Scale,Scale,Scale });
	}

	// 戦闘機を左右にティルトさせる
	auto F15EGroupObj = GetObject<GameObject>("F15EGroup");
	auto F15EGroupRot = F15EGroupObj->GetRotation();
	F15EGroupRot.z += std::sinf(Time) * 5.0f;
	F15EGroupObj->SetRotation(F15EGroupRot);

}

void SceneTitle::Draw()
{
}

void SceneTitle::ChangeScene()
{
}
