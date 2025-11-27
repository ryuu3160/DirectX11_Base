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
	, m_IsInitialized(false)
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

void SceneManager::SceneObjectsInit() noexcept
{
	if (m_pCurrentScene)
		m_pCurrentScene->_ObjectsInit();
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_ObjectsInit();
	}
}

void SceneManager::RootFixedUpdate(_In_ double In_FixedTick) noexcept
{
	// メインシーンの更新
	if (m_pCurrentScene)
		m_pCurrentScene->_RootFixedUpdate(In_FixedTick);
	// サブシーンの更新
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_RootFixedUpdate(In_FixedTick);
	}

	// 当たり判定の更新
	m_CollisionManager.CheckAllCollisions();
}

void SceneManager::RootUpdate(_In_ float In_Tick) noexcept
{
	// 初期化されていないオブジェクトの初期化
	SceneObjectsInit();

	// メインの更新
	_RootUpdateMain(In_Tick);

	// 遅延更新
	_RootUpdateLate(In_Tick);

	// コンポーネント削除予約リストの処理
	_ExecuteDestroyObjectsComponents();

	// フェードの処理
	m_FadeManager.Update(In_Tick);
}

void SceneManager::RootDraw() noexcept
{
	if (m_pCurrentScene)
		m_DrawFutures.push_back(std::async(std::launch::async, &SceneBase::_RootDraw, m_pCurrentScene));
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			m_DrawFutures.push_back(std::async(std::launch::async, &SceneBase::_RootDraw, itr.second));
	}

	// 非同期描画の完了を待つ
	for(auto &itr : m_DrawFutures)
	{
		if (itr.valid())
			itr.get();
	}

	// 全ての描画
	m_RenderManager.DrawAll();
}

void SceneManager::DestroyObjects() noexcept
{
	if (m_pCurrentScene)
		m_pCurrentScene->_DestroyObjects();

	for (auto itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_DestroyObjects();
	}
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

void SceneManager::_RootUpdateMain(_In_ float In_Tick) noexcept
{
	// メインシーンの更新
	if (m_pCurrentScene)
		m_pCurrentScene->_RootUpdateMain(In_Tick);
	// サブシーンの更新
	for (auto &itr : m_SubScene)
	{
		if(itr.second)
			itr.second->_RootUpdateMain(In_Tick);
	}
}

void SceneManager::_RootUpdateLate(_In_ float In_Tick) noexcept
{
	// メインシーンの更新
	if (m_pCurrentScene)
		m_pCurrentScene->_RootUpdateLate(In_Tick);
	// サブシーンの更新
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_RootUpdateLate(In_Tick);
	}
}

void SceneManager::_ExecuteDestroyObjectsComponents() noexcept
{
	if (m_pCurrentScene)
		m_pCurrentScene->_ExecuteDestroyObjectsComponents();
	for (auto &itr : m_SubScene)
	{
		if (itr.second)
			itr.second->_ExecuteDestroyObjectsComponents();
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
