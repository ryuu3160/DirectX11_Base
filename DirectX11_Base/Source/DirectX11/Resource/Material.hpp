/*+===================================================================
	File: Material.hpp
	Summary: マテリアルクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:02:57 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/Texture/MeshBuffer.hpp"
#include "DirectX11/Texture/Texture.hpp"
#include "ResourceSetting.hpp"

// ==============================
//  前方宣言
// ==============================
struct aiMaterial;

/// <summary>
/// Materialクラス
/// </summary>
class Material
{
private:
	struct ShaderParamInfo
	{
		std::string ParamName;
		int SlotNum;
	};

public:
	Material();
	~Material();

	/// <summary>
	/// ファイルからマテリアルを読み込む
	/// </summary>
	/// <param name="[In_pMaterial]">assimpのMaterialへのポインタ</param>
	/// <param name="[In_File]">ファイルパス</param>
	/// <param name="[In_MaterialName]">マテリアル名</param>
	void Load(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File, _In_ const std::string In_MaterialName) noexcept;

	/// <summary>
	/// テクスチャを取得します。
	/// </summary>
	/// <returns>テクスチャを指す std::shared_ptr を返します。</returns>
	std::shared_ptr<Texture> GetTexture(_In_ const ResourceSetting::TextureType &In_Type) const noexcept;

	/// <summary>
	/// テクスチャの配列を取得します。
	/// </summary>
	/// <returns>ResourceSetting::TextureType_Max 個の std::shared_ptr<Texture> からなる std::array を返します。</returns>
	std::array<std::shared_ptr<Texture>, ResourceSetting::TextureType_Max> GetTextures() const noexcept;

	/// <summary>
	/// テクスチャの数を取得します。
	/// </summary>
	/// <returns>テクスチャの数を表す整数値。</returns>
	int GetTextureNum() const noexcept;

	/// <summary>
	/// マテリアル名を取得します。
	/// </summary>
	/// <returns>マテリアル名を表す std::string への参照。</returns>
	inline const std::string &GetMaterialName() const noexcept { return m_strMaterialName; }

	inline VertexShader *GetVertexShader() const noexcept
	{
		if(!m_bIsInstancedVertexShader)
			return reinterpret_cast<VertexShader *>(m_pVS);
		else
			return nullptr;
	}

	inline InstancedVertexShader *GetInstancedVertexShader() const noexcept
	{
		if (m_bIsInstancedVertexShader)
			return reinterpret_cast<InstancedVertexShader *>(m_pVS);
		else
			return nullptr;
	}

	inline PixelShader *GetPixelShader() const noexcept { return m_pPS; }

	/// <summary>
	/// テクスチャを設定します。
	/// </summary>
	/// <param name="[In_Type]">設定するテクスチャの種類。</param>
	/// <param name="[In_pTexture]">設定するテクスチャへのポインタ。</param>
	void SetTexture(_In_ const ResourceSetting::TextureType &In_Type, _In_ std::shared_ptr<Texture> In_pTexture) noexcept;

	/// <summary>
	/// シェーダーパラメータのリストを取得します。
	/// </summary>
	/// <returns>ResourceSetting::ShaderParamType 型の std::vector への定数参照。</returns>
	inline const std::vector<ShaderParamInfo> &GetShaderParamList() const noexcept { return m_vecShaderParamList; }

	/// <summary>
	/// インスタンスト頂点シェーダーかどうかの状態を設定します。
	/// </summary>
	/// <param name="In_IsInstanced">インスタンスト頂点シェーダーであるかどうかを示す真偽値。</param>
	inline void SetIsInstancedVertexShader(_In_ const bool &In_IsInstanced) noexcept { m_bIsInstancedVertexShader = In_IsInstanced; }

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

	/// <summary>
	/// MaterialShaderInfoファイルからシェーダー情報を読み込みます。
	/// </summary>
	/// <param name="[In_Directory]">シェーダー名を検索するディレクトリへの参照。</param>
	/// <returns>読み取りが成功したかどうか</returns>
	bool LoadShaderInfo(_In_ const std::string_view &In_Directory) noexcept;

	/// <summary>
	/// 書き込みパラメータを読み込みます。
	/// </summary>
	/// <param name="[In_WriteParam]">読み込む書き込みパラメータを表す文字列ビュー。</param>
	void LoadWriteParam(_In_ const std::string_view &In_WriteParam) noexcept;

	/// <summary>
	/// マテリアルシェーダー情報を保存します。
	/// </summary>
	void SaveMaterialShaderInfo(_In_ const std::string_view &In_Directory) const noexcept;

private:
	std::string m_strMaterialName; // マテリアル名
	DirectX::XMFLOAT4 m_fDiffuse;
	DirectX::XMFLOAT4 m_fAmbient;
	DirectX::XMFLOAT4 m_fSpecular;
	std::array<std::shared_ptr<Texture>,ResourceSetting::TextureType_Max> m_spTextures;
	std::string m_strDirectory; // マテリアルのディレクトリパス

	std::vector<ShaderParamInfo> m_vecShaderParamList; // PixelShaderに書き込む情報リスト

	bool m_bIsInstancedVertexShader; // インスタンシング用の頂点シェーダーかどうか

	Shader *m_pVS;
	PixelShader *m_pPS;
	std::string m_strVSName;
	std::string m_strPSName;

	std::shared_ptr<Shader> m_defVS;
	std::shared_ptr<PixelShader> m_defPS;
};
