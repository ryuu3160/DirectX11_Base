/*+===================================================================
	File: Gizmos.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/
#pragma once

#include "DirectX11/Resource/Shaders/Shader.hpp"
#include "DirectX11/Resource/Meshes/MeshBuffer.hpp"

// ==============================
//  前方宣言
// ==============================
class GameObject;
class RenderContext;

// ==============================
//	定数定義
// ==============================
namespace
{
	inline constexpr DirectX::XMFLOAT4 DefaultLineColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

class Gizmos
{
	friend class RenderManager;
public:
	Gizmos();
	~Gizmos();

	void SetVertexShader(_In_ VertexShader* In_Vs) noexcept;
	void SetPixelShader(_In_ PixelShader* In_Ps) noexcept;
	void SetColor(_In_ DirectX::XMFLOAT4 In_Color) noexcept;
	void SetLightDirection(_In_ DirectX::XMFLOAT3 In_Dir) noexcept;
	void EnableLight(_In_ bool In_Enable) noexcept;

	void AddLine(_In_ GameObject* In_Obj, _In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End, _In_ DirectX::XMFLOAT4 In_StartColor = DefaultLineColor, _In_ DirectX::XMFLOAT4 In_EndColor = DefaultLineColor) noexcept;
	void DrawBox(_In_ DirectX::XMFLOAT4X4 In_World) noexcept;
	void DrawSphere(_In_ DirectX::XMFLOAT4X4 In_World) noexcept;

	void UpdateMeshBuffer() noexcept { m_IsUpdate = true; }

private:
	void Init();
	void MakeVertexShader() noexcept;
	void MakePixelShader() noexcept;
	void MakeLine() noexcept;
	void MakeBox() noexcept;
	void MakeSphere() noexcept;

	void DrawLines(_In_ RenderContext *In_RenderContext) noexcept;
	void DrawBoxes(_In_ RenderContext *In_RenderContext) noexcept;
	void DrawSpheres(_In_ RenderContext *In_RenderContext) noexcept;
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;
	};

	struct Data
	{
		std::shared_ptr<VertexShader> defVS;
		std::shared_ptr<PixelShader> defPS;
		VertexShader *pVS = nullptr;
		PixelShader *pPS = nullptr;
		DirectX::XMFLOAT4 param[2] = {};
		std::vector<Vertex> lineVtxs;
		std::vector<DirectX::XMFLOAT4X4> BoxMatrices;
		std::vector<DirectX::XMFLOAT4X4> SphereMatrices;
		std::shared_ptr<MeshBuffer> lineMesh;
		std::shared_ptr<MeshBuffer> boxMesh;
		std::shared_ptr<MeshBuffer> sphereMesh;
		GameObject *pOwner = nullptr;
	};

	Data m_Data;
	std::mutex m_Mutex;
	MeshBuffer::Description m_Desc;
	bool m_IsUpdate;
};
