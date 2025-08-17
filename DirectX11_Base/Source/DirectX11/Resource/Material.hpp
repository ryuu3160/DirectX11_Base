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

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

private:
	std::string m_strMaterialName; // マテリアル名
	DirectX::XMFLOAT4 m_fDiffuse;
	DirectX::XMFLOAT4 m_fAmbient;
	DirectX::XMFLOAT4 m_fSpecular;
	std::array<std::shared_ptr<Texture>,ResourceSetting::TextureType_Max> m_spTextures;

	VertexShader *m_pVS;
	PixelShader *m_pPS;

	std::shared_ptr<VertexShader> m_defVS;
	std::shared_ptr<PixelShader> m_defPS;
};
