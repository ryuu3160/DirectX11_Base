/*+===================================================================
	File: ResourceSetting.hpp
	Summary: 定数とか保持しておくやつ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/16 Sat AM 03:54:05 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ResourceSetting
{
	aiTextureType const TextureTypes[] = {
		aiTextureType_DIFFUSE,    // ディフューズ
		aiTextureType_SPECULAR,   // スペキュラー
		aiTextureType_NORMALS,    // ノーマルマップ
		aiTextureType_EMISSIVE,   // エミッシブ
	};

	enum TextureType : unsigned int
	{
		TextureType_Diffuse = 0,    // ディフューズ
		TextureType_Specular,       // スペキュラー
		TextureType_Normal,         // ノーマルマップ
		TextureType_Emissive,       // エミッシブ
		TextureType_Max,            // 最大値（配列のサイズ取得用）

		TextureType_Unknown = 0xFFFFFFFF // 未知のテクスチャタイプ
	};

	inline const TextureType aiTextureTypeToTextureType(_In_ const aiTextureType &In_Type) noexcept
	{
		switch (In_Type)
		{
		case aiTextureType_DIFFUSE:    return TextureType_Diffuse;
		case aiTextureType_SPECULAR:   return TextureType_Specular;
		case aiTextureType_NORMALS:    return TextureType_Normal;
		case aiTextureType_EMISSIVE:   return TextureType_Emissive;


		case aiTextureType_UNKNOWN: // 未知のテクスチャタイプ
		default:                       return TextureType_Unknown; // 未知のテクスチャタイプ
		}
	}

	/// <summary>
	/// マテリアル名を生成する関数
	/// </summary>
	/// <param name="[In_FilePath]">fbxファイルへのパス</param>
	/// <param name="[In_pMaterial]">マテリアルオブジェクトへのポインタ</param>
	/// <returns>マテリアル名</returns>
	std::string CreateMaterialName(_In_ const FilePath &In_FilePath, _In_ const aiMaterial *In_pMaterial) noexcept;

	/// <summary>
	/// FBXのマテリアル名から、FBXの名前を抽出する関数
	/// </summary>
	/// <param name="[In_MaterialName]">マテリアル名</param>
	/// <param name="[In_RemoveExtension]">拡張子を削除するかどうか</param>
	/// <returns>FBX名</returns>
	std::string ExtractFbxNameFromMaterialName(_In_ const std::string &In_MaterialName,_In_ const bool &In_RemoveExtension) noexcept;
}
