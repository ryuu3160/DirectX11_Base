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
#include "DirectX11/Resource/Meshes/MeshBuffer.hpp"
#include "DirectX11/Resource/Shaders/Shader.hpp"
#include "DirectX11/Resource/Texture.hpp"

class Sprite
{
public:
    struct SpriteVertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };

    struct SpriteData
    {
        std::shared_ptr<MeshBuffer> mesh;
        DirectX::XMFLOAT4X4 matrix[3] = {}; // Initialize to default values
        DirectX::XMFLOAT4 param[3] = {};    // Initialize to default values
        Texture *texture = nullptr;         // Initialize to nullptr
        Shader *vs = nullptr;               // Initialize to nullptr
        Shader *ps = nullptr;               // Initialize to nullptr
        bool Is3D = false;
        bool IsBillBoard = false;
    };

public:
    Sprite();
    ~Sprite();

   void Draw();

   void Load(_In_ const FilePath &In_File, _In_ const int& In_Layer, _In_ const float &In_Scale = 1.0f);

   /// <summary>
   /// 名前を取得します。
   /// </summary>
   /// <returns>メンバー変数 m_Name への参照。</returns>
   inline const std::string &GetName() const noexcept { return m_Name; }

   /// <summary>
   /// サイズを取得
   /// </summary>
   /// <returns>DirectX::XMFLOAT2型のサイズ</returns>
   inline const DirectX::XMFLOAT2 &GetSize() const noexcept { return DirectX::XMFLOAT2(m_SpriteData.param[0].z, m_SpriteData.param[0].w); }
   /// <summary>
   /// UV座標を取得
   /// </summary>
   /// <returns>DirectX::XMFLOAT2型のUV座標</returns>
   inline const DirectX::XMFLOAT2 &GetUVPos() const noexcept { return DirectX::XMFLOAT2(m_SpriteData.param[1].x, m_SpriteData.param[1].y); }
   /// <summary>
   /// UVスケールを取得
   /// </summary>
   /// <returns>DirectX::XMFLOAT2型のUVスケール</returns>
   inline const DirectX::XMFLOAT2 &GetUVScale() const noexcept { return DirectX::XMFLOAT2(m_SpriteData.param[1].z, m_SpriteData.param[1].w); }
   /// <summary>
   /// 色を取得
   /// </summary>
   /// <returns>DirectX::XMFLOAT4型の色</returns>
   inline DirectX::XMFLOAT4 &GetColor() noexcept { return m_SpriteData.param[2]; }

   /// <summary>
   /// 位置ベクトルを取得します。
   /// </summary>
   /// <returns>オブジェクトの位置を表す const DirectX::XMFLOAT3 型の参照。</returns>
   inline const DirectX::XMFLOAT3 &GetPosition() const noexcept { return m_Position; }

   /// <summary>
   /// スケール値（DirectX::XMFLOAT3型）への参照を取得します。
   /// </summary>
   /// <returns>オブジェクトのスケール値を表すDirectX::XMFLOAT3型への定数参照。</returns>
   inline const DirectX::XMFLOAT3 &GetScale() const noexcept { return m_Scale; }

   /// <summary>
   /// 回転ベクトルを取得します。
   /// </summary>
   /// <returns>オブジェクトの回転を表す const DirectX::XMFLOAT3 型の参照。</returns>
   inline const DirectX::XMFLOAT3 &GetRotation() const noexcept { return m_Rotation; }

   /// <summary>
   /// クォータニオンを取得する
   /// </summary>
   /// <returns>Rotationをクォータニオンに変換したもの</returns>
   inline const DirectX::XMFLOAT4 GetQuaternion() const noexcept
   {
       DirectX::XMFLOAT4 work;
       DirectX::XMStoreFloat4(&work,DirectX::XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z));
       return work;
   }

   /// <summary>
   /// 法線ベクトルを取得します。
   /// </summary>
   /// <returns>このオブジェクトの法線ベクトルへの参照。</returns>
   const DirectX::XMVECTOR GetNormal() const noexcept;

   /// <summary>
   /// 3D空間に配置されているかどうか
   /// </summary>
   /// <returns>スプライトが3Dである場合はtrue、そうでない場合はfalseを参照で返します。</returns>
   inline const bool &GetIs3D() const noexcept { return m_SpriteData.Is3D; }

   /// <summary>
   /// スプライトがビルボードかどうかを示すフラグを取得します。
   /// </summary>
   /// <returns>ビルボードである場合は true、そうでない場合は false を指す定数参照。</returns>
   inline const bool &GetIsBillBoard() const noexcept { return m_SpriteData.IsBillBoard; }

   /// <summary>
   /// レイヤー値を取得します。
   /// </summary>
   /// <returns>レイヤー値への定数参照。</returns>
   inline const int &GetLayer() const noexcept { return m_Layer; }

   /// <summary>
   /// ファイルパスを取得します。
   /// </summary>
   /// <returns>ファイルパスを表す std::string への参照。</returns>
   inline const FilePathHold &GetFilePath() const noexcept { return m_FilePath; }

   /// <summary>
   /// 名前を設定します。
   /// </summary>
   /// <param name="[In_Name]">設定する名前。</param>
   inline void SetName(_In_ const std::string &In_Name) noexcept { m_Name = In_Name; }

   inline void SetFilePath(_In_ const FilePathHold &In_FilePath) noexcept { m_FilePath = In_FilePath; }

   inline void SetLayer(_In_ const int &In_Layer) noexcept { m_Layer = In_Layer; }

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
   /// 位置を設定します。
   /// </summary>
   /// <param name="In_Pos">設定する位置を表す DirectX::XMFLOAT3 型の参照。</param>
   void SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;

   /// <summary>
   /// スケール値を設定します。
   /// </summary>
   /// <param name="In_Scale">設定するスケール値（DirectX::XMFLOAT3 型の参照）。</param>
   void SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept;

   /// <summary>
   /// 回転を指定された値に設定します。
   /// </summary>
   /// <param name="In_Rot">設定する回転値（DirectX::XMFLOAT3 型の参照）。</param>
   void SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rot) noexcept;

   /// <summary>
   /// 3Dモードを設定します。
   /// </summary>
   /// <param name="[In_Is3D]">3Dモードを有効にする場合はtrue、無効にする場合はfalseを指定します。</param>
   void Set3D(_In_ const bool &In_Is3D) noexcept;

   /// <summary>
   /// <para>ビルボード状態を設定します。</para>
   /// <para>3Dが有効な場合のみ効果が反映されます。</para>
   /// </summary>
   /// <param name="[In_IsBillBoard]">ビルボード状態に設定するかどうかを示すブール値（true でビルボード、false で非ビルボード）。</param>
   void SetBillBoard(_In_ const bool &In_IsBillBoard) noexcept;

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
   /// ビルボード用のビュー行列を設定します。
   /// </summary>
   /// <param name="[In_View]">設定するDirectX::XMFLOAT4X4型のビュー行列。</param>
   void SetBillBoardView(_In_ DirectX::XMFLOAT4X4 In_View) noexcept;

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
	std::string m_Name; // スプライトの名前
    SpriteData m_SpriteData;
    DirectX::XMFLOAT3 m_Position;  // 座標
    DirectX::XMFLOAT3 m_Scale;     // スケール
    DirectX::XMFLOAT3 m_Rotation;  // 回転
    int m_Layer; // レイヤー

	FilePathHold m_FilePath; // テクスチャのファイルパス

    std::shared_ptr<VertexShader> m_defVS;
    std::shared_ptr<PixelShader> m_defPS;
    std::shared_ptr<Texture> m_whiteTex;
    DirectX::XMFLOAT4X4 m_BillBoardView;
};
