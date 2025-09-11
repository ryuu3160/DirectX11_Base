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

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update() noexcept
{
	if (!m_Futures.empty())
	{
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
	}

	if (m_pNextScene && m_pCurrentScene)
	{
		UnLoadCurrentScene();
		m_pCurrentScene.swap(m_pNextScene);
		m_pNextScene.reset();
		m_pNextScene = nullptr;
	}

	if (!m_NextSubScene.empty())
	{
		for (auto &itr : m_NextSubScene)
		{
			if (m_pCurrentScene->m_Name == itr.first)
			{
				m_pCurrentScene->RemoveSubScene();
				m_pCurrentScene->m_pSubScene = itr.second;
				itr.second->m_pParent = m_pCurrentScene.get();
			}
		}
	}
}

void SceneManager::UnLoadCurrentScene() noexcept
{
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Uninit();
		m_pCurrentScene.reset();
		m_pCurrentScene = nullptr;
	}
}
