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
#include "App/GameObject/MainCamera.hpp"
#include "System/SpriteManager/SpriteManager.hpp"
#include "DirectX11/Resource/Mesh.hpp"
#include "DirectX11/Resource/ShaderParam.hpp"

// ==============================
//  定数
// ==============================
namespace
{
	constexpr const float cx_fGridSize = 200.0f; // グリッドのサイズ
	constexpr const float cx_fGridMargin = 1.0f; // グリッドの間隔
	constexpr const DirectX::XMFLOAT4 cx_GridColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // グリッドの色
	constexpr const bool cx_bGridAxis = true; // グリッドの軸を表示するかどうか
}

void SceneRoot::Init()
{
	// オブジェクトの作成
	// カメラオブジェクトの生成
#ifdef _DEBUG
	CameraDCC *pCamera = CreateObject<CameraDCC>("Camera");
#else
	MainCamera *pCamera = CreateObject<MainCamera>("Camera");
#endif
	Camera *pCameraComp = pCamera->GetComponent<Camera>();

	// スプライトマネージャーにカメラを設定
	SpriteManager::GetInstance().SetCamera(pCameraComp);
	SpriteManager::GetInstance().SetCameraObject(pCamera);
}

void SceneRoot::Uninit()
{
}

void SceneRoot::Update()
{

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
