/*+===================================================================
	File: Gizmos.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/
#include "Gizmos.hpp"
#include "RenderManager.hpp"

void Gizmos::Init()
{
	m_Data.param[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Data.param[1] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	MakeVertexShader();
	MakePixelShader();
	MakeLine();
	MakeBox();
	MakeSphere();
	m_IsUpdate = true;
}

void Gizmos::SetVertexShader(_In_ VertexShader *In_Vs) noexcept
{
	m_Data.pVS = In_Vs;
}
void Gizmos::SetPixelShader(_In_ PixelShader *In_Ps) noexcept
{
	m_Data.pPS = In_Ps;
}
void Gizmos::SetColor(_In_ DirectX::XMFLOAT4 In_Color) noexcept
{
	m_Data.param[0] = In_Color;
}
void Gizmos::SetLightDirection(_In_ DirectX::XMFLOAT3 In_Dir) noexcept
{
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat3(&In_Dir);
	DirectX::XMStoreFloat3(&In_Dir, DirectX::XMVector3Normalize(vDir));
	m_Data.param[1].x = In_Dir.x;
	m_Data.param[1].y = In_Dir.y;
	m_Data.param[1].z = In_Dir.z;
}
void Gizmos::EnableLight(_In_ bool In_Enable) noexcept
{
	m_Data.param[1].w = In_Enable ? 0.0f : 1.0f;
}

void Gizmos::AddLine(_In_ GameObject *In_Obj, _In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End, _In_ DirectX::XMFLOAT4 In_StartColor, _In_ DirectX::XMFLOAT4 In_EndColor) noexcept
{
	DirectX::XMFLOAT3 ObjPos = In_Obj->GetPosition();

	Vertex start, end;
	start.pos = In_Start;
	start.color = In_StartColor;
	start.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	end.pos = In_End;
	end.color = In_EndColor;
	end.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	// ワールド行列から位置を変換
	start.pos += ObjPos;
	end.pos += ObjPos;

	m_Data.lineVtxs.emplace_back(start);
	m_Data.lineVtxs.emplace_back(end);
	m_IsUpdate = true;
}
void Gizmos::DrawLines(_In_ RenderContext *In_RenderContext) noexcept
{
	if(m_Data.lineVtxs.empty())
	{
		return;
	}

	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = In_RenderContext->GetView();
	mat[2] = In_RenderContext->GetProj();

	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, DirectX::XMMatrixIdentity());
	// 単位行列でワールド行列を作成
	mat[0] = fmat;

	DirectX::XMFLOAT4 colorBackup = m_Data.param[0];
	float lightBackup = m_Data.param[1].w;

	m_Data.param[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Data.param[1].w = 1.0f;

	m_Data.pVS->WriteBuffer(0, mat);
	m_Data.pVS->WriteBuffer(1, m_Data.param);
	m_Data.pVS->Bind();
	m_Data.pPS->Bind();
	if(m_IsUpdate)
	{
		m_Desc.pVtx = m_Data.lineVtxs.data();
		m_Desc.vtxSize = sizeof(Vertex);
		m_Desc.vtxCount = static_cast<UINT>(m_Data.lineVtxs.size());
		m_Data.lineMesh->RemakeBuffer(m_Data.lineVtxs.data(), m_Desc);
		m_IsUpdate = false;
	}
	m_Data.lineMesh->Draw(static_cast<int>(m_Data.lineVtxs.size()));

	m_Data.param[0] = colorBackup;
	m_Data.param[1].w = lightBackup;

	m_Data.lineVtxs.clear();
}
void Gizmos::DrawBoxes(_In_ RenderContext *In_RenderContext) noexcept
{
	for(auto &itr : m_Data.BoxMatrices)
	{
		// 定数バッファに渡す行列の情報を作成
		DirectX::XMFLOAT4X4 mat[3];
		// カメラのビュー/プロジェクション行列を設定
		mat[1] = In_RenderContext->GetView();
		mat[2] = In_RenderContext->GetProj();

		mat[0] = itr;
		m_Data.pVS->WriteBuffer(0, mat);
		m_Data.pVS->WriteBuffer(1, m_Data.param);
		m_Data.pVS->Bind();
		m_Data.pPS->Bind();
		m_Data.boxMesh->Draw();
	}
	m_Data.BoxMatrices.clear();
}
void Gizmos::DrawSpheres(_In_ RenderContext *In_RenderContext) noexcept
{
	for(auto &itr : m_Data.SphereMatrices)
	{
		// 定数バッファに渡す行列の情報を作成
		DirectX::XMFLOAT4X4 mat[3];
		// カメラのビュー/プロジェクション行列を設定
		mat[1] = In_RenderContext->GetView();
		mat[2] = In_RenderContext->GetProj();

		mat[0] = itr;
		m_Data.pVS->WriteBuffer(0, mat);
		m_Data.pVS->WriteBuffer(1, m_Data.param);
		m_Data.pVS->Bind();
		m_Data.pPS->Bind();
		m_Data.sphereMesh->Draw();
	}
	m_Data.SphereMatrices.clear();
}
void Gizmos::DrawBox(_In_ DirectX::XMFLOAT4X4 In_World) noexcept
{
	m_Data.BoxMatrices.push_back(In_World);
}
void Gizmos::DrawSphere(_In_ DirectX::XMFLOAT4X4 In_World) noexcept
{
	m_Data.SphereMatrices.push_back(In_World);
}

void Gizmos::MakeVertexShader() noexcept
{
	m_Data.defVS = std::make_shared<VertexShader>();
	HRESULT hr = m_Data.defVS->Load("Binaries/Shader/VS_Geometory.cso");
	m_Data.pVS = m_Data.defVS.get();
}

void Gizmos::MakePixelShader() noexcept
{
	m_Data.defPS = std::make_shared<PixelShader>();
	HRESULT hr = m_Data.defPS->Load("Binaries/Shader/PS_Geometory.cso");
	m_Data.pPS = m_Data.defPS.get();
}

void Gizmos::MakeLine() noexcept
{
	m_Data.lineVtxs.reserve(2048);

	MeshBuffer::Description desc{};
	desc.pVtx = m_Data.lineVtxs.data();
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = static_cast<UINT>(m_Data.lineVtxs.size());
	desc.isWrite = true;
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_Data.lineMesh = std::make_shared<MeshBuffer>(desc);
}
void Gizmos::MakeBox() noexcept
{
	const float d = 0.5f;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 normal[] = {
		DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f),
		DirectX::XMFLOAT3( 0.0f,-1.0f, 0.0f),
		DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT3( 0.0f, 0.0f,-1.0f),
	};
	Vertex vtx[] = {
		// +X
		{DirectX::XMFLOAT3( d, d,-d), color, normal[0]},
		{DirectX::XMFLOAT3( d, d, d), color, normal[0]},
		{DirectX::XMFLOAT3( d,-d,-d), color, normal[0]},
		{DirectX::XMFLOAT3( d,-d, d), color, normal[0]},
		// -X
		{DirectX::XMFLOAT3(-d, d, d), color, normal[1]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[1]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[1]},
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[1]},
		// +Y
		{DirectX::XMFLOAT3(-d, d, d), color, normal[2]},
		{DirectX::XMFLOAT3( d, d, d), color, normal[2]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[2]},
		{DirectX::XMFLOAT3( d, d,-d), color, normal[2]},
		// -Y
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[3]},
		{DirectX::XMFLOAT3( d,-d,-d), color, normal[3]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[3]},
		{DirectX::XMFLOAT3( d,-d, d), color, normal[3]},
		// +Z
		{DirectX::XMFLOAT3(-d, d, d), color, normal[4]},
		{DirectX::XMFLOAT3( d, d, d), color, normal[4]},
		{DirectX::XMFLOAT3(-d,-d, d), color, normal[4]},
		{DirectX::XMFLOAT3( d,-d, d), color, normal[4]},
		// -Z
		{DirectX::XMFLOAT3(-d,-d,-d), color, normal[5]},
		{DirectX::XMFLOAT3( d,-d,-d), color, normal[5]},
		{DirectX::XMFLOAT3(-d, d,-d), color, normal[5]},
		{DirectX::XMFLOAT3( d, d,-d), color, normal[5]},
	};
	WORD idx[] = {
		 2, 0, 1,  1, 3, 2,
		 6, 4, 5,  5, 7, 6,
		10, 8, 9,  9,11,10,
		14,12,13, 13,15,14,
		18,16,17, 17,19,18,
		22,20,21, 21,23,22,
	};
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx;
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = _countof(vtx);
	desc.pIdx = idx;
	desc.idxSize = sizeof(idx[0]);
	desc.idxCount = _countof(idx);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Data.boxMesh = std::make_shared<MeshBuffer>(desc);
}
void Gizmos::MakeSphere() noexcept
{
	std::vector<Vertex> vtx;
	const int detail = 32;
	for (int j = 0; j < detail / 2 + 1; ++j)
	{
		float radY = DirectX::XM_2PI / detail * j;
		for (int i = 0; i < detail; ++i)
		{
			float radXZ = DirectX::XM_2PI / detail * i;
			float x = sinf(radY) * sinf(radXZ);
			float y = cosf(radY);
			float z = sinf(radY) * cosf(radXZ);
			vtx.push_back({
				DirectX::XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f),
				DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				DirectX::XMFLOAT3(x, y, z)
			});
		}
	}
	std::vector<DWORD> idx;
	for (int j = 0; j < detail / 2; ++j)
	{
		for (int i = 0; i < detail; ++i)
		{
			idx.push_back((j + 1) * detail + i);
			idx.push_back(j * detail + i);
			idx.push_back(j * detail + ((i + 1) % detail));
			idx.push_back(idx.back());
			idx.push_back((j + 1) * detail + ((i + 1) % detail));
			idx.push_back((j + 1) * detail + i);
		}
	}
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx.data();
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = static_cast<UINT>(vtx.size());
	desc.pIdx = idx.data();
	desc.idxSize = sizeof(idx[0]);
	desc.idxCount = static_cast<UINT>(idx.size());
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Data.sphereMesh = std::make_shared<MeshBuffer>(desc);
}

Gizmos::Gizmos()
{
	Init();
}

Gizmos::~Gizmos()
{
	m_Data.sphereMesh.reset();
	m_Data.boxMesh.reset();
	m_Data.lineMesh.reset();
	m_Data.defPS.reset();
	m_Data.defVS.reset();
}
