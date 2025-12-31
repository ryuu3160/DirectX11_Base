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
#include "Engine/Core/System/Scene/SceneBase.hpp"

// ==============================
//	前方宣言
// ==============================
class RenderManager;
class CollisionManager;

/// <summary>
/// SceneManagerクラス
/// </summary>
class SceneManager : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;
public:

	template <typename T, typename ...Args, typename std::enable_if < std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	std::shared_ptr<T> Init(_In_ Args&&... In_Args) noexcept;

	void SceneObjectsInit() noexcept;

	void RootFixedUpdate(_In_ double In_FixedDeltaTime) noexcept;

	/// <summary>
	/// シーンのアップデート
	/// </summary>
	void RootUpdate(_In_ float In_DeltaTime) noexcept;

	/// <summary>
	/// シーンの描画
	/// </summary>
	void RootDraw() noexcept;

	/// <summary>
	/// 削除予約リストに追加されたオブジェクトを破棄します。
	/// </summary>
	void DestroyObjects() noexcept;

	/// <summary>
	/// <para>シーンの切り替えを行うアップデート</para>
	/// <para>すべてのアップデートが終わった後に呼び出してください。</para>
	/// </summary>
	void UpdateSceneChange() noexcept;

	template <typename T, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void RemoveSubScene() noexcept;

	/// <summary>
	/// すべてのサブシーンを削除します。
	/// </summary>
	void RemoveAllSubScene() noexcept;

	/// <summary>
	/// 現在のシーンを取得します。
	/// </summary>
	/// <returns>シーンへのシェアポインタ</returns>
	std::shared_ptr<SceneBase> GetCurrentScene() const noexcept { return m_pCurrentScene; }

	template <typename T>
	requires std::derived_from<T, SceneBase>
	std::shared_ptr<T> GetSubScene() noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadScene(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSceneAsync(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSubScene(_In_ Args&&... In_Args) noexcept;

	template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type * = nullptr>
	void LoadSubSceneAsync(_In_ Args&&... In_Args) noexcept;

private:
	SceneManager();
	~SceneManager() override;

	/// <summary>
	/// 現在のシーンをアンロードします。
	/// </summary>
	void UnLoadCurrentScene() noexcept;

	void _RootUpdateMain(_In_ float In_DeltaTime) noexcept;
	void _RootUpdateLate(_In_ float In_DeltaTime) noexcept;

	void _ExecuteDestroyObjectsComponents() noexcept;

	/// <summary>
	/// 削除予約リストに追加されたサブシーンを削除します。
	/// </summary>
	void _RemoveSubScene() noexcept;

	/// <summary>
	/// サブシーンを変更し、追加します。
	/// </summary>
	void _ChangeAndAddSubScene() noexcept;

private:

	bool m_IsInitialized; // 最初のシーンロードがされているか

	std::shared_ptr<SceneBase> m_pCurrentScene; // 現在のシーン
	std::shared_ptr<SceneBase> m_pNextScene;    // 次にロードするシーン

	std::vector<std::pair<std::type_index, std::shared_ptr<SceneBase>>> m_SubScene;		// 現在のサブシーン
	std::vector<std::pair<std::type_index, std::shared_ptr<SceneBase>>> m_NextSubScene; // 次にロードするサブシーン
	std::vector<std::type_index> m_RemoveSubScene; // 削除するサブシーンの型情報

	std::vector<std::future<void>> m_Futures; // 非同期ロード用のfuture
	std::list<std::future<void>> m_DrawFutures; // 描画用の非同期ロード

	RenderManager &m_RenderManager; // レンダリングマネージャーのインスタンス
	FadeManager &m_FadeManager;
	CollisionManager &m_CollisionManager;
};

/// <summary>
/// 一番最初のシーンをロードする関数
/// </summary>
/// <param name="[...In_Args]">コンストラクタに引数がある場合は渡す</param>
/// <returns>作成したシーンへのシェアポインタ</returns>
template<typename T, typename ...Args, typename std::enable_if < std::is_base_of<SceneBase, T>::value>::type *>
inline std::shared_ptr<T> SceneManager::Init(_In_ Args&&... In_Args) noexcept
{
	if (m_IsInitialized)
		return std::static_pointer_cast<T>(m_pCurrentScene);

	std::shared_ptr<T> newScene = std::make_shared<T>(In_Args...);
	m_pCurrentScene = newScene;
	m_pCurrentScene->CommonProcessScene();
	m_pCurrentScene->Init();
	m_IsInitialized = true;
	return std::static_pointer_cast<T>(m_pCurrentScene);
}

/// <summary>
/// SceneBaseを継承した型Tのサブシーンを削除予約リストに追加します。
/// </summary>
/// <typeparam name="T">SceneBaseを継承したサブシーンの型。</typeparam>
template<typename T, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::RemoveSubScene() noexcept
{
	// 削除予約リストに追加
	m_RemoveSubScene.push_back(typeid(T));
}

template<typename T>
requires std::derived_from<T, SceneBase>
inline std::shared_ptr<T> SceneManager::GetSubScene() noexcept
{
	for (const auto &itr : m_SubScene)
	{
		if(itr.first == typeid(T))
		{
			return std::static_pointer_cast<T>(itr.second);
		}
	}
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
	m_Futures.push_back(std::move(future));
}

template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSubScene(Args && ...In_Args) noexcept
{
	// サブシーンを作成
	std::shared_ptr<T> newSubScene = std::make_shared<T>(In_Args...);
	newSubScene->Init();

	// 既に同じ型のサブシーンがロードされている場合は追加しない
	for (auto &itr : m_NextSubScene)
	{
		if (itr.first == typeid(T))
		{
			return;
		}
	}

	// 見つからなかった場合は追加
	m_NextSubScene.push_back({ typeid(T),newSubScene });
}

template <typename T, typename ...Args, typename std::enable_if<std::is_base_of<SceneBase, T>::value>::type *>
inline void SceneManager::LoadSubSceneAsync(Args && ...In_Args) noexcept
{
	// サブシーン作成の非同期処理
	std::future<void> future = std::async(std::launch::async, [this, In_Args...]() {
		// サブシーンを作成
		std::shared_ptr<T> newSubScene = std::make_shared<T>(In_Args...);
		newSubScene->Init();

		// 既に同じ型のサブシーンがロードされている場合は追加しない
		for(auto &itr : m_NextSubScene)
		{
			if (itr.first == typeid(T))
			{
				return;
			}
		}

		// 見つからなかった場合は追加
		m_NextSubScene.push_back({ typeid(T),newSubScene });
		});
	m_Futures.push_back(std::move(future));
}
