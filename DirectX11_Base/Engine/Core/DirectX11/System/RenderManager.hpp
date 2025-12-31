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
#include "Engine/Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Engine/Core/DirectX11/System/RenderContext.hpp"

// ==============================
//  前方宣言
// ==============================
class Gizmos;

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

	inline void SetDrawGizmos(_In_ bool In_IsDrawGizmos) noexcept { m_IsDrawGizmos = In_IsDrawGizmos; }
	inline bool IsDrawGizmos() const noexcept { return m_IsDrawGizmos; }

	/// <summary>
	/// 指定したカメラとレンダーターゲット／デプスステンシルを使用してメインの RenderContext を作成します。関数は例外を投げません。
	/// </summary>
	/// <param name="[In_Camera]">描画に使用する入力カメラ。</param>
	/// <param name="[In_RTV]">描画結果の出力先となるレンダーターゲットビュー（RenderTarget）。</param>
	/// <param name="[In_DSV]">深度およびステンシル情報を保持するデプスステンシルビュー（DepthStencil）。</param>
	/// <returns>作成された RenderContext へのポインタを返します。作成に失敗した場合は nullptr を返す可能性があります。</returns>
	RenderContext *CreateMainRenderContext(_In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept;

	/// <summary>
	/// 指定したカメラ、レンダーターゲット、およびデプスステンシルを使用してレンダーコンテキストを作成または初期化します。
	/// </summary>
	/// <param name="[In_Name]">レンダーコンテキストの名前を表す文字列。</param>
	/// <param name="[In_Camera]">レンダリングに使用するカメラへのポインタ (Camera*)。ビューや投影情報を提供します。</param>
	/// <param name="[In_RTV]">カラー描画先となるレンダーターゲットビューへのポインタ (RenderTarget*)。</param>
	/// <param name="[In_DSV]">深度およびステンシル情報を保持するデプスステンシルへのポインタ (DepthStencil*)。</param>
	/// <returns>作成または既に作成されているRenderContextへのポインタ。</returns>
	RenderContext* CreateRenderContext(_In_ const std::string &In_Name, _In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept;

	/// <summary>
	/// 指定した名前のレンダーコンテキストを取得する
	/// </summary>
	/// <param name="[In_Name]">検索名</param>
	/// <returns>ヒットした場合、RenderContextへのポインタが返る</returns>
	RenderContext *GetRenderContext(_In_ const std::string &In_Name);

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
	bool m_IsDrawGizmos; // ギズモを描画するかどうかのフラグ
	std::future<void> m_RemoveFuture; // 非同期削除用のfutureリスト
	std::vector<LayerGroup> m_StandbyRemoveComponent; // 削除待ちのレイヤーグループリスト
	std::vector<LayerGroup> m_StandbySortLayerGroup; // ソート待ちのレイヤーグループリスト
	std::vector<LayerGroup> m_StandbySortLayer; // ソート待ちのレイヤーリスト
	std::map<LayerGroup, std::vector<RenderComponent *>> m_RenderComponents; // レンダリングコンポーネントのマップ
	std::unordered_set<GameObject *> m_ObjectsToDrawGizmos; // ギズモを描画するオブジェクトのリスト

	MultiComparison<std::string,LayerGroup> m_RenderContextComparison; // レンダーコンテキストの比較用マルチコンパリソン
	std::unordered_map<std::string, RenderContext *> m_RenderContexts; // レンダーコンテキストのマップ
	std::vector<std::string> m_RenderContextNames; // レンダーコンテキストのリスト

#ifdef _DEBUG
	Gizmos *m_pGizmos; // ギズモ描画用オブジェクト
#endif
};
