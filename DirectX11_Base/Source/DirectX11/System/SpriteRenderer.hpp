/*+===================================================================
	File: SpriteRenderer.hpp
	Summary: SpriteRendererクラス ModelRendererの実装を参考にして作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/14 Mon AM 05:48:13 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Component/Component.hpp"
#include "Sprite.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// SpriteRendererクラス
/// </summary>
class SpriteRenderer : public Component
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	void Execute() final;

	/// <summary>
	/// スプライトへのポインタを取得します。
	/// </summary>
	/// <returns>現在のオブジェクトに関連付けられている Sprite型のポインタ。</returns>
	inline Sprite *GetSprite() const noexcept { return m_pSprite; }

	/// <summary>
	/// データアクセサーを使用して読み書きを行います。
	/// </summary>
	/// <param name="In_Data">読み書き操作に使用する DataAccessor 型のポインタ。</param>
	void ReadWrite(_In_ DataAccessor *In_Data) override final;

	/// <summary>
	/// スプライトのパスを設定します。
	/// </summary>
	/// <param name="[In_File]">設定する画像ファイルのパス。</param>
	void SetSpritePath(_In_ FilePath In_File) noexcept;

	//#if _DEBUG
	//    void Debug(debug::Window *window) final;
	//#endif

private:
	char m_cSpriteName[MAX_PATH];
	Sprite *m_pSprite;
	float m_fScale;
};
