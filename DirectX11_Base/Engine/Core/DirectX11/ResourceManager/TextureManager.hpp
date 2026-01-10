/*+===================================================================
	File: TextureManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/15 Fri AM 01:22:43 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/DirectX11/Resource/Texture.hpp"

// ==============================
//  前方宣言
// ==============================
struct aiScene;
struct aiMaterial;
enum aiTextureType;

/// <summary>
/// TextureManagerクラス
/// </summary>
class TextureManager : public Singleton<TextureManager>
{
	friend class Singleton<TextureManager>;
public:

	/// <summary>
	/// 指定されたファイルパスからテクスチャを取得します。
	/// </summary>
	/// <param name="[In_FilePath]">テクスチャファイルのパスを指定します。</param>
	/// <returns>取得したテクスチャへのポインタ。失敗した場合は nullptr を返します。</returns>
	std::shared_ptr<Texture> GetTexture(_In_ const FilePath &In_FilePath) noexcept;

	/// <summary>
	/// 指定された aiScene からテクスチャを読み込みます。
	/// </summary>
	/// <param name="[In_Scene]">テクスチャを読み込む対象となる aiScene オブジェクトへのポインタ</param>
	/// <param name="[In_File]">テクスチャファイルのパスを指定する FilePath オブジェクト</param>
	void LoadTextures(_In_ const aiScene *In_Scene,_In_ const FilePath &In_File) noexcept;

	/// <summary>
	/// 指定されたファイルパスからテクスチャを読み込みます。
	/// </summary>
	/// <param name="In_FilePath">読み込むテクスチャファイルのパス。</param>
	/// <returns>読み込まれたテクスチャへの共有ポインタ。失敗した場合は nullptr を返します。</returns>
	std::shared_ptr<Texture> LoadTexture(_In_ const FilePath &In_FilePath) noexcept;

private:
	TextureManager();
	~TextureManager() override;

private:
	std::unordered_map<FilePathHold, std::shared_ptr<Texture>> m_mapTextures; // テクスチャのマップ
};
