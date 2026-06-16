/*+===================================================================
	File: Application.hpp
	Summary:
	Author: 青木雄一郎
	Date: 2026/06/16 Tue PM 12:34:31 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"

/**
 * @brief Applicationクラス
 */
class Application : public Singleton<Application>
{
	friend class Singleton<Application>;
public:
	inline void Quit() noexcept { m_bIsQuit = true; }
	inline bool IsQuit() const noexcept { return m_bIsQuit; }
private:
	Application() = default;
	~Application() = default;


private:
	bool m_bIsQuit = false; // エディタモードでアプリケーションを終了するためのフラグ
};
