/*+===================================================================
	File: Sprite.hpp
	Summary: スプライトクラス 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 05:27:48 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Texture/MeshBuffer.hpp"
#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/Texture/Texture.hpp"

class Sprite : public Singleton<Sprite>  
{  
   friend class Singleton<Sprite>;  
public:  
   void Init();  
   void Uninit();  
   void Draw();  

   /// <summary>
   /// 2次元オフセット値を設定します。
   /// </summary>
   /// <param name="In_Offset">設定するDirectX::XMFLOAT2型のオフセット値。</param>
   void SetOffset(_In_ const DirectX::XMFLOAT2 &In_Offset) noexcept;
   /// <summary>
   /// 指定されたサイズでオブジェクトのサイズを設定します。
   /// </summary>
   /// <param name="In_Size">新しく設定するサイズを表す DirectX::XMFLOAT2 構造体への参照。</param>
   void SetSize(_In_ const DirectX::XMFLOAT2 &In_Size) noexcept;
   /// <summary>
   /// UV座標を設定します。
   /// </summary>
   /// <param name="In_Pos">設定するUV座標（DirectX::XMFLOAT2型）の参照。</param>
   void SetUVPos(_In_ const DirectX::XMFLOAT2 &In_Pos) noexcept;
   /// <summary>
   /// UVスケールを設定します。
   /// </summary>
   /// <param name="In_Scale">設定するUVスケール値（DirectX::XMFLOAT2型の参照）。</param>
   void SetUVScale(_In_ const DirectX::XMFLOAT2 &In_Scale) noexcept;
   /// <summary>
   /// 指定された色を設定します。
   /// </summary>
   /// <param name="In_Color">設定する色を表す DirectX::XMFLOAT4 型の参照。</param>
   void SetColor(_In_ const DirectX::XMFLOAT4 &In_Color) noexcept;
   /// <summary>
   /// テクスチャを設定します。
   /// </summary>
   /// <param name="In_pTex">設定するテクスチャへのポインタ。</param>
   void SetTexture(_In_ Texture *In_pTex) noexcept;

   /// <summary>
   /// ワールド行列を設定します。
   /// </summary>
   /// <param name="In_World">設定する DirectX::XMFLOAT4X4 型のワールド行列。</param>
   void SetWorld(_In_ DirectX::XMFLOAT4X4 In_World) noexcept;
   /// <summary>
   /// ビュー行列を設定します。
   /// </summary>
   /// <param name="In_View">設定するDirectX::XMFLOAT4X4型のビュー行列。</param>
   void SetView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept;
   /// <summary>
   /// 投影行列を設定します。
   /// </summary>
   /// <param name="In_Proj">設定するDirectX::XMFLOAT4X4型の投影行列。</param>
   void SetProjection(_In_ DirectX::XMFLOAT4X4 In_Proj) noexcept;

   /// <summary>
   /// 頂点シェーダーを設定します。
   /// </summary>
   /// <param name="In_Vs">設定するシェーダーへのポインタ。</param>
   void SetVertexShader(_In_ Shader *In_Vs) noexcept;
   /// <summary>
   /// ピクセルシェーダーを設定します。
   /// </summary>
   /// <param name="In_Ps">設定するピクセルシェーダーへのポインタ。</param>
   void SetPixelShader(_In_ Shader *In_Ps) noexcept;

private:  

   Sprite();  
   ~Sprite();  

   struct Data  
   {  
       std::shared_ptr<MeshBuffer> mesh;  
       DirectX::XMFLOAT4X4 matrix[3] = {}; // Initialize to default values  
       DirectX::XMFLOAT4 param[3] = {};    // Initialize to default values  
       Texture *texture = nullptr;        // Initialize to nullptr  
       Shader *vs = nullptr;              // Initialize to nullptr  
       Shader *ps = nullptr;              // Initialize to nullptr  
   };
   Data m_SpriteData;  
   std::shared_ptr<VertexShader> m_defVS;  
   std::shared_ptr<PixelShader> m_defPS;  
   std::shared_ptr<Texture> m_whiteTex;  
};
