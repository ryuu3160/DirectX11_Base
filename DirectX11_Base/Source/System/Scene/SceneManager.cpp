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
	if (m_Future.valid())
	{
		// 非同期ロードが完了している場合はロード
		m_Future.get();
		m_Future = std::future<void>();// futureを無効化
	}

	if (m_pNextScene && m_pCurrentScene)
	{
		UnLoadCurrentScene();
		m_pCurrentScene.swap(m_pNextScene);
		m_pNextScene.reset();
		m_pNextScene = nullptr;
	}
}

void SceneManager::UnLoadCurrentScene() noexcept
{
}
