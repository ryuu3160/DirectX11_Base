/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/24 Thu AM 01:09:49 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"
#include "System/Component/Camera.hpp"

SpriteRenderer::SpriteRenderer() : m_bIsLoaded(false)
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
		MakeDefaultTexture();
	}
}

SpriteRenderer::~SpriteRenderer()
{
	if (m_SpriteData.texture)
	{
		delete m_SpriteData.texture; // テクスチャの解放
		m_SpriteData.texture = nullptr; // nullptrに設定
	}
	if(m_SpriteData.vs)
	{
		m_SpriteData.vs = nullptr;
	}
	if(m_SpriteData.ps)
	{
		m_SpriteData.ps = nullptr;
	}
}

void SpriteRenderer::ExecuteUpdate() noexcept
{
	// 既にメッシュが読み込まれている場合は何もしない
	if (m_bIsLoaded)
		return;

	this->Load(m_AssetPath, 1.0f); // デフォルトのスケール1.0fで読み込み
	m_bIsLoaded = true; // 読み込み完了フラグを立てる
}

void SpriteRenderer::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	if (NullCheck(In_Vs, NCMode::OUTPUT, "VertexShader is null in SpriteRenderer::SetVertexShader."))
		return;
	m_SpriteData.vs = In_Vs;
}

void SpriteRenderer::SetPixelShader(_In_ Shader *In_Ps) noexcept
{
	if (NullCheck(In_Ps, NCMode::OUTPUT, "PixelShader is null in SpriteRenderer::SetPixelShader."))
		return;
	m_SpriteData.ps = In_Ps;
}

void SpriteRenderer::ReadWrite(_In_ DataAccessor *In_Data)
{
	In_Data->Access<FilePath>(&m_AssetPath);
}

void SpriteRenderer::Load(_In_ const FilePath &In_File, _In_ const float &In_Scale)
{
	// 頂点データの作成
	SpriteVertex vtx[] = {
		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},
		{DirectX::XMFLOAT3(0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
	};

	// メッシュ
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx;
	desc.vtxSize = sizeof(SpriteVertex);
	desc.vtxCount = _countof(vtx);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_SpriteData.mesh = std::make_shared<MeshBuffer>(desc);

	// パラメーター
	m_SpriteData.param[0] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f * In_Scale, 1.0f * In_Scale);
	m_SpriteData.param[1] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_SpriteData.param[2] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&m_SpriteData.matrix[0], DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_SpriteData.matrix[1], DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_SpriteData.matrix[2], DirectX::XMMatrixIdentity());

	// シェーダー
	m_SpriteData.vs = m_defVS.get();
	m_SpriteData.ps = m_defPS.get();

	// テクスチャの設定
	m_SpriteData.texture = new Texture();
	if (FAILED(m_SpriteData.texture->Create(In_File.data())))
	{
		// テクスチャの読み込みに失敗した場合は白いテクスチャを使用
		m_SpriteData.texture = m_DefaultTex.get();
	}
}

void SpriteRenderer::Load() noexcept
{
	// 既に読み込まれている場合は何もしない
	if (m_bIsLoaded)
		return;
	this->Load(m_AssetPath, 1.0f); // デフォルトのスケール1.0fで読み込み
	m_bIsLoaded = true; // 読み込み完了フラグを立てる
}

void SpriteRenderer::Draw() noexcept
{
	// 2D描画の準備
	Main::Change2D_Draw();

	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX BillBoard = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_pTransform->GetPos().x, m_pTransform->GetPos().y, m_pTransform->GetPos().z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_pTransform->GetScale().x, m_pTransform->GetScale().y, m_pTransform->GetScale().z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(m_pTransform->GetRotation().x, m_pTransform->GetRotation().y, m_pTransform->GetRotation().z);

	// ビュー行列とプロジェクション行列取得
	bool prev = m_pViewCamera->Is3D();
	if (m_SpriteData.Is3D)
	{
		m_SpriteData.matrix[1] = m_pViewCamera->GetView(); // 3Dカメラのビュー行列を取得

		m_pViewCamera->Set3D(true); // 3Dカメラに設定
		m_SpriteData.matrix[2] = m_pViewCamera->GetProj();
	}
	else
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
		m_SpriteData.matrix[1] = view; // 2Dカメラのビュー行列を設定

		m_pViewCamera->Set3D(false); // 2Dカメラに設定
		m_SpriteData.matrix[2] = m_pViewCamera->GetProj();
	}
	m_pViewCamera->Set3D(prev); // 元のカメラモードに戻す

	// ビルボードかそうでないかで処理を分ける(3D空間に配置されている時限定)
	if (m_SpriteData.IsBillBoard && m_SpriteData.Is3D)
	{
		DirectX::XMFLOAT4X4 BillBoardView = m_pViewCamera->GetView(false);

		// 計算用の型に置き換え
		BillBoard = DirectX::XMLoadFloat4x4(&BillBoardView);
		// 逆行列を求める
		BillBoard = DirectX::XMMatrixInverse(nullptr, BillBoard);
		// 一度、mCamInvをviewに変換
		DirectX::XMStoreFloat4x4(&BillBoardView, BillBoard);
		// 逆行列の座標を0にする
		BillBoardView._41 = BillBoardView._42 = BillBoardView._43 = 0.0f;
		// World行列と計算を行うため、再度veiwをmCamInvに変換
		BillBoard = DirectX::XMLoadFloat4x4(&BillBoardView);

		mWorld = BillBoard * S * T;
		// mWorldの転置行列を計算
		mWorld = DirectX::XMMatrixTranspose(mWorld);

		// スプライトに設定するWorld行列を用意
		DirectX::XMStoreFloat4x4(&world, mWorld);
	}
	else
	{
		// ビルボードではない時は回転も掛ける
		mWorld = R * S * T;
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));
	}

	m_SpriteData.matrix[0] = world;

	m_SpriteData.vs->WriteBuffer(0, m_SpriteData.matrix);
	m_SpriteData.vs->WriteBuffer(1, m_SpriteData.param);
	m_SpriteData.vs->Bind();
	m_SpriteData.ps->SetTexture(0, m_SpriteData.texture);
	m_SpriteData.ps->Bind();
	m_SpriteData.mesh->Draw();

	// 3D描画の設定に戻す
	Main::Change3D_Draw();
}

void SpriteRenderer::SetOffset(_In_ const DirectX::XMFLOAT2 &In_Offset) noexcept
{
	m_SpriteData.param[0].x = In_Offset.x;
	m_SpriteData.param[0].y = In_Offset.y;
}
void SpriteRenderer::SetSize(_In_ const DirectX::XMFLOAT2 &In_Size) noexcept
{
	m_SpriteData.param[0].z = In_Size.x / (static_cast<float>(cx_nWINDOW_WIDTH) / 2.0f / 10.0f);
	m_SpriteData.param[0].w = In_Size.y / (static_cast<float>(cx_nWINDOW_HEIGHT) / 2.0f / 5.6f);
}

void SpriteRenderer::SetUVPos(_In_ const DirectX::XMFLOAT2 &In_Pos) noexcept
{
	m_SpriteData.param[1].x = In_Pos.x;
	m_SpriteData.param[1].y = In_Pos.y;
}
void SpriteRenderer::SetUVScale(_In_ const DirectX::XMFLOAT2 &In_Scale) noexcept
{
	m_SpriteData.param[1].x = In_Scale.x;
	m_SpriteData.param[1].y = In_Scale.y;
}
void SpriteRenderer::SetColor(_In_ const DirectX::XMFLOAT4 &In_Color) noexcept
{
	m_SpriteData.param[2] = In_Color;
}
void SpriteRenderer::SetTexture(_In_ Texture *In_pTex) noexcept
{
	m_SpriteData.texture = In_pTex ? In_pTex : m_DefaultTex.get();
}

void SpriteRenderer::Set3D(_In_ const bool &In_Is3D) noexcept
{
	m_SpriteData.Is3D = In_Is3D;
}
void SpriteRenderer::SetBillBoard(_In_ const bool &In_IsBillBoard) noexcept
{
	m_SpriteData.IsBillBoard = In_IsBillBoard;
}

void SpriteRenderer::MakeDefaultShader() noexcept
{
	m_defVS = std::make_shared<VertexShader>();
	m_defVS->Load("Assets/Shader/VS_Sprite.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Sprite.cso");
}

void SpriteRenderer::MakeDefaultTexture() noexcept
{
	BYTE color[] = { 255,255,255,255 };
	m_DefaultTex = std::make_shared<Texture>();
	m_DefaultTex->Create(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, color);
}
