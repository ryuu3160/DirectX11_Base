/*+===================================================================
	File: RenderManager.cpp
	Summary: 全ての描画を管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/20 Sun PM 09:43:56 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "RenderManager.hpp"
#include "System/Component/Camera.hpp"

void RenderManager::AddRenderComponent(_In_ RenderComponent *In_RenderComponent, _In_ LayerGroup In_Layer) noexcept
{
	if(NullCheck(In_RenderComponent,NCMode::OUTPUT,"RenderComponent is null in RenderManager::AddRenderComponent."))
		return;

	// 受け取ったレイヤーをキーにして要素の構築を試す
	auto &&itr = m_RenderComponents.try_emplace(In_Layer);
	// 構築された、又は既に存在するレイヤーへRenderComponentを追加
	itr.first->second.push_back(In_RenderComponent);
}

void RenderManager::RemoveRenderComponent(_In_ RenderComponent *In_RenderComponent, _In_ LayerGroup In_LayerGroup) noexcept
{
	if (NullCheck(In_RenderComponent, NCMode::OUTPUT, "RenderComponent is null in RenderManager::RemoveRenderComponent."))
		return;
	auto &&itr = m_RenderComponents.find(In_LayerGroup);
	if (itr != m_RenderComponents.end())
	{
		auto &layerList = itr->second;
		auto compItr = std::find(layerList.begin(), layerList.end(), In_RenderComponent);
		if (compItr != layerList.end())
		{
			layerList.erase(compItr);
		}
	}
}

void RenderManager::RemoveAllRenderComponent() noexcept
{
	for (auto &layer : m_RenderComponents)
	{
		layer.second.clear();
	}
}

RenderContext *RenderManager::CreateMainRenderContext(_In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept
{
	RenderContext *Context = new RenderContext();
	DirectX::XMFLOAT4X4 ViewMatrix;
	DirectX::XMFLOAT4X4 ProjMatrix;
	// カメラからビュー行列と射影行列を取得
	ViewMatrix = In_Camera->GetView(false);
	ProjMatrix = In_Camera->GetProj(false);
	Context->CreateMainContext(In_Camera, In_RTV, In_DSV);
	auto Result = m_RenderContexts.try_emplace("Main", Context);
	if (!Result.second)
	{
		// 既に同じ名前のコンテキストが存在する場合は古い方を削除して置き換え
		delete Result.first->second;
		Result.first->second = Context;
	}
	else
	{
		m_RenderContextNames.push_back("Main");
	}
	return Context;
}

RenderContext* RenderManager::CreateRenderContext(_In_ const std::string &In_Name, _In_ Camera *In_Camera, _In_ RenderTarget *In_RTV, _In_ DepthStencil *In_DSV) noexcept
{
	RenderContext *Context = new RenderContext();
	DirectX::XMFLOAT4X4 ViewMatrix;
	DirectX::XMFLOAT4X4 ProjMatrix;
	// カメラからビュー行列と射影行列を取得
	ViewMatrix = In_Camera->GetView(false);
	ProjMatrix = In_Camera->GetProj(false);
	Context->Create(In_Camera, In_RTV, In_DSV);
	auto Result = m_RenderContexts.try_emplace(In_Name, Context);
	if (!Result.second)
	{
		// 既に同じ名前のコンテキストが存在する場合は古い方を削除して置き換え
		delete Result.first->second;
		Result.first->second = Context;
	}
	else
	{
		m_RenderContextNames.push_back(In_Name);
	}
	return Context;
}

RenderContext *RenderManager::GetRenderContext(_In_ const std::string &In_Name)
{
	auto ctx = m_RenderContexts.find(In_Name);
	if (ctx != m_RenderContexts.end())
		return ctx->second;
	return nullptr;
}

void RenderManager::LayerGroupSortRequest(_In_ LayerGroup In_OldLayerGroup) noexcept
{
	m_IsSortLayerGroup = true;
	m_StandbySortLayerGroup.push_back(In_OldLayerGroup);
}

void RenderManager::LayerSortRequest(_In_ LayerGroup In_LayerGroup) noexcept
{
	m_IsSortLayer = true;
	m_StandbySortLayer.push_back(In_LayerGroup);
}

void RenderManager::DrawAll() noexcept
{
	auto &DX11Core = DX11_Core::GetInstance();

	// レンダーコンポーネントの削除要求があれば完了を待機
	if(m_IsRemoveComponent)
		WaitRemoveComponentAsync();

	// ソート要求があれば実行
	if(m_IsSortLayerGroup)
		ExecuteLayerGroupSort();
	if (m_IsSortLayer)
		ExecuteLayerSort();

	// 全てのレンダーコンテキストで描画処理を実行
	// メインのコンテキストでレンダーテクスチャを使用するため、逆順で処理を行う
	for (auto itr = m_RenderContextNames.rbegin(); itr != m_RenderContextNames.rend();++itr)
	{
		auto ctx = m_RenderContexts.find(*itr);
		// ViewとProjection行列を再計算
		ctx->second->ReCalculateMatrices();

		// レンダーターゲットとデプスステンシルをセット
		auto rtv = ctx->second->GetRTV();
		auto dsv = ctx->second->GetDSV();
		DX11Core.SetRenderTargets(1, &rtv, dsv);

		if(!ctx->second->IsMainContext())
		{
			// メインコンテキスト以外ではクリア処理を実行
			float color[4] = { 1.0f,1.0f,1.0f,1.0f };
			rtv->Clear(color);
			dsv->Clear();
		}

		// レイヤー順に登録されたものを全て描画
		// mapは常にキーの昇順で管理されている
		for (auto &layer : m_RenderComponents)
		{
			// メインコンテキスト以外ではレンダーテクスチャ用のレイヤーはスキップ
			// また、UIレイヤーもスキップ
			if (!ctx->second->IsMainContext() &&
				(m_RenderContextComparison.CheckComparison("MainOnlyDraw", layer.first )))
				continue;

			for (auto &itr : layer.second)
			{
				// RenderComopnentがnullptrじゃない場合のみDrawを呼び出す
				if (itr)
					itr->Draw(ctx->second);
			}
		}
	}

	// RTVとDSVをデフォルトに戻す
	DX11Core.SetRenderTargetsToDefault();

	// レンダーコンポーネントの削除要求があれば非同期で実行
	if (m_IsRemoveComponent)
		ExecuteRemoveComponentAsync();
}

RenderManager::RenderManager()
	: m_IsSortLayerGroup(false), m_IsSortLayer(false)
	, m_IsRemoveComponent(false)
{
	// レンダリングコンポーネントのマップを初期化
	m_RenderComponents.clear();

	m_RenderContextComparison.MakeComparison("MainOnlyDraw",
		{ LayerGroup::LayerGroup_RenderTexture,LayerGroup::LayerGroup_UI,LayerGroup::LayerGroup_Fade });
}
RenderManager::~RenderManager()
{
	for (auto &ctx : m_RenderContexts)
	{
		delete ctx.second;
		ctx.second = nullptr;
	}
	m_RenderContexts.clear();
	m_RenderContextNames.clear();

	for (auto &layer : m_RenderComponents)
	{
		for (auto *component : layer.second)
		{
			component = nullptr;
		}
		layer.second.clear(); // レイヤーのコンポーネントをクリア
	}
	m_RenderComponents.clear(); // マップ自体をクリア
}

void RenderManager::ExecuteLayerGroupSort() noexcept
{
	for (auto &itr : m_StandbySortLayerGroup)
	{
		for (auto layer = m_RenderComponents[itr].begin(); layer != m_RenderComponents[itr].end();)
		{
			LayerGroup NewGroup = (*layer)->GetLayerGroup();
			if (NewGroup != itr)
			{
				// 現在のレイヤーグループと異なる場合は新しいレイヤーグループへ移動
				m_RenderComponents[NewGroup].push_back(*layer);
				*layer = nullptr; // 移動元の要素をnullptrに設定
				RemoveRenderComponentRequest(itr); // 削除も要求
				LayerSortRequest(NewGroup); // レイヤーソートも要求
			}
			else
			{
				++layer;
			}

		}
	}

	m_StandbySortLayerGroup.clear();
	m_IsSortLayerGroup = false;
}

void RenderManager::ExecuteLayerSort() noexcept
{
	for (auto &itr : m_StandbySortLayer)
	{
		auto &vec = m_RenderComponents[itr];
		std::stable_sort(vec.begin(), vec.end(),
			[](RenderComponent *a, RenderComponent *b)
			{
				return a->GetLayer() < b->GetLayer();
			});
	}

	m_StandbySortLayer.clear();
	m_IsSortLayer = false;
}

void RenderManager::RemoveRenderComponentRequest(_In_ LayerGroup In_LayerGroup) noexcept
{
	m_IsRemoveComponent = true;
	m_StandbyRemoveComponent.push_back(In_LayerGroup);
}

void RenderManager::ExecuteRemoveComponentAsync() noexcept
{
	auto RemoveTask = [this]()
	{
		for (auto &itr : m_StandbyRemoveComponent)
		{
#if __cplusplus >= 202002L
			auto &LayerList = m_RenderComponents[itr];
			std::erase(LayerList, nullptr);
#else
			auto &LayerList = m_RenderComponents[itr];
			LayerList.erase(std::remove(LayerList.begin(), LayerList.end(), nullptr), LayerList.end());
#endif
		}
		m_StandbyRemoveComponent.clear();
	};
	m_RemoveFuture = std::async(std::launch::async, RemoveTask);
}

void RenderManager::WaitRemoveComponentAsync() noexcept
{
	if (m_RemoveFuture.valid())
	{
		m_RemoveFuture.get();
	}
	m_IsRemoveComponent = false;
}

