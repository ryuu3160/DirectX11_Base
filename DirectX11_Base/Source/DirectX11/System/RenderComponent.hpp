/*+===================================================================
	File: RenderComponent.hpp
	Summary: 描画用の基底コンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/20 Sun PM 10:33:20 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Component.hpp"
#include "DirectX11/Texture/Shader.hpp"
#include "DirectX11/Texture/MeshBuffer.hpp"
#include "DirectX11/Texture/Texture.hpp"

// ==============================
//  前方宣言
// ==============================
class Camera;
class RenderManager;

// ==============================
//  列挙型
// ==============================
enum RenderLayer : int
{
	RenderLayer_SkyBox = -100,	// スカイボックス
	RenderLayer_Default = 0,	// 通常オブジェクト
	RenderLayer_UI = 50,		// UIオブジェクト
	RenderLayer_Fade = 100,	// フェード用オブジェクト
};

/// <summary>
/// RenderComponentクラス
/// </summary>
class RenderComponent : public Component
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderComponent();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~RenderComponent();

	/// <summary>
	/// ビューカメラを設定します。
	/// </summary>
	/// <param name="[In_Camera]">設定するカメラオブジェクトへのポインタ。</param>
	void SetCamera(_In_ GameObject *In_Camera) noexcept;

	/// <summary>
	/// レイヤー番号を設定します。
	/// </summary>
	/// <param name="[In_Layer]">設定するレイヤー番号。</param>
	void SetLayer(_In_ const int &In_Layer) noexcept { m_nLayer = In_Layer; }

	/// <summary>
	/// アセットのパスを設定します。
	/// </summary>
	/// <param name="[In_Path]">設定するファイルパス。</param>
	inline void SetAssetPath(_In_ const FilePath &In_Path) noexcept { m_AssetPath = In_Path; }

	virtual void ExecuteUpdate() noexcept override;

	virtual void ExecuteDraw() noexcept override;

	/// <summary>
	/// データアクセサーを使用して読み書きを行います。
	/// </summary>
	/// <param name="[In_Data]">読み書き操作に使用する DataAccessor 型のポインタ。</param>
	void ReadWrite(_In_ DataAccessor *In_Data) override;

	/// <summary>
	/// 描画処理の実装をする純粋仮想関数
	/// </summary>
	virtual void Draw() noexcept = 0;

protected:
	FilePath m_AssetPath;					// アセットパス
	int m_nLayer;							// レイヤー番号
	GameObject *m_pCameraObj;				// カメラオブジェクトへのポインタ
	Camera *m_pViewCamera;					// ビューカメラ

	std::shared_ptr<Shader> m_defVS;	// デフォルトの頂点シェーダー
	std::shared_ptr<PixelShader> m_defPS;	// デフォルトのピクセルシェーダー

private:
	// RenderManagerのインスタンス
	RenderManager &m_RenderManager;
};
