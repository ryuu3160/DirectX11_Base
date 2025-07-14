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
#include "System/Object/CameraDCC.hpp"
#include "DirectX11/System/SpriteManager.hpp"

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


	const char *file[] = {
		"VS_Object",
		"PS_TexColor",	// テクスチャ貼っただけ
	};
	Setup(file, _countof(file), 1);
	GameObject *pModel = GetObject<GameObject>("RootModel0");
	pModel->GetComponent<ModelRenderer>()->SetModelPath("Assets/Model/spot/spot.fbx");

	auto &Instance = SpriteManager::GetInstance();
	auto sprite = Instance.CreateSprite("TestSprite", "Assets/Texture/TestTexture.png");
	sprite->Set3D(true);
	sprite->SetBillBoard(true);
}

void SceneRoot::Uninit()
{
	SpriteManager::GetInstance().DeleteAll();
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

	// ゲーム内のオブジェクトの取得
	GameObject *pModel[] = {
		// GetObj - 引数の名前で指定された
		// ヒエラルキー内のオブジェクトを取得
		GetObject<GameObject>("RootModel0")
	};
	// カメラオブジェクトの取得
	GameObject *pCameraObj2 = GetObject<GameObject>("Camera");
	// カメラコンポーネントの取得
	Camera *pCamera2 = pCameraObj2->GetComponent<Camera>();

	// 読み込まれたシェーダーファイルの取得
	Shader *pVS = GetObject<Shader>("VS_Object");
	Shader *pPS[] =
	{
		GetObject<Shader>("PS_TexColor"),
	};

	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = pCamera2->GetView(false);
	mat[2] = pCamera2->GetProj(false);

	// カメラの情報を定数バッファで渡す
	DirectX::XMFLOAT3 CamPos = pCameraObj2->GetPos();
	DirectX::XMFLOAT4 CameraParam[] = {
		{CamPos.x,CamPos.y,CamPos.z,0.0f}
	};

	// 描画に必要な奴ら
	ModelRenderer *pRenderer;
	Model *pDrawModel;

	// 描画
	for (int i = 0; i < sizeof(pModel) / sizeof(GameObject *); i++)
	{
		// 単位行列でワールド行列を作成
		mat[0] = pModel[i]->GetWorld(false);
		// カメラのビュー/プロジェクション行列を設定

		// メモリ上の行列をグラフィックスメモリへコピー
		// 1つ目の引数はバッファの番号
		pVS->WriteBuffer(0, mat);

		// オブジェクト内のコンポーネントを取得
		pRenderer = pModel[i]->GetComponent<ModelRenderer>();

		// ModelRenderコンポーネントの内部に
		// DX22で使用していたModelクラスがあり、
		// 描画はModelクラスで行うため取得する
		pDrawModel = pRenderer->GetModel();
		if (pDrawModel)
		{
			// 描画
			pDrawModel->SetVertexShader(pVS);
			pDrawModel->SetPixelShader(pPS[i]);
			pDrawModel->Draw();
		}
	}

	// todo: スプライトクラスをもっと使いやすく改造
	
	// 2D描画の準備
	Main::Change2D_Draw();

	pCamera2->Set3D(false);
	SpriteManager::GetInstance().SetCameraViewAndCameraProj(pCamera2);
	SpriteManager::GetInstance().GetSprite("TestSprite")->SetPosition({ 5.0f,0.0f,0.0f });
	SpriteManager::GetInstance().GetSprite("TestSprite")->SetRotation({ 0.0f,0.0f,90.0f });
	SpriteManager::GetInstance().Draw();

	pCamera2->Set3D(true);
	Main::Change3D_Draw();
}

void SceneRoot::ChangeScene()
{
}
