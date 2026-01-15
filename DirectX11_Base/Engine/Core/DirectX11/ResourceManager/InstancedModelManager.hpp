/*+===================================================================
	File: InstancedModelManager.hpp
	Summary: インスタンシング用モデルマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/09/01 Mon PM 03:59:15 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/DirectX11/Resource/MesheResource/InstancedMesh.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiMesh;

/// <summary>
/// InstancedModelManagerクラス
/// </summary>
class InstancedModelManager : public Singleton<InstancedModelManager>
{
	friend class Singleton<InstancedModelManager>;
public:

	/// <summary>
	/// 指定されたファイルからメッシュを取得します。
	/// </summary>
	/// <param name="[In_MeshName]">メッシュの固有名</param>
	/// <returns>取得したメッシュを保持する std::shared_ptr<Mesh> オブジェクト。</returns>
	std::shared_ptr<InstancedMesh> GetMesh(_In_ std::string_view In_MeshName) noexcept;

	/// <summary>
	/// メッシュを作成する
	/// </summary>
	/// <param name="[In_Mesh]">メッシュデータのソースとなる aiMesh オブジェクトへのポインタ。</param>
	/// <param name="[In_File]">メッシュの作成元ファイルのパス。</param>
	/// <param name="[In_Scale]">メッシュのスケール値。</param>
	/// <param name="[In_MeshIndex]">ファイル内のメッシュインデックス。</param>
	/// <param name="[In_Material]">メッシュに適用する Material オブジェクトへの共有ポインタ。</param>
	/// <returns>作成されたメッシュへの共有ポインタ。</returns>
	std::shared_ptr<InstancedMesh> CreateMesh(_In_ const aiMesh *In_Mesh, _In_ const FilePath &In_File, _In_ const float &In_Scale, _In_ const InstancedMesh::AlignInstanceData &In_InstanceData, _In_ const int In_MeshIndex, _In_ std::shared_ptr<Material> In_Material);

private:
	InstancedModelManager() = default;
	~InstancedModelManager();

private:
	std::unordered_map<std::string, std::shared_ptr<InstancedMesh>> m_mapMeshes; // モデルのリスト
};
