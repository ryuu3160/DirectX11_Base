/*+===================================================================
	File: LineRenderer.cpp
	Summary: 線を描画するコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 03:44:20 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "LineRenderer.hpp"
#include "Core/System/Object/GameObject.hpp"

// ==============================
//  定数
// ==============================
namespace
{
	inline constexpr int DEFAULT_LINE_NUM = 1024 * 2;
}


LineRenderer::LineRenderer()
	: RenderComponent("LineRenderer")
	, m_IsUpdate(false), m_IsClearLines(false)
{
	m_Data.param[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Data.param[1] = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	MakeDefaultShader();

	m_Data.lineVtxs.reserve(DEFAULT_LINE_NUM);

	MeshBuffer::Description desc{};
	desc.pVtx = m_Data.lineVtxs.data();
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = static_cast<UINT>(m_Data.lineVtxs.size());
	desc.isWrite = true;
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_Data.lineMesh = std::make_shared<MeshBuffer>(desc);

	m_Desc = desc;

	m_LayerGroup = LayerGroup_Line;
}

LineRenderer::~LineRenderer()
{
	m_Data.lineMesh.reset();
	m_Data.defPS.reset();
	m_Data.defVS.reset();
}

void LineRenderer::SetVertexShader(_In_ Shader *In_Vs) noexcept
{
	VertexShader *vs = dynamic_cast<VertexShader *>(In_Vs);
	if(vs)
		m_Data.pVS = vs;
}

void LineRenderer::SetPixelShader(_In_ Shader *In_Ps) noexcept
{
	PixelShader *ps = dynamic_cast<PixelShader *>(In_Ps);
	if(ps)
		m_Data.pPS = ps;
}

void LineRenderer::AddLine(_In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End, _In_ DirectX::XMFLOAT4 In_StartColor, _In_ DirectX::XMFLOAT4 In_EndColor) noexcept
{
	Vertex start, end;
	start.pos = In_Start;
	start.color = In_StartColor;
	start.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	end.pos = In_End;
	end.color = In_EndColor;
	end.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	m_Data.lineVtxs.emplace_back(start);
	m_Data.lineVtxs.emplace_back(end);
	m_IsUpdate = true;
}

void LineRenderer::ClearLines() noexcept
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_Data.lineVtxs.clear();
}

void LineRenderer::Draw(_In_ RenderContext *In_RenderContext) noexcept
{
	// 定数バッファに渡す行列の情報を作成
	DirectX::XMFLOAT4X4 mat[3];
	// カメラのビュー/プロジェクション行列を設定
	mat[1] = In_RenderContext->GetView();
	mat[2] = In_RenderContext->GetProj();

	// 単位行列でワールド行列を作成
	mat[0] = m_pGameObject->GetWorld();

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
		m_Data.lineMesh->RemakeBuffer(m_Data.lineVtxs.data(),m_Desc);
		m_IsUpdate = false;
	}
	m_Data.lineMesh->Draw(static_cast<int>(m_Data.lineVtxs.size()));

	m_Data.param[0] = colorBackup;
	m_Data.param[1].w = lightBackup;
}


#ifdef _DEBUG
void LineRenderer::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
}
#endif

void LineRenderer::MakeDefaultShader()
{
	m_Data.defVS = std::make_shared<VertexShader>();
	m_Data.defVS->Load("Binaries/Shader/VS_Geometory.cso");
	m_Data.pVS = m_Data.defVS.get();
	m_Data.defPS = std::make_shared<PixelShader>();
	m_Data.defPS->Load("Binaries/Shader/PS_Geometory.cso");
	m_Data.pPS = m_Data.defPS.get();
}
