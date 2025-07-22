/*+===================================================================
	File: RenderManager.hpp
	Summary: 全ての描画を管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/20 Sun PM 09:43:49 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "RenderComponent.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// RenderManagerクラス
/// </summary>
class RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;
public:


	// ------------------------------
	//  Getter
	// ------------------------------


	// ------------------------------
	//  Setter
	// ------------------------------

private:
	RenderManager() = default;
	~RenderManager() = default;
};
