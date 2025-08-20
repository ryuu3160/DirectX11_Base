/*+===================================================================
	File: ModelManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:02:29 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Mesh.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiMesh;

/// <summary>
/// ModelManagerクラス
/// </summary>
class ModelManager : public Singleton<ModelManager>
{
	friend class Singleton<ModelManager>;
public:

	/// <summary>
	/// 指定されたファイルからメッシュを取得します。
	/// </summary>
	/// <param name="[In_MeshName]">メッシュの固有名</param>
	/// <returns>取得したメッシュを保持する std::shared_ptr<Mesh> オブジェクト。</returns>
	std::shared_ptr<Mesh> GetMesh(_In_ const std::string_view &In_MeshName) noexcept;

	/// <summary>
	/// メッシュを作成する
	/// </summary>
	/// <param name="[In_Mesh]">メッシュデータのソースとなる aiMesh オブジェクトへのポインタ。</param>
	/// <param name="[In_File]">メッシュの作成元ファイルのパス。</param>
	/// <param name="[In_Scale]">メッシュのスケール値。</param>
	/// <param name="[In_MeshIndex]">ファイル内のメッシュインデックス。</param>
	/// <param name="[In_Material]">メッシュに適用する Material オブジェクトへの共有ポインタ。</param>
	/// <returns>作成されたメッシュへの共有ポインタ。</returns>
	std::shared_ptr<Mesh> CreateMesh(_In_ const aiMesh *In_Mesh, _In_ const FilePath &In_File, _In_ const float &In_Scale, _In_ const int In_MeshIndex, _In_ std::shared_ptr<Material> In_Material);

private:
	ModelManager() = default;
	~ModelManager() = default;

private:
	std::unordered_map<std::string, std::shared_ptr<Mesh>> m_mapMeshes; // モデルのリスト
};
