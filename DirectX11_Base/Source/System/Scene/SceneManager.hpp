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

	template <typename T, typename ...Args, typename std::enable_if < std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	std::shared_ptr<T> Init(_In_ Args&&... In_Args) noexcept;

	/// <summary>
	/// <para>シーンの切り替えを行うアップデート</para>
	/// <para>すべてのアップデートが終わった後に呼び出してください。</para>
	/// </summary>
	void Update() noexcept;

	/// <summary>
	/// 現在のシーンを取得します。
	/// </summary>
	/// <returns>シーンへのシェアポインタ</returns>
	std::shared_ptr<SceneBase> GetCurrentScene() const noexcept { return m_pCurrentScene; }

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadScene(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSceneAsync(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSubScene(_In_ std::string In_SceneName,_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSubSceneAsync(_In_ std::string In_SceneName, _In_ Args&&... In_Args) noexcept;

private:

	/// <summary>
	/// 現在のシーンをアンロードします。
	/// </summary>
	void UnLoadCurrentScene() noexcept;

private:

	std::shared_ptr<SceneBase> m_pCurrentScene; // 現在のシーン
	std::shared_ptr<SceneBase> m_pNextScene;    // 次にロードするシーン

	std::unordered_map<std::string,std::shared_ptr<SceneBase>> m_NextSubScene; // 次にロードするサブシーン

	std::vector<std::future<void>> m_Futures; // 非同期ロード用のfuture
};

/// <summary>
/// 一番最初のシーンをロードする関数
/// </summary>
/// <param name="[...In_Args]">コンストラクタに引数がある場合は渡す</param>
/// <returns>作成したシーンへのシェアポインタ</returns>
template<typename T, typename ...Args, typename std::enable_if < std::is_base_of<SceneBase, T>::value>::type *>
inline std::shared_ptr<T> SceneManager::Init(_In_ Args&&... In_Args) noexcept
{
	if (m_pCurrentScene)
		UnLoadCurrentScene();

	std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
	m_pCurrentScene = newScene;
	m_pCurrentScene->Init();
	return std::static_pointer_cast<T>(m_pCurrentScene);
}

/// <summary>
/// メインのシーンをロードする関数
/// </summary>
/// <param name="[...In_Args]">コンストラクタに引数がある場合は渡す</param>
template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadScene(Args && ...In_Args) noexcept
{
	std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
	// すでに次のシーンがある場合は解放
	if (m_pNextScene)
	{
		m_pNextScene.reset();
		m_pNextScene = nullptr;
	}
	m_pNextScene = newScene;
	m_pNextScene->Init();
	for(auto &sub : m_NextSubScene)
	{
		sub->Uninit();
		sub.reset();
		sub = nullptr;
	}
	m_NextSubScene.clear();
}

/// <summary>
/// メインのシーンを非同期でロードする関数
/// </summary>
/// <param name="[...In_Args]">コンストラクタに引数がある場合は渡す</param>
template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSceneAsync(Args && ...In_Args) noexcept
{
	std::future<void> future = std::async(std::launch::async, [this, In_Args...]() {
		std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
		if (m_pNextScene)
		{
			m_pNextScene.reset();
			m_pNextScene = nullptr;
		}
		m_pNextScene = newScene;
		m_pNextScene->Init();
		for (auto &sub : m_NextSubScene)
		{
			sub->Uninit();
			sub.reset();
			sub = nullptr;
		}
		m_NextSubScene.clear();
		});
	m_Futures.push_back(future);
}

template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSubScene(_In_ std::string In_SceneName, Args && ...In_Args) noexcept
{
	std::shared_ptr<T> newSubScene = std::make_shared<T>(In_Args...);
	newSubScene->Init();
	auto itr = m_NextSubScene.find(In_SceneName);
	if (itr != m_NextSubScene.end())
	{
		itr->second->Uninit();
		itr->second.reset();
		itr->second = newSubScene;
	}
	else
	{
		m_NextSubScene.insert({ In_SceneName,newSubScene });
	}
}

template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSubSceneAsync(_In_ std::string In_SceneName, Args && ...In_Args) noexcept
{
	std::future<void> future = std::async(std::launch::async, [this, In_SceneName, In_Args...]() {
		std::shared_ptr<SceneBase> newSubScene = std::make_shared<T>(In_Args...);
		newSubScene->Init();
		auto itr = m_NextSubScene.find(In_SceneName);
		if (itr != m_NextSubScene.end())
		{
			itr->second->Uninit();
			itr->second.reset();
			itr->second = newSubScene;
		}
		else
		{
			m_NextSubScene.insert({ In_SceneName,newSubScene });
		}
		});
	m_Futures.push_back(future);
}
