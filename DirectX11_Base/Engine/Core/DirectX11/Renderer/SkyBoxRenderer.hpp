/*+===================================================================
	File: SkyBoxRenderer.hpp
	Summary: スカイボックス用のレンダーコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/30 Sat AM 12:34:34 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Engine/Core/DirectX11/Resource/ResourceSetting.hpp"

// ==============================
//	前方宣言
// ==============================
class Material;
class Mesh;

/// <summary>
/// SkyBoxRendererクラス
/// </summary>
class SkyBoxRenderer : public RenderComponent
{
public:
	using Meshes = std::vector<std::shared_ptr<Mesh>>;

	/// <summary>
	/// 頂点とインデックスデータの情報を格納する構造体
	/// </summary>
	struct RemakeInfo
	{
		UINT vtxNum;
		void *dest;
		const void *source;
		UINT idxNum;
		const void *idx;
	};

public:
	SkyBoxRenderer();
	~SkyBoxRenderer();

	void Update(_In_ float In_DeltaTime) noexcept final;

	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	/// <summary>
	/// マテリアルシェーダーの使用状態を設定します。
	/// </summary>
	/// <param name="[In_IsUse]">マテリアルシェーダーを使用するかどうかを示す真偽値。</param>
	inline void IsUseMaterialShader(_In_ const bool &In_IsUse) noexcept { m_bUseMaterialShader = In_IsUse; }

	/// <summary>
	/// ファイルを指定されたパスから読み込み、スケールと反転オプションを適用します。
	/// </summary>
	/// <param name="[In_File]">読み込むファイルのパス。</param>
	/// <param name="[In_Scale]">適用するスケール係数（デフォルトは1.0f）。</param>
	/// <param name="[In_IsFlip]">ファイルを反転して読み込むかどうか（デフォルトはfalse）。</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返します。</returns>
	bool Load(_In_ const FilePath &In_File, _In_ const float &In_Scale = 1.0f, _In_ const bool &In_IsFlip = false);

	/// <summary>
	/// 指定されたテクスチャスロットに描画を行います。
	/// </summary>
	void Draw(_In_ RenderContext *In_RenderContext) noexcept override final;

	/// <summary>
	/// 頂点データを再生成するための関数を呼び出します。
	/// </summary>
	/// <param name="[In_VtxSize]">再生成する頂点データのサイズ。</param>
	/// <param name="[In_Func]">RemakeInfo 構造体への参照を受け取り、頂点データの再生成処理を行うコールバック関数。</param>
	void RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func);

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

private:
	Meshes m_vecMeshes;
	VertexShader *m_pVS;
	PixelShader *m_pPS;

	float m_fScale;
	bool m_bUseMaterialShader;	// マテリアルに付いているシェーダーを使用するかどうか
};
