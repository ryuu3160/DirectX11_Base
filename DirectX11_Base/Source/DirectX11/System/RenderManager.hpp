/*+===================================================================
	File: RenderManager.hpp
	Summary: 全ての描画を管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/20 Sun PM 09:43:49 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "RenderComponent.hpp"

/// <summary>
/// RenderManagerクラス
/// </summary>
class RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;
public:

	/// <summary>
	/// レンダーコンポーネントを指定したレイヤーに追加します。
	/// </summary>
	/// <param name="[In_RenderComponent]">追加するRenderComponentへのポインタ。</param>
	/// <param name="[In_Layer]">レンダーコンポーネントを追加するレイヤー番号。省略時は0。</param>
	void AddRenderComponent(_In_ RenderComponent* In_RenderComponent, _In_ const int& In_Layer = 0) noexcept;

	/// <summary>
	/// すべての描画処理を実行します。
	/// </summary>
	void DrawAll() noexcept;

private:
	RenderManager();
	~RenderManager();

private:

	std::map<int, std::vector<RenderComponent *>> m_RenderComponents; // レンダリングコンポーネントのマップ
};
