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
}
