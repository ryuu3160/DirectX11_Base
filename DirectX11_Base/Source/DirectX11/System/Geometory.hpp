/*+===================================================================
	File: Geometory.hpp
	Summary: wasssk様のものを参考にして作成
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

	void SetVertexShader(VertexShader* vs);
	void SetPixelShader(PixelShader* ps);
	void SetWorld(DirectX::XMFLOAT4X4 world);
	void SetView(DirectX::XMFLOAT4X4 view);
	void SetProjection(DirectX::XMFLOAT4X4 proj);
	void SetColor(DirectX::XMFLOAT4 color);
	void SetLightDirection(DirectX::XMFLOAT3 dir);
	void EnableLight(bool enable);

	void AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);
	void AddLine(
		DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT4 startColor, 
		DirectX::XMFLOAT3 endPos, DirectX::XMFLOAT4 endColor);
	void DrawLines();
	void DrawBox();
	void DrawSphere();

private:
	void MakeVertexShader();
	void MakePixelShader();
	void MakeLine();
	void MakeBox();
	void MakeSphere();

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
