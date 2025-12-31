/*+===================================================================
	File: Mesh.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/14 Thu PM 05:02:12 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/DirectX11/Resource/MesheResource/MeshBuffer.hpp"
#include "Engine/Core/DirectX11/Resource/Material.hpp"
#include "Engine/Core/DirectX11/Resource/ResourceSetting.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiMesh;

/// <summary>
/// Meshクラス
/// </summary>
class Mesh
{
public:
	/// <summary>
	/// 頂点データを表す構造体
	/// </summary>
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};

	Mesh();
	~Mesh();

	/// <summary>
	/// 指定されたメッシュ、スケール、マテリアルを使用してデータをロードします。
	/// </summary>
	/// <param name="[In_Mesh]">ロードする対象の aiMesh オブジェクトへのポインタ。</param>
	/// <param name="[In_Scale]">メッシュに適用するスケール値。</param>
	/// <param name="[In_Material]">メッシュに関連付ける Material オブジェクトの共有ポインタ。</param>
	void Load(_In_ const aiMesh *In_Mesh, _In_ const float &In_Scale, _In_ std::shared_ptr<Material> In_Material) noexcept;

	/// <summary>
	/// メッシュバッファへの共有ポインタを取得します。
	/// </summary>
	/// <returns>メッシュバッファ（MeshBuffer）への std::shared_ptr を返します。例外は発生しません。</returns>
	std::shared_ptr<MeshBuffer> GetMesh() const noexcept;

	/// <summary>
	/// メッシュに関連付けられたマテリアルを取得する
	/// </summary>
	/// <returns>マテリアルへのシェアポインタ</returns>
	std::shared_ptr<Material> GetMaterial() const noexcept;

	/// <summary>
	/// 指定された MeshBuffer で現在のメッシュバッファを置き換えます。
	/// </summary>
	/// <param name="In_MeshBuffer">置き換えに使用する MeshBuffer への共有ポインタ。</param>
	void ReplaceMeshBuffer(_In_ std::shared_ptr<MeshBuffer> In_MeshBuffer) noexcept;

private:
	std::shared_ptr<MeshBuffer> m_spMeshBuffer; // メッシュバッファ
	std::shared_ptr<Material> m_spMaterial; // マテリアル
};
