/*+===================================================================
	File: Geometory.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 07:23:17 初回作成
===================================================================+*/
#pragma once

#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/Texture/MeshBuffer.hpp"

namespace 
{
	constexpr int GEOMETORY_MAX_LINE_NUM = 10000 * 2; // 1直線につき、2頂点
}

class Geometory : public Singleton<Geometory>
{
	friend class Singleton<Geometory>;
public:
	void Init();
	void Uninit();

	void SetVertexShader(_In_ VertexShader* In_Vs) noexcept;
	void SetPixelShader(_In_ PixelShader* In_Ps) noexcept;
	void SetWorld(_In_ DirectX::XMFLOAT4X4 In_World) noexcept;
	void SetView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept;
	void SetProjection(_In_ DirectX::XMFLOAT4X4 In_Proj) noexcept;
	void SetColor(_In_ DirectX::XMFLOAT4 In_Color) noexcept;
	void SetLightDirection(_In_ DirectX::XMFLOAT3 In_Dir) noexcept;
	void EnableLight(_In_ bool In_Enable) noexcept;

	void AddLine(_In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End) noexcept;
	void AddLine(
		_In_ DirectX::XMFLOAT3 In_StartPos, _In_ DirectX::XMFLOAT4 In_StartColor,
		_In_ DirectX::XMFLOAT3 In_EndPos, _In_ DirectX::XMFLOAT4 In_EndColor) noexcept;
	void DrawLines() noexcept;
	void DrawBox() noexcept;
	void DrawSphere() noexcept;

private:
	void MakeVertexShader() noexcept;
	void MakePixelShader() noexcept;
	void MakeLine() noexcept;
	void MakeBox() noexcept;
	void MakeSphere() noexcept;

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;
	};
	struct Data
	{
		const int LineNum = GEOMETORY_MAX_LINE_NUM; // 1直線につき、2頂点
		std::shared_ptr<VertexShader> defVS;
		std::shared_ptr<PixelShader> defPS;
		VertexShader* pVS;
		PixelShader* pPS;
		DirectX::XMFLOAT4X4 matrix[3];
		DirectX::XMFLOAT4 param[2];
		Vertex lineVtx[GEOMETORY_MAX_LINE_NUM];
		UINT lineIndex;
		std::shared_ptr<MeshBuffer> lineMesh;
		std::shared_ptr<MeshBuffer> boxMesh;
		std::shared_ptr<MeshBuffer> sphereMesh;
	};
	Data m_Data;
};
