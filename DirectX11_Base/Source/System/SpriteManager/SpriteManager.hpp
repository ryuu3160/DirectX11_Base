/*+===================================================================
	File: SpriteManager.hpp
	Summary: スプライトマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/14 Mon AM 09:55:16 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Sprite.hpp"
#include "System/Component/Camera.hpp"

/// <summary>
/// SpriteManagerクラス
/// </summary>
class SpriteManager : public Singleton<SpriteManager>
{
	friend class Singleton<SpriteManager>;
	using Sprites = std::unordered_map<std::string, Sprite *>;
public:

	void Init() noexcept;

	void SetCameraViewAndCameraProj(_In_ Camera* In_Camera) noexcept;

	/// <summary>
	/// 操作中のディレクトリに保存されているスプライトを描画
	/// </summary>
	void Draw() noexcept;

	/// <summary>
	/// 操作中ディレクトリのスプライトリストを取得します。
	/// </summary>
	/// <returns>スプライト（Sprite 型ポインタ）のリストへの定数参照。</returns>
	std::list<Sprite *> GetSpriteList() const noexcept;

	/// <summary>
	/// 操作中のディレクトリにあるスプライトを取得します。
	/// </summary>
	/// <param name="[In_SpriteName]">スプライト名</param>
	/// <returns>スプライトへのポインタ</returns>
	Sprite *GetSprite(_In_ const std::string_view &In_SpriteName) const noexcept;

	/// <summary>
	/// スプライトを作成し、操作中のディレクトリに追加します。
	/// </summary>
	/// <param name="[In_SpriteName]">スプライト名</param>
	/// <param name="[In_FilePath]">スプライト</param>
	/// <returns>追加されたスプライトへのポインタ。</returns>
	Sprite* CreateSprite(_In_ const std::string_view &In_SpriteName, _In_ const FilePath &In_FilePath, _In_ const float &In_Scale = 1.0f) noexcept;

	/// <summary>
	/// 全てのスプライトオブジェクトを解放、削除する。
	/// </summary>
	void DeleteAll() noexcept;

private:
	SpriteManager();
	~SpriteManager();

private:
	Sprites m_Sprites; // スプライトのマップ
	std::list<Sprite *> m_SpriteList; // スプライトのリスト

	DirectX::XMFLOAT4X4 m_View;         // ビュー行列
	DirectX::XMFLOAT4X4 m_Projection3D;  // 投影行列
	DirectX::XMFLOAT4X4 m_Projection2D;
	DirectX::XMFLOAT4X4 m_BillBoardView;	
};
