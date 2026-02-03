/*+===================================================================
	File: LineRenderer.hpp
	Summary: 線を描画するコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 03:44:10 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Core/DirectX11/Resource/ShaderResource/ShaderParam.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	inline constexpr DirectX::XMFLOAT4 cx_DefaultLineColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

// ---ComponentDescription---
// ラインを描画するコンポーネント
// --------------------------
class LineRenderer : public RenderComponent
{
public:
	LineRenderer();
	~LineRenderer();

	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	void SetColor(_In_ DirectX::XMFLOAT4 In_Color) noexcept { m_Data.param[0] = In_Color; }

	void AddLine(_In_ DirectX::XMFLOAT3 In_Start, _In_ DirectX::XMFLOAT3 In_End, _In_ DirectX::XMFLOAT4 In_StartColor = cx_DefaultLineColor, _In_ DirectX::XMFLOAT4 In_EndColor = cx_DefaultLineColor) noexcept;

	/// <summary>
	/// ライン情報をクリアします
	/// </summary>
	void ClearLines() noexcept;

	/// <summary>
	/// 指定されたテクスチャスロットに描画を行います
	/// </summary>
	void Draw(_In_ RenderContext *In_RenderContext) noexcept override final;

protected:
#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif // _DEBUG

private:

	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

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
		std::shared_ptr<MeshBuffer> lineMesh;
	};
	std::mutex m_Mutex;
	Data m_Data;
	MeshBuffer::Description m_Desc;
	bool m_IsUpdate;
	bool m_IsClearLines;
};
