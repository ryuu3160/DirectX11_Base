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
		aiTextureType_DIFFUSE,		// ディフューズ
		aiTextureType_SPECULAR,		// スペキュラー(光沢)
		aiTextureType_AMBIENT,		// アンビエント(環境光)
		aiTextureType_EMISSIVE,		// エミッシブ(発光)
		aiTextureType_HEIGHT,		// ハイトマップ(高さマップ)
		aiTextureType_NORMALS,		// ノーマルマップ(法線マップ)
		aiTextureType_OPACITY,		// オパシティ(不透明度)
		aiTextureType_REFLECTION,	// リフレクション(反射)
		aiTextureType_BASE_COLOR,	// ベースカラー(PBRで使用)
		aiTextureType_TRANSMISSION,	// トランスミッション(透過)

		// --- Maya独自のテクスチャタイプ ---
		aiTextureType_MAYA_BASE,				// Mayaのベースカラー
		aiTextureType_MAYA_SPECULAR,			// Mayaのスペキュラー
		aiTextureType_MAYA_SPECULAR_COLOR,		// Mayaのスペキュラーカラー
		aiTextureType_MAYA_SPECULAR_ROUGHNESS,	// Mayaのスペキュラーラフネス
	};

	enum TextureType : unsigned int
	{
		TextureType_Diffuse = 0,    // ディフューズ
		TextureType_Specular,       // スペキュラー(光沢)
		TextureType_Ambient,        // アンビエント(環境光)
		TextureType_Emissive,       // エミッシブ(発光)
		TextureType_Height,         // ハイトマップ(高さマップ)
		TextureType_Normal,         // ノーマルマップ(法線マップ)
		TextureType_Opacity,        // オパシティ(不透明度)
		TextureType_Reflection,     // リフレクション(反射)
		TextureType_BaseColor,      // ベースカラー(PBRで使用)
		TextureType_Transmission,   // トランスミッション(透過)

		// --- Maya独自のテクスチャタイプ ---
		TextureType_Maya_Base,				// Mayaのベースカラー
		TextureType_Maya_Specular,			// Mayaのスペキュラー
		TextureType_Maya_SpecularColor,		// Mayaのスペキュラーカラー
		TextureType_Maya_SpecularRoughness,	// Mayaのスペキュラーラフネス

		TextureType_Max,            // 最大値（配列のサイズ取得用）

		TextureType_Unknown = 0xFFFFFFFF // 未知のテクスチャタイプ
	};

	/// <summary>
	/// aiTextureTypeの値を対応するTextureTypeに変換します。
	/// </summary>
	/// <param name="In_Type">変換するaiTextureType型の値。</param>
	/// <returns>対応するTextureType型の値。未知のタイプの場合はTextureType_Unknownを返します。</returns>
	inline const TextureType aiTextureTypeToTextureType(_In_ const aiTextureType &In_Type) noexcept
	{
		switch (In_Type)
		{
		case aiTextureType_DIFFUSE:    return TextureType_Diffuse;
		case aiTextureType_SPECULAR:   return TextureType_Specular;
		case aiTextureType_AMBIENT:    return TextureType_Ambient;
		case aiTextureType_EMISSIVE:   return TextureType_Emissive;
		case aiTextureType_HEIGHT:     return TextureType_Height;
		case aiTextureType_NORMALS:    return TextureType_Normal;
		case aiTextureType_OPACITY:    return TextureType_Opacity;
		case aiTextureType_REFLECTION: return TextureType_Reflection;
		case aiTextureType_BASE_COLOR: return TextureType_BaseColor;
		case aiTextureType_TRANSMISSION: return TextureType_Transmission;

		// --- Maya独自のテクスチャタイプ ---
		case aiTextureType_MAYA_BASE:				return TextureType_Maya_Base;
		case aiTextureType_MAYA_SPECULAR:			return TextureType_Maya_Specular;
		case aiTextureType_MAYA_SPECULAR_COLOR:		return TextureType_Maya_SpecularColor;
		case aiTextureType_MAYA_SPECULAR_ROUGHNESS:	return TextureType_Maya_SpecularRoughness;

		case aiTextureType_UNKNOWN: // 未知のテクスチャタイプ
		default:                       return TextureType_Unknown; // 未知のテクスチャタイプ
		}
	}

	/// <summary>
	/// マテリアル名を生成する関数
	/// </summary>
	/// <param name="[In_FilePath]">fbxファイルへのパス</param>
	/// <param name="[In_pMaterial]">マテリアルオブジェクトへのポインタ</param>
	/// <returns>マテリアル名</returns>
	std::string CreateMaterialName(_In_ const FilePath &In_FilePath, _In_ const aiMaterial *In_pMaterial) noexcept;

	/// <summary>
	/// FBXのマテリアル名から、FBXの名前を抽出する関数
	/// </summary>
	/// <param name="[In_MaterialName]">マテリアル名</param>
	/// <param name="[In_RemoveExtension]">拡張子を削除するかどうか</param>
	/// <returns>FBX名</returns>
	std::string ExtractFbxNameFromMaterialName(_In_ const std::string &In_MaterialName,_In_ const bool &In_RemoveExtension) noexcept;

	enum ShaderParamType : unsigned int
	{
		ShaderParam_Camera,				// カメラパラメーター
		ShaderParam_Light,				// 標準ライト
		ShaderParam_PointLight,			// ポイントライト
		ShaderParam_DirectionalLight,	// 平行光源(リムライトや太陽光など)
		ShaderParam_SpotLight,			// スポットライト
		ShaderParam_PBR,				// PBRマテリアル用パラメーター
		ShaderParam_POM,				// 視差遮蔽マップ用パラメーター

		ShaderParam_MAX, // 最大値（配列のサイズ取得用）
		ShaderParam_Unknown = 0xFFFFFFFF // 未知のパラメータータイプ
	};

	// パラメーターのベース構造体
	struct BaseParam
	{
	};

	// ライトのパラメーター構造体
	struct LightParam : public BaseParam
	{
		DirectX::XMFLOAT3 Direction; // ライトの方向
		float Dummy;
		DirectX::XMFLOAT4 Diffuse; // 拡散光の色
		DirectX::XMFLOAT4 Specular; // 鏡面反射光の色
	};
	// ポイントライトのパラメーター構造体
	struct PointLightParam : public BaseParam
	{
		DirectX::XMFLOAT3 Pos;		// ライトの位置
		float Range;				// ライトの範囲
		DirectX::XMFLOAT4 Color;	// 光源の色
	};
	// 平行光源のパラメーター構造体
	struct DirectionalLightParam : public BaseParam
	{
		DirectX::XMFLOAT3 Direction; // ライトの方向
		float Dummy;
		DirectX::XMFLOAT4 Diffuse; // 色
	};
	// スポットライトのパラメーター構造体
	struct SpotLightParam : public BaseParam
	{
		DirectX::XMFLOAT3 Pos;		// ライトの位置
		float Range;				// ライトの範囲
		DirectX::XMFLOAT4 Color;	// 光源の色
		DirectX::XMFLOAT3 Direction; // ライトの方向
		float Angle;				// スポットライトの照射角度
	};
	// PBRマテリアルのパラメーター構造体
	struct PBR_Param : public BaseParam
	{
		float Metallic;
		float Smooth;
		DirectX::XMFLOAT2 dummy;
	};

	struct POM_Param : public BaseParam
	{
		float HeightScale;			// 高さスケール
		int NumSteps;
		DirectX::XMFLOAT2 dummy;
	};

	// シェーダーに渡すパラメーターの基底クラス
	class ShaderParam
	{
	public:
		ShaderParam(ShaderParamType In_Type) : m_pParam(nullptr)
		{
			m_Type = In_Type; // パラメーターのタイプを設定
		}
		~ShaderParam()
		{
			if (m_pParam)
			{
				delete m_pParam;
				m_pParam = nullptr;
			}
		}

		/// <summary>
		/// パラメータを取得する
		/// </summary>
		/// <returns>パラメータへのポインタを返します。</returns>
		virtual void *GetParam() noexcept = 0;

		/// <summary>
		/// m_Type を unsigned int 型に変換して、スロット番号を取得します。
		/// </summary>
		/// <returns>m_Type を unsigned int 型にキャストした値（スロット番号）。</returns>
		inline const unsigned int GetSlotNum() const noexcept { return static_cast<unsigned int>(m_Type); }

	protected:
		ShaderParamType m_Type; // パラメーターのタイプ
		BaseParam *m_pParam;
	};

	class ShaderParamLight : public ShaderParam
	{
	public:
		ShaderParamLight(LightParam *In_Param) : ShaderParam(ShaderParamType::ShaderParam_Light)
		{
			m_pParam = In_Param;
		}

		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<LightParam *>(m_pParam)); }
	};

	class ShaderParamPointLight : public ShaderParam
	{
		public:
		ShaderParamPointLight(PointLightParam *In_Param) : ShaderParam(ShaderParamType::ShaderParam_PointLight)
		{
			m_pParam = In_Param;
		}
		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<PointLightParam *>(m_pParam)); }
	};
	class ShaderParamDirectionalLight : public ShaderParam
	{
		public:
		ShaderParamDirectionalLight(DirectionalLightParam *In_Param) : ShaderParam(ShaderParamType::ShaderParam_DirectionalLight)
		{
			m_pParam = In_Param;
		}
		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<DirectionalLightParam *>(m_pParam)); }
	};
	class ShaderParamSpotLight : public ShaderParam
	{
		public:
		ShaderParamSpotLight(SpotLightParam *In_Param) : ShaderParam(ShaderParamType::ShaderParam_SpotLight)
		{
			m_pParam = In_Param;
		}
		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<SpotLightParam *>(m_pParam)); }
	};
	class ShaderParamPBR : public ShaderParam
	{
		public:
		ShaderParamPBR(PBR_Param *In_Param) : ShaderParam(ShaderParamType::ShaderParam_PBR)
		{
			m_pParam = In_Param;
		}
		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<PBR_Param *>(m_pParam)); }
	};
	class ShaderParamPOM : public ShaderParam
	{
		public:
		ShaderParamPOM(POM_Param *In_Param) : ShaderParam(ShaderParamType::ShaderParam_POM)
		{
			m_pParam = In_Param;
		}
		void *GetParam() noexcept override { return static_cast<void *>(reinterpret_cast<POM_Param *>(m_pParam)); }
	};
}
