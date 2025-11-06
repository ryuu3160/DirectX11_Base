/*+===================================================================
	File: ModelRenderer.hpp
	Summary: モデルレンダラー 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/01 Tue AM 10:45:12 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Renderer/RenderComponent.hpp"
#include "DirectX11/Resource/ShaderParam.hpp"

// ==============================
//	前方宣言
// ==============================
class Material;
class Mesh;

class ModelRenderer : public RenderComponent
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
    ModelRenderer();
    ~ModelRenderer();
    void Update() noexcept final;

	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	template<typename T, typename std::enable_if<std::is_same<ShaderParam, T>::value>::type * = nullptr>
	void SetWriteParam(_In_ T *In_Param);

	/// <summary>
	/// マテリアルシェーダーの使用状態を設定します。
	/// </summary>
	/// <param name="[In_IsUse]">マテリアルシェーダーを使用するかどうかを示す真偽値。</param>
	inline void IsUseMaterialShader(_In_ const bool &In_IsUse) noexcept { m_bUseMaterialShader = In_IsUse; }

	/// <summary>
	/// 指定されたインデックスのメッシュを取得します。
	/// </summary>
	/// <param name="[In_Index]">取得したいメッシュのインデックス。</param>
	/// <returns>インデックスが有効な場合は対応するMeshオブジェクトへのポインタ。無効な場合はnullptrを返します。</returns>
	inline const std::shared_ptr<Mesh> GetMesh(_In_ const unsigned int &In_Index) const noexcept
	{
		if (In_Index < 0 || m_vecMeshes.size() <= In_Index)
		{
			return nullptr;
		}
		return m_vecMeshes[In_Index];
	}

	/// <summary>
	/// メッシュの数を取得します
	/// </summary>
	/// <returns>メッシュの数を表す定数参照（unsigned int型）を返します。</returns>
	inline const unsigned int &GetMeshNum() const noexcept { return static_cast<uint32_t>(m_vecMeshes.size()); }


	/// <summary>
	/// ファイルを指定されたパスから読み込み、スケールと反転オプションを適用します。
	/// </summary>
	/// <param name="[In_File]">読み込むファイルのパス。</param>
	/// <param name="[In_Scale]">適用するスケール係数（デフォルトは1.0f）。</param>
	/// <param name="[In_IsFlip]">ファイルを反転して読み込むかどうか（デフォルトはfalse）。</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返します。</returns>
	bool Load(_In_ const FilePath &In_File, _In_ const float &In_Scale = 1.0f, _In_ const bool &In_IsFlip = false);

	/// <summary>
	/// 既に設定されているアセットパスとスケールを使用してファイルを読み込みます。
	/// </summary>
	/// <param name="[In_IsFlip]">ロード時にアセットを反転するかどうかを指定します。デフォルトは false です。</param>
	/// <returns>ロードが成功した場合は true、失敗した場合は false を返します。</returns>
	bool Load(_In_ const bool &In_IsFlip = false) { return this->Load(m_AssetPath, m_fScale, In_IsFlip); }

	/// <summary>
	/// 指定されたテクスチャスロットに描画を行います。
	/// </summary>
	void Draw() noexcept override final;

	/// <summary>
	/// 頂点データを再生成するための関数を呼び出します。
	/// </summary>
	/// <param name="[In_VtxSize]">再生成する頂点データのサイズ。</param>
	/// <param name="[In_Func]">RemakeInfo 構造体への参照を受け取り、頂点データの再生成処理を行うコールバック関数。</param>
	void RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func);

//#if _DEBUG
//    void Debug(debug::Window *window) final;
//#endif

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

private:
	Meshes m_vecMeshes;
	VertexShader *m_pVS;
	PixelShader *m_pPS;

	// モデル全体のPixelシェーダーに書き込むもの
	std::vector<DirectX::XMFLOAT4> m_vecLightParam; // ライトパラメータ
	std::vector<DirectX::XMFLOAT4> m_vecCameraParam; // カメラパラメータ

    float m_fScale;
	bool m_bUseMaterialShader;	// マテリアルに付いているシェーダーを使用するかどうか

	std::vector<ShaderParam *> m_pShaderParams; // シェーダーパラメータ
};

/// <summary>
/// 指定されたシェーダーパラメータをスロット番号に基づいて設定します。
/// </summary>
/// <typeparam name="[T]">ShaderParamを継承した型。</typeparam>
/// <param name="[In_Param]">設定するシェーダーパラメータへのポインタ。</param>
template<typename T, typename std::enable_if<std::is_same<ShaderParam, T>::value>::type*>
inline void ModelRenderer::SetWriteParam(T *In_Param)
{
	if(In_Param)
		m_pShaderParams.push_back(In_Param);
}
