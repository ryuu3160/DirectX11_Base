/*+===================================================================
	File: Geometory.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/
#include "Geometory.hpp"

void Geometory::Init()
{
	for (int i = 0; i < 3; ++i)
		DirectX::XMStoreFloat4x4(&m_Data.matrix[i], DirectX::XMMatrixIdentity());
	m_Data.param[0] = DirectX::XMFLOAT4(1, 1, 1, 1);
	m_Data.param[1] = DirectX::XMFLOAT4(1, 1, 1, 1);

	MakeVertexShader();
	MakePixelShader();
	MakeLine();
	MakeBox();
	MakeSphere();
}
void Geometory::Uninit()
{
	m_Data.sphereMesh.reset();
	m_Data.boxMesh.reset();
	m_Data.lineMesh.reset();
	m_Data.defPS.reset();
	m_Data.defVS.reset();
}

void Geometory::SetVertexShader(_In_ VertexShader *In_Vs) noexcept
{
	m_Data.pVS = In_Vs;
}
void Geometory::SetPixelShader(_In_ PixelShader *In_Ps) noexcept
{
	m_Data.pPS = In_Ps;
}
void Geometory::SetWorld(_In_ DirectX::XMFLOAT4X4 In_World) noexcept
{
	m_Data.matrix[0] = In_World;
}
void Geometory::SetView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept
{
	m_Data.matrix[1] = In_View;
}
void Geometory::SetProjection(_In_ DirectX::XMFLOAT4X4 In_Proj) noexcept
{
	m_Data.matrix[2] = In_Proj;
}
void Geometory::SetColor(_In_ DirectX::XMFLOAT4 In_Color) noexcept
{
	m_Data.param[0] = In_Color;
}
void Geometory::SetLightDirection(_In_ DirectX::XMFLOAT3 In_Dir) noexcept
{
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat3(&In_Dir);
	DirectX::XMStoreFloat3(&In_Dir, DirectX::XMVector3Normalize(vDir));
	m_Data.param[1].x = In_Dir.x;
	m_Data.param[1].y = In_Dir.y;
	m_Data.param[1].z = In_Dir.z;
}
void Geometory::EnableLight(_In_ bool In_Enable) noexcept
{
	m_Data.param[1].w = In_Enable ? 0.0f : 1.0f;
}

void Geometory::AddLine(_In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End) noexcept
{
	if (m_Data.lineIndex < GEOMETORY_MAX_LINE_NUM)
	{
		m_Data.lineVtx[m_Data.lineIndex].pos = In_Start;
		m_Data.lineVtx[m_Data.lineIndex].color = m_Data.param[0];
		++m_Data.lineIndex;
		m_Data.lineVtx[m_Data.lineIndex].pos = In_End;
		m_Data.lineVtx[m_Data.lineIndex].color = m_Data.param[0];
		++m_Data.lineIndex;
	}
}
void Geometory::AddLine(_In_ DirectX::XMFLOAT3 In_StartPos, _In_ DirectX::XMFLOAT4 In_StartColor,
	_In_ DirectX::XMFLOAT3 In_EndPos, _In_ DirectX::XMFLOAT4 In_EndColor) noexcept
{
	if (m_Data.lineIndex < GEOMETORY_MAX_LINE_NUM)
	{
		m_Data.lineVtx[m_Data.lineIndex].pos = In_StartPos;
		m_Data.lineVtx[m_Data.lineIndex].color = In_StartColor;
		++m_Data.lineIndex;
		m_Data.lineVtx[m_Data.lineIndex].pos = In_EndPos;
		m_Data.lineVtx[m_Data.lineIndex].color = In_EndColor;
		++m_Data.lineIndex;
	}
}
void Geometory::DrawLines() noexcept
{
	DirectX::XMFLOAT4X4 worldBackup = m_Data.matrix[0];
	DirectX::XMFLOAT4 colorBackup = m_Data.param[0];
	float lightBackup = m_Data.param[1].w;

	DirectX::XMStoreFloat4x4(&m_Data.matrix[0], DirectX::XMMatrixIdentity());
	m_Data.param[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Data.param[1].w = 1.0f;
	
	m_Data.pVS->WriteBuffer(0, m_Data.matrix);
	m_Data.pVS->WriteBuffer(1, m_Data.param);
	m_Data.pVS->Bind();
	m_Data.pPS->Bind();
	m_Data.lineMesh->Write(m_Data.lineVtx);
	m_Data.lineMesh->Draw(m_Data.lineIndex);
	
	m_Data.matrix[0] = worldBackup;
	m_Data.param[0] = colorBackup;
	m_Data.param[1].w = lightBackup;

	m_Data.lineIndex = 0;
}
void Geometory::DrawBox() noexcept
{
	m_Data.pVS->WriteBuffer(0, m_Data.matrix);
	m_Data.pVS->WriteBuffer(1, m_Data.param);
	m_Data.pVS->Bind();
	m_Data.pPS->Bind();
	m_Data.boxMesh->Draw();
}
void Geometory::DrawSphere() noexcept
{
	m_Data.pVS->WriteBuffer(0, m_Data.matrix);
	m_Data.pVS->WriteBuffer(1, m_Data.param);
	m_Data.pVS->Bind();
	m_Data.pPS->Bind();
	m_Data.sphereMesh->Draw();
}

void Geometory::MakeVertexShader() noexcept
{
	m_Data.defVS = std::make_shared<VertexShader>();
	HRESULT hr = m_Data.defVS->Load("Assets/Shader/VS_Geometory.cso");
	m_Data.pVS = m_Data.defVS.get();
}

void Geometory::MakePixelShader() noexcept
{
	m_Data.defPS = std::make_shared<PixelShader>();
	HRESULT hr = m_Data.defPS->Load("Assets/Shader/PS_Geometory.cso");
	m_Data.pPS = m_Data.defPS.get();
}

void Geometory::MakeLine() noexcept
{
	MeshBuffer::Description desc = {};
	for (int i = 0; i < GEOMETORY_MAX_LINE_NUM; ++i)
	{
		m_Data.lineVtx[i].pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_Data.lineVtx[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_Data.lineVtx[i].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
	desc.pVtx = m_Data.lineVtx;
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = GEOMETORY_MAX_LINE_NUM;
	desc.isWrite = true;
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_Data.lineIndex = 0;
	m_Data.lineMesh = std::make_shared<MeshBuffer>(desc);
}
void Geometory::MakeBox() noexcept
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
void Geometory::MakeSphere() noexcept
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