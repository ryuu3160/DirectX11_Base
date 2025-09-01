/*+===================================================================
	File: InstancedModelRenderer.hpp
	Summary: インスタンシング用のモデルレンダラークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 05:05:58 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/System/RenderComponent.hpp"
#include "DirectX11/Resource/ResourceSetting.hpp"

// ==============================
//	前方宣言
// ==============================
class Material;
class InstancedMesh;

/// <summary>
/// InstancedModelRendererクラス
/// </summary>
class InstancedModelRenderer : public RenderComponent
{
public:
	using Meshes = std::vector<std::shared_ptr<InstancedMesh>>;

public:
	InstancedModelRenderer();
	~InstancedModelRenderer();

	void ExecuteUpdate() noexcept final;

	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	template<typename T, typename std::enable_if<std::is_base_of<ResourceSetting::ShaderParam, T>::value>::type * = nullptr>
	void SetWriteParam(_In_ T *In_Param);

	/// <summary>
	/// マテリアルシェーダーの使用状態を設定します。
	/// </summary>
	/// <param name="[In_IsUse]">マテリアルシェーダーを使用するかどうかを示す真偽値。</param>
	inline void IsUseMaterialShader(_In_ const bool &In_IsUse) noexcept { m_bUseMaterialShader = In_IsUse; }

	/// <summary>
	/// モデル全体のピクセルシェーダーにカメラ情報を書き込むかどうかを設定します。
	/// </summary>
	/// <param name="[In_IsEnable]">カメラ情報を書き込むかどうかを示す真偽値。</param>
	inline void IsEnablePS_WriteCamera(_In_ const bool &In_IsEnable) noexcept { m_bEnablePS_WriteCamera = In_IsEnable; }

	/// <summary>
	/// モデル全体のピクセルシェーダーに書き込む情報を設定する
	/// </summary>
	/// <param name="[In_Type]">設定するタイプ</param>
	/// <param name="[In_Enable]">書き込むかどうかの真偽値</param>
	void IsEnablePS_WriteParam(_In_ const ResourceSetting::ShaderParamType In_Type, _In_ const bool &In_Enable);

	/// <summary>
	/// データアクセサーを使用して読み書きを行います。
	/// </summary>
	/// <param name="In_Data">読み書き操作に使用する DataAccessor 型のポインタ。</param>
	void ReadWrite(_In_ DataAccessor *In_Data) override final;

	/// <summary>
	/// 指定されたインデックスのメッシュを取得します。
	/// </summary>
	/// <param name="[In_Index]">取得したいメッシュのインデックス。</param>
	/// <returns>インデックスが有効な場合は対応するMeshオブジェクトへのポインタ。無効な場合はnullptrを返します。</returns>
	inline const std::shared_ptr<InstancedMesh> GetMesh(_In_ const unsigned int &In_Index) const noexcept
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
	/// 指定されたテクスチャスロットに描画を行います。
	/// </summary>
	void Draw() noexcept override final;

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
	bool m_bEnablePS_WriteCamera; // モデル全体のピクセルシェーダーでカメラ情報を書き込むかどうか
	std::array<bool, ResourceSetting::ShaderParam_MAX> m_bEnablePS_WriteParamList; // モデル全体のピクセルシェーダーでパラメーターを書き込むかの真偽値リスト

	std::array<ResourceSetting::ShaderParam *, ResourceSetting::ShaderParam_MAX> m_pShaderParams; // シェーダーパラメータ
};

template<typename T, typename std::enable_if<std::is_base_of<ResourceSetting::ShaderParam, T>::value>::type *>
inline void InstancedModelRenderer::SetWriteParam(T *In_Param)
{
	if (In_Param)
		m_pShaderParams[In_Param->GetSlotNum()] = In_Param;
}
