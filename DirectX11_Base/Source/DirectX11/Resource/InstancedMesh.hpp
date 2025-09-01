/*+===================================================================
	File: InstancedMesh.hpp
	Summary: インスタンシング用のメッシュクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 04:01:02 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Texture/InstancedMeshBuffer.hpp"
#include "DirectX11/Resource/Material.hpp"
#include "ResourceSetting.hpp"

// ==============================
//	前方宣言
// ==============================
struct aiMesh;

/// <summary>
/// InstancedMeshクラス
/// </summary>
class InstancedMesh
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

	/// <summary>
	/// インスタンシング用のデータ構造体
	/// </summary>
	struct PerInstanceData
	{
		DirectX::XMFLOAT3 pos;
		float dummy; // 16バイトアラインメントのためのダミー
		DirectX::XMFLOAT3 scale;
		float dummy2; // 16バイトアラインメントのためのダミー
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT4 color;
	};

	InstancedMesh();
	~InstancedMesh();

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
	/// <returns>インスタンスメッシュバッファ（InstancedMeshBuffer）への std::shared_ptr を返します。例外は発生しません。</returns>
	std::shared_ptr<InstancedMeshBuffer> GetMesh() const noexcept;

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
	std::shared_ptr<InstancedMeshBuffer> m_spMeshBuffer; // メッシュバッファ
	std::shared_ptr<Material> m_spMaterial; // マテリアル
};
