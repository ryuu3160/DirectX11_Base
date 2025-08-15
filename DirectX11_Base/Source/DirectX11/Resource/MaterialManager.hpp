/*+===================================================================
	File: MaterialManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:03:09 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Material.hpp"

// ==============================
//  前方宣言
// ==============================
struct aiMaterial;

/// <summary>
/// MaterialManagerクラス
/// </summary>
class MaterialManager : public Singleton<MaterialManager>
{
	friend class Singleton<MaterialManager>;
public:

	/// <summary>
	/// 指定された aiMaterial オブジェクトとファイルパスから Material オブジェクトを取得します。
	/// </summary>
	/// <param name="[In_pMaterial]">取得元となる aiMaterial オブジェクトへのポインタ。</param>
	/// <param name="[In_File]">関連するファイルのパス。</param>
	/// <returns>Material オブジェクトへの std::shared_ptr。取得に失敗した場合は nullptr を返します。</returns>
	std::shared_ptr<Material> GetMaterial(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File) noexcept;

private:
	MaterialManager();
	~MaterialManager() override;

	/// <summary>
	/// 指定されたパラメータから新しいMaterialオブジェクトを作成し、std::shared_ptrで返します。
	/// </summary>
	/// <param name="[In_pMaterial]">aiMaterial型のポインタ。新しいMaterialの元となるマテリアルデータ。</param>
	/// <param name="[In_File]">FilePath型の参照。マテリアルに関連するファイルパス。</param>
	/// <param name="[In_MaterialName]">std::string型。作成するマテリアルの名前。</param>
	/// <returns>作成されたMaterialオブジェクトへのstd::shared_ptr。</returns>
	std::shared_ptr<Material> CreateMaterial(_In_ const aiMaterial *In_pMaterial, _In_ const FilePath &In_File, _In_ const std::string In_MaterialName) noexcept;

private:
	std::unordered_map<std::string,std::shared_ptr<Material>> m_mapMaterials; // マテリアルのリスト
};
