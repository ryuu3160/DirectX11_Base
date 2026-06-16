/*+===================================================================
	File: EditorApp.hpp
	Summary:
	Author: Â–Ø—Yˆê˜Y
	Date: 2026/06/16 Tue PM 12:19:14 ‰‰ñì¬
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"

/**
 * @brief EditorAppƒNƒ‰ƒX
 */
class EditorApp : public Singleton<EditorApp>
{
	friend class Singleton<EditorApp>;
public:

	inline void SetPaused(_In_ bool In_IsPaused) noexcept { m_IsPaused = In_IsPaused; }

	inline bool IsPaused() const noexcept { return m_IsPaused; }

	void Step() noexcept;

private:
	EditorApp() = default;
	~EditorApp() = default;

private:
	bool m_IsPaused = false;
};
