/*+===================================================================
	File: Model.hpp
	Summary: assimpを利用したモデル描画クラス 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/01 Tue AM 10:47:04 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/Texture/MeshBuffer.hpp"
#include "DirectX11/Texture/Texture.hpp"

/// <summary>
/// Modelクラス
/// </summary>
class Model
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
	/// マテリアル構造体
	/// </summary>
	struct Material
	{
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 specular;
		std::shared_ptr<Texture> texture;
	};

	using Materials = std::vector<Material>;

	/// <summary>
	/// メッシュデータとマテリアルIDを保持する構造体
	/// </summary>
	struct Mesh
	{
		std::shared_ptr<MeshBuffer> mesh;
		unsigned int materialID;
	};
	using Meshes = std::vector<Mesh>;

	/// <summary>
	/// 頂点とインデックスデータの情報を格納する構造体
	/// </summary>
	struct RemakeInfo
	{
		UINT vtxNum;
		void *dest;
		const void *source;
		UINT idxNum;
		const void *idx;
	};

public:
	Model();
	~Model();
	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	/// <summary>
	/// 指定されたインデックスのメッシュを取得します。
	/// </summary>
	/// <param name="[In_Index]">取得したいメッシュのインデックス。</param>
	/// <returns>インデックスが有効な場合は対応するMeshオブジェクトへのポインタ。無効な場合はnullptrを返します。</returns>
	inline const Mesh *GetMesh(_In_ const unsigned int &In_Index) const noexcept
	{
		if (In_Index < 0 || m_vecMeshes.size() <= In_Index)
		{
			return nullptr;
		}
		return &m_vecMeshes[In_Index];
	}

	/// <summary>
	/// メッシュの数を取得します
	/// </summary>
	/// <returns>メッシュの数を表す定数参照（unsigned int型）を返します。</returns>
	inline const unsigned int& GetMeshNum() const noexcept { return static_cast<uint32_t>(m_vecMeshes.size()); }


	/// <summary>
	/// ファイルを指定されたパスから読み込み、スケールと反転オプションを適用します。
	/// </summary>
	/// <param name="[In_File]">読み込むファイルのパス。</param>
	/// <param name="[In_Scale]">適用するスケール係数（デフォルトは1.0f）。</param>
	/// <param name="[In_IsFlip]">ファイルを反転して読み込むかどうか（デフォルトはfalse）。</param>
	/// <returns>読み込みに成功した場合はtrue、失敗した場合はfalseを返します。</returns>
	bool Load(_In_ const FilePath &In_File, _In_ const float &In_Scale = 1.0f, _In_ const bool &In_IsFlip = false);

	/// <summary>
	/// 指定されたテクスチャスロットに描画を行います。
	/// </summary>
	/// <param name="[In_TexSlot]">使用するテクスチャスロットのインデックス。デフォルトは0です。</param>
	void Draw(_In_ const int &In_TexSlot = 0);

	/// <summary>
	/// 頂点データを再生成するための関数を呼び出します。
	/// </summary>
	/// <param name="[In_VtxSize]">再生成する頂点データのサイズ。</param>
	/// <param name="[In_Func]">RemakeInfo 構造体への参照を受け取り、頂点データの再生成処理を行うコールバック関数。</param>
	void RemakeVertex(_In_ const int &In_VtxSize, _In_ std::function<void(RemakeInfo &data)> In_Func);

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader();

private:
	static std::shared_ptr<VertexShader> m_defVS;
	static std::shared_ptr<PixelShader> m_defPS;

	Meshes m_vecMeshes;
	Materials m_vecMaterials;
	VertexShader *m_pVS;
	PixelShader *m_pPS;
};
