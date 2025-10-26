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
	void AddRenderComponent(_In_ RenderComponent* In_RenderComponent, _In_ LayerGroup In_Layer = LayerGroup_Default) noexcept;

	/// <summary>
	/// 指定されたレンダーコンポーネント削除します。
	/// </summary>
	/// <param name="[In_RenderComponent]">削除対象の RenderComponent へのポインタ。</param>
	/// <param name="[In_LayerGroup]">削除を行う LayerGroup。省略時は RenderLayer_Default が使用されます。</param>
	void RemoveRenderComponent(_In_ RenderComponent *In_RenderComponent, _In_ LayerGroup In_LayerGroup) noexcept;

	void RemoveAllRenderComponent() noexcept;

	/// <summary>
	/// 指定されたレイヤーグループのソート要求を行います。
	/// </summary>
	/// <param name="[In_OldLayerGroup]">ソート対象となるレイヤーグループ。</param>
	void LayerGroupSortRequest(_In_ LayerGroup In_OldLayerGroup) noexcept;

	/// <summary>
	/// 指定されたレイヤー番号に基づいてレイヤーの並び替えを要求します。
	/// </summary>
	/// <param name="In_OldLayer">並び替え対象のレイヤーグループ</param>
	void LayerSortRequest(_In_ LayerGroup In_LayerGroup) noexcept;

	/// <summary>
	/// すべての描画処理を実行します。
	/// </summary>
	void DrawAll() noexcept;

private:
	RenderManager();
	~RenderManager();

	void ExecuteLayerGroupSort() noexcept;
	void ExecuteLayerSort() noexcept;


	void RemoveRenderComponentRequest(_In_ LayerGroup In_LayerGroup) noexcept;
	void ExecuteRemoveComponentAsync() noexcept;
	void WaitRemoveComponentAsync() noexcept;

private:

	bool m_IsSortLayerGroup; // レイヤーグループのソートが必要かどうかのフラグ
	bool m_IsSortLayer; // ソートが必要かどうかのフラグ
	bool m_IsRemoveComponent; // レンダーコンポーネントの削除が必要かどうかのフラグ
	std::future<void> m_RemoveFuture; // 非同期削除用のfutureリスト
	std::vector<LayerGroup> m_StandbyRemoveComponent; // 削除待ちのレイヤーグループリスト
	std::vector<LayerGroup> m_StandbySortLayerGroup; // ソート待ちのレイヤーグループリスト
	std::vector<LayerGroup> m_StandbySortLayer; // ソート待ちのレイヤーリスト
	std::map<LayerGroup, std::vector<RenderComponent *>> m_RenderComponents; // レンダリングコンポーネントのマップ
};
