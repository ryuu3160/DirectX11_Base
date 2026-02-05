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
#include "Core/System/Component/Component.hpp"
#include "Core/DirectX11/Resource/ShaderResource/Shader.hpp"
#include "Core/DirectX11/System/RenderContext.hpp"

// ==============================
//  前方宣言
// ==============================
class Camera;
class RenderManager;

// ==============================
//  列挙型
// ==============================
enum LayerGroup : int
{
	LayerGroup_SkyBox = -2,		// スカイボックス
	LayerGroup_Grid,			// グリッド
	LayerGroup_Default,			// 通常オブジェクト
	LayerGroup_UI,				// UIオブジェクト
	LayerGroup_Line,			// 線描画オブジェクト
	LayerGroup_RenderTexture,	// レンダーテクスチャ用オブジェクト
	LayerGroup_Fade,			// フェード用オブジェクト

};

/// <summary>
/// RenderComponentクラス
/// </summary>
class RenderComponent : public Component
{
public:
	RenderComponent(_In_ std::string In_Name);
	virtual ~RenderComponent();

	/// <summary>
	/// レイヤーグループを取得します。
	/// </summary>
	/// <returns>レイヤーグループの値を返します。</returns>
	LayerGroup GetLayerGroup() const noexcept { return m_LayerGroup; }

	/// <summary>
	/// レイヤー番号を取得します。
	/// </summary>
	/// <returns>レイヤーの番号（int型）を返します。</returns>
	int GetLayer() const noexcept { return m_nLayer; }

	/// <summary>
	/// アセットのファイルパスを取得します。
	/// </summary>
	/// <returns>アセットのファイルパスを表す FilePath 型の値。</returns>
	FilePath GetAssetPath() const noexcept { return m_AssetPath; }

	void SetLayerGroup(_In_ LayerGroup In_LayerGroup) noexcept;

	/// <summary>
	/// レイヤー番号を設定します。
	/// </summary>
	/// <param name="[In_Layer]">設定するレイヤー番号。</param>
	void SetLayer(_In_ int In_Layer) noexcept;

	/// <summary>
	/// アセットのパスを設定します。
	/// </summary>
	/// <param name="[In_Path]">設定するファイルパス。</param>
	inline void SetAssetPath(_In_ const FilePath &In_Path) noexcept { m_AssetPath = std::string(In_Path); }

	virtual void Init() noexcept override;

	virtual void Update(_In_ float In_DeltaTime) noexcept override;

	/// <summary>
	/// データアクセサーを使用して読み書きを行います。
	/// </summary>
	/// <param name="[In_Data]">読み書き操作に使用する DataAccessor 型のポインタ。</param>
	void SaveLoad(_In_ DataAccessor *In_Data) override final;

	/// <summary>
	/// 描画処理の実装をする純粋仮想関数
	/// </summary>
	virtual void Draw(_In_ RenderContext *In_RenderContext) noexcept = 0;

protected:
#ifdef _DEBUG
	virtual void Inspector(_In_ ItemGroup *In_pGroup) override;
#endif // _DEBUG

private:
	
	void CheckLayerGroupChange();
	void CallLayerSortRequest();

protected:
	FilePathHold m_AssetPath;				// アセットパス
	LayerGroup m_LayerGroup;				// レイヤーグループ
	LayerGroup m_PrevLayerGroup;			// 前回のレイヤーグループ
	int m_nLayer;							// レイヤー番号

	std::shared_ptr<Shader> m_defVS;	// デフォルトの頂点シェーダー
	std::shared_ptr<PixelShader> m_defPS;	// デフォルトのピクセルシェーダー

private:
	// RenderManagerのインスタンス
	RenderManager &m_RenderManager;
};
