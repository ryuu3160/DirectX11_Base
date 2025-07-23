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

void RenderManager::AddRenderComponent(_In_ RenderComponent *In_RenderComponent, _In_ const int &In_Layer) noexcept
{
	if(NullCheck(In_RenderComponent,NCMode::OUTPUT,"RenderComponent is null in RenderManager::AddRenderComponent."))
		return;

	// 受け取ったレイヤーをキーにして要素の構築を試す
	auto &&itr = m_RenderComponents.try_emplace(In_Layer);
	// 構築された、又は既に存在するレイヤーへRenderComponentを追加
	itr.first->second.push_back(In_RenderComponent);
}

void RenderManager::DrawAll() noexcept
{
	// レイヤー順に登録されたものを全て描画
	// mapは常にキーの昇順で管理されている
	for (auto &layer : m_RenderComponents)
	{
		for (auto &itr : layer.second)
		{
			// RenderComopnentがnullptrじゃない場合のみDrawを呼び出す
			if (itr)
				itr->Draw();
		}
		layer.second.clear(); // レイヤーに登録されているRenderComponentをクリア
	}
	// レイヤー自体は残しておいた方が処理効率が良いため、マップ自体はクリアしない
}

RenderManager::RenderManager()
{
	// レンダリングコンポーネントのマップを初期化
	m_RenderComponents.clear();
}
RenderManager::~RenderManager()
{
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

