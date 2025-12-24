/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: スプライトの描画コンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/24 Thu AM 01:09:49 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"
#include "System/Component/Camera.hpp"
#include "DirectX11/ResourceManager/TextureManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
	const inline constexpr float cx_fScreenWidthCorrect = 10.0f; // スクリーンの幅補正値
	const inline constexpr float cx_fScreenHeightCorrect = 5.6f; // スクリーンの高さ補正値
}

SpriteRenderer::SpriteRenderer()
	: RenderComponent("SpriteRenderer")
	, m_bIsLoaded(false), m_bUsePixelPosition(false), m_PositionPixel{}
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
		MakeDefaultTexture();
	}
	GetPositionPixel();
}

SpriteRenderer::~SpriteRenderer()
{
	if (m_SpriteData.texture)
	{
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

void SpriteRenderer::Update(_In_ float In_Tick) noexcept
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
	m_SpriteData.texture = TextureManager::GetInstance().LoadTexture(In_File).get();
	if (!m_SpriteData.texture)
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

void SpriteRenderer::Draw(_In_ RenderContext *In_RenderContext) noexcept
{
	// 2D描画の準備
	auto &DX11 = DX11_Core::GetInstance();
	DX11.Change2DMode(In_RenderContext->GetRTV());

	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX BillBoard = DirectX::XMMatrixIdentity();

	// ワールド行列計算
	auto Pos = m_pGameObject->GetPosition();
	auto Scale = m_pGameObject->GetScale();
	auto Quat = m_pGameObject->GetQuat();

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&Quat));

	// ビュー行列とプロジェクション行列取得
	if (m_SpriteData.Is3D)
	{
		// 3Dカメラのビュー行列
		m_SpriteData.matrix[1] = In_RenderContext->GetView();
		m_SpriteData.matrix[2] = In_RenderContext->GetProj();
	}
	else
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
		m_SpriteData.matrix[1] = view; // 2Dカメラのビュー行列を設定

		m_SpriteData.matrix[2] = In_RenderContext->Get2DProj();
	}

	// ビルボードかそうでないかで処理を分ける(3D空間に配置されている時限定)
	if (m_SpriteData.IsBillBoard && m_SpriteData.Is3D)
	{
		DirectX::XMFLOAT4X4 BillBoardView = In_RenderContext->GetView(false);

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
	DX11.Change3DMode(In_RenderContext->GetRTV(), In_RenderContext->GetDSV());
}

const DirectX::XMFLOAT3 &SpriteRenderer::GetPositionPixel() noexcept
{
	if(!m_pGameObject)
		return m_PositionPixel = { 0.0f,0.0f,0.0f }; // Transformが設定されていない場合は空の値を返す

	m_PositionPixel = m_pGameObject->GetPosition();

	// x軸とy軸をピクセル単位に変換
	m_PositionPixel.x = m_PositionPixel.x * (static_cast<float>(cx_nWINDOW_WIDTH) / 2.0f / cx_fScreenWidthCorrect);
	m_PositionPixel.y = m_PositionPixel.y * (static_cast<float>(cx_nWINDOW_HEIGHT) / 2.0f / cx_fScreenHeightCorrect);
	return m_PositionPixel;
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

void SpriteRenderer::SetPositionPixel(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	DirectX::XMFLOAT3 pos = m_PositionPixel = In_Pos; // ピクセル単位の位置を保存
	pos.x = pos.x / (static_cast<float>(cx_nWINDOW_WIDTH) / 2.0f / cx_fScreenWidthCorrect);
	pos.y = pos.y / (static_cast<float>(cx_nWINDOW_HEIGHT) / 2.0f / cx_fScreenHeightCorrect);
	m_pGameObject->SetPosition(pos);
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
