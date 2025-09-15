/*+===================================================================
	File: SceneManager.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/11 Thu AM 05:31:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SceneManager.hpp"
#include "DirectX11/System/RenderManager.hpp"
#include "System/CollisionManager/CollisionManager.hpp"

SceneManager::SceneManager()
	: m_RenderManager(RenderManager::GetInstance())
	, m_FadeManager(FadeManager::GetInstance())
	, m_CollisionManager(CollisionManager::GetInstance())
{
	m_pCurrentScene = nullptr;
	m_pNextScene = nullptr;
	m_SubScene.clear();
	m_NextSubScene.clear();
	m_Futures.clear();
}

SceneManager::~SceneManager()
{
	UnLoadCurrentScene();
	m_pNextScene.reset();
	m_pNextScene = nullptr;
	for (auto &itr : m_NextSubScene)
	{
		itr.second.reset();
		itr.second = nullptr;
	}
	m_NextSubScene.clear();
	for (auto &itr : m_Futures)
	{
		if (itr.valid())
			itr.get();
	}
	m_Futures.clear();
}

void SceneManager::RootUpdate() noexcept
{
	// メインの更新
	_RootUpdateMain();

	m_CollisionManager.CheckAllCollisions();

	// 遅延更新
	_RootUpdateLate();

	// フェードの処理
	m_FadeManager.Update();
}

void SceneManager::RootDraw() noexcept
{
	if (m_pCurrentScene)
		m_pCurrentScene->_RootDraw();
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_RootDraw();
	}

	// フェードの描画
	m_FadeManager.Draw();

	// 全ての描画
	m_RenderManager.DrawAll();
}

void SceneManager::UpdateSceneChange() noexcept
{
	// シーンの読み込みが無い場合は何もしない
	if(!m_Futures.empty() && !m_pNextScene && m_NextSubScene.empty() && m_RemoveSubScene.empty())
		return;

	// 非同期ロード
	for (auto &itr : m_Futures)
	{
		if (itr.valid())
		{
			// 非同期ロードが完了している場合はロード
			itr.get();
			itr = std::future<void>();// futureを無効化
		}
	}
	m_Futures.clear();

	// シーンの切り替え
	if (m_pNextScene && m_pCurrentScene)
	{
		UnLoadCurrentScene();
		m_pCurrentScene.swap(m_pNextScene);
		m_pNextScene.reset();
		m_pNextScene = nullptr;
	}

	// サブシーンの削除
	_RemoveSubScene();

	// サブシーンの切り替え&追加
	_ChangeAndAddSubScene();
}

void SceneManager::RemoveAllSubScene() noexcept
{
	for(auto &itr : m_SubScene)
	{
		if (itr.second)
		{
			itr.second->Uninit();
			itr.second.reset();
			itr.second = nullptr;
		}
	}
	m_SubScene.clear();
}

void SceneManager::UnLoadCurrentScene() noexcept
{
	// 先にサブシーンを解放
	RemoveAllSubScene();

	// メインシーンを解放
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Uninit();
		m_pCurrentScene.reset();
		m_pCurrentScene = nullptr;
	}
}

void SceneManager::_RootUpdateMain() noexcept
{
	// メインシーンの更新
	if (m_pCurrentScene)
		m_pCurrentScene->_RootUpdateMain();
	// サブシーンの更新
	for (auto &itr : m_SubScene)
	{
		if(itr.second)
			itr.second->_RootUpdateMain();
	}
}

void SceneManager::_RootUpdateLate() noexcept
{
	// メインシーンの更新
	if (m_pCurrentScene)
		m_pCurrentScene->_RootUpdateLate();
	// サブシーンの更新
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_RootUpdateLate();
	}
}

void SceneManager::_RemoveSubScene() noexcept
{
	// 削除予約リストを元にサブシーンを削除
	for (auto &itr : m_RemoveSubScene)
	{
		// サブシーンを全て検索
		for (auto SubItr = m_SubScene.begin(); SubItr != m_SubScene.end();)
		{
			// 削除予約リストにある型情報と一致した場合は削除
			if (SubItr->first == itr)
			{
				if (SubItr->second)
				{
					SubItr->second->Uninit();
					SubItr->second.reset();
					SubItr->second = nullptr;
					SubItr = m_SubScene.erase(SubItr);
				}
				else
				{
					++SubItr;
				}
			}
		}
	}
	m_RemoveSubScene.clear(); // 削除予約リストをクリア
}

void SceneManager::_ChangeAndAddSubScene() noexcept
{
	for (auto &itr : m_NextSubScene)
	{
		for (auto &cur : m_SubScene)
		{
			// 同じ型のサブシーンがある場合は置き換え
			if (cur.first == itr.first)
			{
				cur.second->Uninit();
				cur.second.reset();
				cur.second = itr.second;
				continue;
			}
		}
		// 見つからなかった場合は追加
		m_SubScene.push_back({ itr.first,itr.second });
		itr.second.reset();
		itr.second = nullptr;
	}
	m_NextSubScene.clear();
}
