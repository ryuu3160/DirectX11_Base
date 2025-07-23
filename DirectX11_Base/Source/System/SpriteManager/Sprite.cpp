/*+===================================================================
	File: Sprite.cpp
	Summary: スプライトクラス 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 05:28:01 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Sprite.hpp"

void Sprite::Draw()
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX BillBoard = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

	// ビルボードかそうでないかで処理を分ける
	if (m_SpriteData.IsBillBoard)
	{
		// 計算用の型に置き換え
		BillBoard = DirectX::XMLoadFloat4x4(&m_BillBoardView);
		// 逆行列を求める
		BillBoard = DirectX::XMMatrixInverse(nullptr, BillBoard);
		// 一度、mCamInvをviewに変換
		DirectX::XMStoreFloat4x4(&m_BillBoardView, BillBoard);
		// 逆行列の座標を0にする
		m_BillBoardView._41 = m_BillBoardView._42 = m_BillBoardView._43 = 0.0f;
		// World行列と計算を行うため、再度veiwをmCamInvに変換
		BillBoard = DirectX::XMLoadFloat4x4(&m_BillBoardView);

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
}

void Sprite::Load(_In_ const FilePath &In_File, _In_ const float &In_Scale)
{
	// 頂点データの作成
	SpriteVertex vtx[] = {
		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
		{DirectX::XMFLOAT3( 0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)},
		{DirectX::XMFLOAT3( 0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
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
	m_defVS = std::make_shared<VertexShader>();
	m_defVS->Load("Assets/Shader/VS_Sprite.cso");
	m_SpriteData.vs = m_defVS.get();
	m_defPS = std::make_shared <PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Sprite.cso");
	m_SpriteData.ps = m_defPS.get();

	// デフォルトテクスチャ
	BYTE color[] = { 255,255,255,255 };
	m_whiteTex = std::make_shared<Texture>();
	m_whiteTex->Create(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, color);

	// テクスチャの設定
	m_SpriteData.texture = new Texture();
	if (FAILED(m_SpriteData.texture->Create(In_File.data())))
	{
		// テクスチャの読み込みに失敗した場合は白いテクスチャを使用
		m_SpriteData.texture = m_whiteTex.get();
	}
}

void Sprite::SetOffset(_In_ const DirectX::XMFLOAT2 &In_Offset) noexcept
{
	m_SpriteData.param[0].x = In_Offset.x;
	m_SpriteData.param[0].y = In_Offset.y;
}
void Sprite::SetSize(_In_ const DirectX::XMFLOAT2 &In_Size) noexcept
{
	m_SpriteData.param[0].z = In_Size.x;
	m_SpriteData.param[0].w = In_Size.y;
}

void Sprite::SetUVPos(_In_ const DirectX::XMFLOAT2 &In_Pos) noexcept
{
	m_SpriteData.param[1].x = In_Pos.x;
	m_SpriteData.param[1].y = In_Pos.y;
}
void Sprite::SetUVScale(_In_ const DirectX::XMFLOAT2 &In_Scale) noexcept
{
	m_SpriteData.param[1].x = In_Scale.x;
	m_SpriteData.param[1].y = In_Scale.y;
}
void Sprite::SetColor(_In_ const DirectX::XMFLOAT4 &In_Color) noexcept
{
	m_SpriteData.param[2] = In_Color;
}
void Sprite::SetTexture(_In_ Texture *In_pTex) noexcept
{
	m_SpriteData.texture = In_pTex ? In_pTex : m_whiteTex.get();
}
void Sprite::SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept
{
	m_Position = In_Pos;
}
void Sprite::SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept
{
	m_Scale = In_Scale;
}
void Sprite::SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rot) noexcept
{
	m_Rotation = ToRad(In_Rot);
}
void Sprite::Set3D(_In_ const bool &In_Is3D) noexcept
{
	m_SpriteData.Is3D = In_Is3D;
}
void Sprite::SetBillBoard(_In_ const bool &In_IsBillBoard) noexcept
{
	m_SpriteData.IsBillBoard = In_IsBillBoard;
}
void Sprite::SetWorld(_In_ DirectX::XMFLOAT4X4 In_World) noexcept
{
	m_SpriteData.matrix[0] = In_World;
}
void Sprite::SetView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept
{
	m_SpriteData.matrix[1] = In_View;
}
void Sprite::SetProjection(_In_ DirectX::XMFLOAT4X4 In_Proj) noexcept
{
	m_SpriteData.matrix[2] = In_Proj;
}
void Sprite::SetBillBoardView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept
{
	m_BillBoardView = In_View;
}
void Sprite::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	if (In_Vs && typeid(VertexShader) == typeid(*In_Vs))
		m_SpriteData.vs = In_Vs;
	else
		m_SpriteData.vs = m_defVS.get();
}
void Sprite::SetPixelShader(_In_ Shader *In_Ps) noexcept
{
	if (In_Ps && typeid(PixelShader) == typeid(*In_Ps))
		m_SpriteData.ps = In_Ps;
	else
		m_SpriteData.ps = m_defPS.get();
}

Sprite::Sprite()
	: m_Position(), m_Scale(1.0f,1.0f,1.0f), m_Rotation(), m_BillBoardView()
{
}

Sprite::~Sprite()
{
	delete m_SpriteData.texture; // テクスチャの解放
	m_SpriteData.texture = nullptr; // nullptrに設定しておく
}
