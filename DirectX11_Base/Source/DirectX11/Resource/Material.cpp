/*+===================================================================
	File: Material.cpp
	Summary: マテリアルクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:03:43 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Material.hpp"
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

Material::Material()
	: m_fDiffuse(1.0f, 1.0f, 1.0f, 1.0f)
	, m_fAmbient(1.0f, 1.0f, 1.0f, 1.0f)
	, m_fSpecular(1.0f, 1.0f, 1.0f, 1.0f)
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_strMaterialName("")
{
	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = m_defVS.get();
	m_pPS = m_defPS.get();
}

Material::~Material()
{
}

void Material::Load(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File, _In_ const std::string In_MaterialName) noexcept
{
	if (!In_pMaterial) return;

	// マテリアル名の設定
	m_strMaterialName = In_MaterialName;

	//--- マテリアルの作成
	// ファイルの探索
	std::string dir = std::string(In_File);
	dir = dir.substr(0, dir.find_last_of('/') + 1);
	// マテリアル
	aiColor3D color(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 ambient(0.3f, 0.3f, 0.3f, 1.0f);
	// 各種パラメーター
	float shininess;
	// 色情報
	m_fDiffuse = In_pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f) : diffuse;
	// 環境光の色
	m_fAmbient = In_pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f) : ambient;
	// 鏡面反射の色
	shininess = In_pMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS ? shininess : 0.0f;
	m_fSpecular = In_pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color.r, color.g, color.b, shininess) : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, shininess);

	// テクスチャの取得
	aiString path;
	m_spTexture = TextureManager::GetInstance().GetTexture(In_pMaterial);

	// テクスチャが見つからない場合は、エラーを出力
	if (m_spTexture == nullptr)
	{
		std::string ErrorMsg = "Texture not found\n";
		ErrorMsg += m_strMaterialName;
		ErrorMsg += " at ";

		if (path.length == 0)
			ErrorMsg += "Failed to load texture from material.";
		else
			ErrorMsg += path.C_Str();
		Error(ErrorMsg);
	}
}

std::shared_ptr<Texture> Material::GetTexture() const noexcept
{
	return m_spTexture;
}

void Material::MakeDefaultShader()
{
	m_defVS = std::make_shared<VertexShader>();
	m_defVS->Load("Assets/Shader/VS_Model.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Model.cso");
}
