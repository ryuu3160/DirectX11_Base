/*+===================================================================
	File: TextureManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/15 Fri AM 01:22:55 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "TextureManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// assimpのライブラリリンク
#ifdef _DEBUG
#pragma comment(lib, "assimp-vc143-mtd.lib")
#else
#pragma comment (lib, "assimp-vc143-mt.lib")
#endif

std::shared_ptr<Texture> TextureManager::GetTexture(_In_ const FilePath &In_FilePath) noexcept
{
	auto itr = m_mapTextures.find(HoldFilePath(In_FilePath));

	if (itr != m_mapTextures.end())
	{
		return itr->second; // 既存のテクスチャを返す
	}

	return nullptr; // テクスチャが見つからない場合はnullptrを返す
}

void TextureManager::LoadTextures(_In_ const aiScene *In_Scene, _In_ const FilePath &In_File) noexcept
{
	HRESULT hr;

	// ファイルの探索
	std::string dir = std::string(In_File);
	dir = dir.substr(0, dir.find_last_of('/') + 1);

	for (unsigned int i = 0; i < In_Scene->mNumMaterials; ++i)
	{
		// テクスチャ
		aiString path;
		if (In_Scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			// そのまま探索
			auto Tex = std::make_shared<Texture>();
			hr = Tex->Create(path.C_Str());
			// モデルと同じ階層を探索
			if (FAILED(hr))
			{
				std::string file = dir;
				file += path.C_Str();
				hr = Tex->Create(file.c_str());
			}
			// ファイル名のみで探索
			if (FAILED(hr))
			{
				std::string file = path.C_Str();
				size_t idx = file.find_last_of('\\');
				if (idx != std::string::npos)
				{
					file = file.substr(idx + 1);
					file = dir + file;
					hr = Tex->Create(file.c_str());
				}
			}
			// 失敗
			if (FAILED(hr))
			{
				Tex = nullptr;
			}
			else
			{
				// 成功した場合はマップに追加
				m_mapTextures.insert({ HoldFilePath(path.C_Str()), Tex });
			}
		}
	}

	for (int i = 0; i < In_Scene->mNumTextures; i++)
	{
		aiTexture *aiTex = In_Scene->mTextures[i];

		auto Tex = std::make_shared<Texture>();

		// テクスチャ読み込み
		hr = Tex->Create(aiTex);

		// 埋め込みテクスチャの場合はファイル名でマップに登録
		if (SUCCEEDED(hr))
		{
			// 成功した場合はマップに追加
			m_mapTextures.insert({ HoldFilePath(aiTex->mFilename.data), Tex });
		}
	}
}

TextureManager::TextureManager()
{
}
TextureManager::~TextureManager()
{
}
