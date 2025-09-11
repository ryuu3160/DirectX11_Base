/*+===================================================================
	File: SceneManager.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/11 Thu AM 05:31:11 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SceneBase.hpp"

/// <summary>
/// SceneManagerクラス
/// </summary>
class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	void Update() noexcept;

	std::shared_ptr<SceneBase> GetCurrentScene() const noexcept { return m_pCurrentScene; }

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadScene(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSceneAsync(_In_ Args&&... In_Args) noexcept;

private:

	/// <summary>
	/// 現在のシーンをアンロードします。
	/// </summary>
	void UnLoadCurrentScene() noexcept;

private:

	std::shared_ptr<SceneBase> m_pCurrentScene; // 現在のシーン
	std::shared_ptr<SceneBase> m_pNextScene;    // 次にロードするシーン

	std::future<void> m_Future; // 非同期ロード用のfuture
};

template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadScene(Args && ...In_Args) noexcept
{
	std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
	m_pNextScene.reset(newScene);
	m_pNextScene->Init();
}

template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSceneAsync(Args && ...In_Args) noexcept
{
	m_Future = std::async(std::launch::async, [this, In_Args...]() {
		std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
		m_pNextScene.reset(newScene);
		m_pNextScene->Init();
		});
}
