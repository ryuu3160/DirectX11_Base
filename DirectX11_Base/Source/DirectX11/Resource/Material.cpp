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
#include "DirectX11/ResourceManager/TextureManager.hpp"
#include "DirectX11/ResourceManager/ShaderManager.hpp"

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
	, m_bIsInstancedVertexShader(false)
{
	m_strDirectory.clear();

	if (!m_defVS && !m_defPS) // どちらもnullptr
	{
		MakeDefaultShader();
	}
	m_pVS = m_defVS.get();
	m_pPS = m_defPS.get();

	for(auto &itr : m_spTextures)
	{
		itr = nullptr; // テクスチャの初期化
	}

	m_strVSName.clear();
	m_strPSName.clear();
	m_vecShaderParamList.clear();
}

Material::~Material()
{
	// マテリアル情報を保存
	SaveMaterialShaderInfo(m_strDirectory); // マテリアルシェーダー情報を保存
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
	m_strDirectory = dir; // ディレクトリの保存
	// マテリアル
	aiColor3D color(0.0f, 0.0f, 0.0f);
	aiColor4D color4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 ambient(0.3f, 0.3f, 0.3f, 1.0f);
	// 各種パラメーター
	float shininess;
	// 色情報
	m_fDiffuse = In_pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color4) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color4.r, color4.g, color4.b, color4.a) : diffuse;
	// 環境光の色
	m_fAmbient = In_pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color.r, color.g, color.b, 1.0f) : ambient;
	// 鏡面反射の色
	shininess = In_pMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS ? shininess : 0.0f;
	m_fSpecular = In_pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS ?
		DirectX::XMFLOAT4(color.r, color.g, color.b, shininess) : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, shininess);

	// テクスチャの取得
	aiString path;
	for (auto type : ResourceSetting::TextureTypes)
	{
		if (In_pMaterial->GetTexture(type, 0, &path) == AI_SUCCESS)
		{
			auto TexType = ResourceSetting::aiTextureTypeToTextureType(type);
			m_spTextures[TexType] = TextureManager::GetInstance().GetTexture(path.C_Str());
			break; // 最初に見つかったテクスチャを使用
		}
	}

	// テクスチャが見つからない場合は、エラーを出力
	if (m_spTextures.empty())
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

	// シェーダーの取得
	if (LoadShaderInfo(dir))
	{
		auto &ShaderM = ShaderManager::GetInstance();
		if (!m_strVSName.empty())
			m_pVS = reinterpret_cast<VertexShader *>(ShaderM.GetShader(m_strVSName));
		if(!m_strPSName.empty())
			m_pPS = reinterpret_cast<PixelShader *>(ShaderM.GetShader(m_strPSName));
	}
}

std::shared_ptr<Texture> Material::GetTexture(_In_ const ResourceSetting::TextureType &In_Type) const noexcept
{
	return m_spTextures[In_Type];
}

std::array<std::shared_ptr<Texture>, ResourceSetting::TextureType_Max> Material::GetTextures() const noexcept
{
	return m_spTextures;
}

int Material::GetTextureNum() const noexcept
{
	int count = 0; 	// テクスチャの数をカウント
	for (const auto &itr : m_spTextures)
	{
		if (itr)
			++count; // テクスチャが存在する場合はカウント
	}
	return count; // テクスチャの数を返す
}

void Material::SetTexture(_In_ const ResourceSetting::TextureType &In_Type, _In_ std::shared_ptr<Texture> In_pTexture) noexcept
{
	if (In_Type >= ResourceSetting::TextureType_Max) return; // 範囲外の値の場合は何もしない
	m_spTextures[In_Type] = In_pTexture;
}

void Material::MakeDefaultShader()
{
	m_defVS = std::make_shared<VertexShader>();
	m_defVS->Load("Assets/Shader/VS_Model.cso");
	m_defPS = std::make_shared<PixelShader>();
	m_defPS->Load("Assets/Shader/PS_Model.cso");
}

bool Material::LoadShaderInfo(_In_ const std::string_view &In_Directory) noexcept
{
	std::fstream file;
	std::string FbxName = ResourceSetting::ExtractFbxNameFromMaterialName(m_strMaterialName, true);

	// ファイルを開く
	std::string path = std::string(In_Directory) + FbxName + "_MaterialShaderInfo.csv";
	file.open(path, std::ios::in);
	if (!file.is_open())
	{
		// ファイルが存在しない場合はデフォルトシェーダーを使用
		SaveMaterialShaderInfo(In_Directory); // マテリアルシェーダー情報を保存
		return false; // ファイルが開けなかった場合はfalseを返す
	}

	// ファイルを1行ずつ読み込み、マテリアル名を検索
	std::string line;
	std::string name;
	for (; std::getline(file, line);)
	{
		name = line.substr(0, line.find(','));
		if (name == m_strMaterialName)
			break;
	}
	// ファイルの終端に達した場合はマテリアル名が見つからなかったことを示す
	if (!file.good())
	{
		file.close();
		return false; // ファイルの読み込みに失敗した場合はfalseを返す
	}

	file.close();// ファイルを閉じる

	// 1つ目のデータはマテリアル名なのでスキップ
	size_t pos = line.find(',');
	size_t nextPos = line.find(',', pos + 1);
	size_t length = nextPos - (pos + 1);
	std::string Info;
	for (nextPos; pos != std::string::npos;pos = line.find(',',pos + 1))
	{
		// カンマの位置から次のカンマの位置までの長さを計算
		// 次のカンマが見つからない場合は行の終わりまでの長さを計算
		if (nextPos == std::string::npos)
			length = line.length() - (pos); // 行の終わりまでの長さを計算
		else
			length = nextPos - (pos + 1);

		// マテリアル情報の抽出
		Info = line.substr(pos + 1, length);

		// シェーダー名の設定
		if(Info.find("VS_") != std::string::npos)
		{
			m_strVSName = Info;
		}
		else if (Info.find("PS_") != std::string::npos)
		{
			m_strPSName = Info;
		}
		else if (Info.find("PSWriteInfo:") != std::string::npos)
		{
			LoadWriteParam(Info);
		}

		// 次のカンマの位置を探す
		if(nextPos != std::string::npos)
			nextPos = line.find(',', nextPos + 1);
	}

	if(m_strVSName.empty() && m_strPSName.empty())
	{
		return false;
	}

	return true;
}

void Material::LoadWriteParam(_In_ const std::string_view &In_WriteParam) noexcept
{
	std::string WriteParam = std::string(In_WriteParam);

	WriteParam = WriteParam.substr(WriteParam.find(':') + 1);
	WriteParam = WriteParam.substr(WriteParam.find('{') + 1); // 最初の{以降を取得

	size_t pos = WriteParam.find('}');
	for (; pos != std::string::npos;pos = WriteParam.find('}'))
	{
		// }の位置までを取得
		std::string ParamStr = WriteParam.substr(0, pos);
		ShaderParamInfo Info{};
		// -の位置で分割してパラメータ名とスロット番号を取得
		Info.ParamName = ParamStr.substr(0, ParamStr.find('-'));
		std::string SlotStr = ParamStr.substr(ParamStr.find('-') + 1);
		std::from_chars(SlotStr.data(), SlotStr.data() + SlotStr.size(), Info.SlotNum);

		// リストに追加
		m_vecShaderParamList.push_back(Info);

		// 次の{以降を取得
		WriteParam = WriteParam.substr(pos + 1);
		WriteParam = WriteParam.substr(WriteParam.find('{') + 1);
	}
}

void Material::SaveMaterialShaderInfo(_In_ const std::string_view &In_Directory) const noexcept
{
	std::fstream file;
	std::string FbxName = ResourceSetting::ExtractFbxNameFromMaterialName(m_strMaterialName, true);

	std::string path = std::string(In_Directory) + FbxName + "_MaterialShaderInfo.csv";

	// ファイルを開く
	file.open(path, std::ios::out | std::ios::in | std::ios::ate);
	if (file.is_open())
	{
		file.seekp(0, std::ios::beg); // 先頭にシーク

		// 同じマテリアル名の行が存在する場合はそのマテリアル行のみを更新する
		std::string line;
		bool found = false;
		auto pos = file.tellg(); // 初めの位置を取得
		for (; std::getline(file, line);pos = file.tellg())
		{
			if (line.find(m_strMaterialName) != std::string::npos)
			{
				found = true; // マテリアル名がヒットした
				break;
			}
		}

		// マテリアル名がヒットした場合はその行を上書き
		if (found)
		{
			std::vector<std::string> lines; // 以降の行を保存するためのベクター
			// ファイルの終端に達していない場合のみ
			if (file.good())
			{
				// マテリアル名がヒットした場合は以降の行を保存
				for (; std::getline(file, line);)
				{
					lines.push_back(line); // 以降の行を保存
				}
			}

			// 上書き実行
			file.seekp(pos, std::ios::beg); // マテリアル名の行に書き込みポインタを移動

			// シェーダー名を書き込む
			file << m_strMaterialName << "," << m_strVSName << "," << m_strPSName;
			// PSに書き込む情報がある場合はそれも書き込む
			if (!m_vecShaderParamList.empty())
			{
				file << ",PSWriteInfo:";
				for (auto &itr : m_vecShaderParamList)
				{
					file << "{" << itr.ParamName << "-" << itr.SlotNum << "}";
				}
			}
			file << "\n"; // 改行

			// 以降の行を書き戻す
			for (auto &itr : lines)
			{
				file << itr << "\n";
			}
			file.close(); // ファイルを閉じる
			return; // 上書きが完了したので終了
		}

		// マテリアル名がヒットしなかった場合は新規追加
		file.close(); // ファイルを閉じる
	}

	// ファイルを開く（追記モード）
	file.open(path, std::ios::out | std::ios::app);

	if (!file.is_open())
	{
		Error("Failed to open file for saving material shader info: " + path);
		return;
	}

	// シェーダー名を書き込む
	file << m_strMaterialName << "," << m_strVSName << "," << m_strPSName;

	// PSに書き込む情報がある場合はそれも書き込む
	if (!m_vecShaderParamList.empty())
	{
		file << ",PSWriteInfo:";
		for (auto &itr : m_vecShaderParamList)
		{
			file << "{" << itr.ParamName << "-" << itr.SlotNum << "}";
		}
	}

	file << "\n";
	file.close();
}
