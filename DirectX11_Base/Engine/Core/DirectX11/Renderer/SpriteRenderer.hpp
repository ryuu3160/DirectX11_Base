/*+===================================================================
	File: SpriteRenderer.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/24 Thu AM 01:09:42 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Engine/Core/DirectX11/Resource/MesheResource/MeshBuffer.hpp"

/// <summary>
/// SpriteRendererクラス
/// </summary>
class SpriteRenderer : public RenderComponent
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
	SpriteRenderer();
	~SpriteRenderer();

	void Awake() noexcept override final;

	void Update(_In_ float In_DeltaTime) noexcept override final;

	void SetVertexShader(_In_ Shader *In_Vs) noexcept;
	void SetPixelShader(_In_ Shader *In_Ps) noexcept;

	/// <summary>
	/// テクスチャを読みこみ、2Dスプライトを生成する
	/// </summary>
	/// <param name="[In_File]">テクスチャへのパス</param>
	/// <param name="[In_Scale]">適用するスケール値。デフォルトは1.0f。</param>
	void Load(_In_ const FilePath &In_File, _In_ const float &In_Scale = 1.0f);

	/// <summary>
	/// 保存されているデータを使用し、2Dスプライトを生成する
	/// </summary>
	void Load() noexcept;

	/// <summary>
	/// 指定されたテクスチャスロットに描画を行います。
	/// </summary>
	void Draw(_In_ RenderContext *In_RenderContext) noexcept override final;

	/// <summary>
	/// オフセット値を取得
	/// </summary>
	/// <returns>DirectX::XMFLOAT2型のオフセット値</returns>
	inline const DirectX::XMFLOAT2 &GetOffset() const noexcept { return DirectX::XMFLOAT2(m_SpriteData.param[0].x, m_SpriteData.param[0].y); }
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
	inline const DirectX::XMFLOAT4 &GetColor() const noexcept { return m_SpriteData.param[2]; }

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
	/// 描画時にピクセル位置を使用するかどうかを示すフラグを取得します。
	/// </summary>
	/// <returns>ピクセル位置を使用する場合は true、使用しない場合は false を返します。</returns>
	inline const bool &GetIsUsePixelPosition() const noexcept { return m_bUsePixelPosition; }

	/// <summary>
	/// ピクセル単位の位置を取得します。
	/// </summary>
	/// <returns>DirectX::XMFLOAT3 型の位置情報への参照を返します。</returns>
	const DirectX::XMFLOAT3 &GetPositionPixel() noexcept;

	/// <summary>
	/// 2次元オフセット値を設定します。
	/// </summary>
	/// <param name="In_Offset">設定するDirectX::XMFLOAT2型のオフセット値。</param>
	void SetOffset(_In_ const DirectX::XMFLOAT2 &In_Offset) noexcept;
	/// <summary>
	/// 指定されたサイズでオブジェクトのサイズを設定します。(単位はピクセル)
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
	/// 描画時にピクセル位置を使用するかどうかを設定します。
	/// </summary>
	/// <param name="[In_Use]">ピクセル位置を使用するかどうかを示す真偽値。</param>
	inline void SetUsePixelPosition(_In_ const bool &In_Use) noexcept { m_bUsePixelPosition = In_Use; }

	/// <summary>
	/// <para>ピクセル単位の位置を設定します。</para>
	/// <para>オブジェクトのTransformコンポーネントの位置も同時に更新されます。</para>
	/// </summary>
	/// <param name="[In_Pos]">ピクセル単位の座標</param>
	void SetPositionPixel(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;

private:
	/// <summary>
	/// デフォルトのシェーダーを作成
	/// </summary>
	void MakeDefaultShader() noexcept;

	void MakeDefaultTexture() noexcept;

private:
	SpriteData m_SpriteData;				// スプライトデータ
	DirectX::XMFLOAT3 m_PositionPixel;		// ピクセル単位の位置
	bool m_bIsLoaded;						// 読み込み済みかどうか
	bool m_bUsePixelPosition;				// 描画時にピクセル位置を使用するかどうか
	std::shared_ptr<Texture> m_DefaultTex;	// デフォルトのテクスチャ
};
