/*+===================================================================
	File: GridObject.hpp
	Summary: グリッドを表示するオブジェクト
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 07:17:29 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "GameObject.hpp"

// ==============================
//	前方宣言
// ==============================
class LineRenderer;
class GridComponent;

/// <summary>
/// GridObjectクラス
/// </summary>
class GridObject : public GameObject
{
public:
	GridObject();
	GridObject(_In_ std::string_view In_Name);
	~GridObject();

	void Init() noexcept override;

	void Awake() noexcept override;

private:
	LineRenderer *m_pRenderComponent; // スカイボックス描画コンポーネント
	GridComponent *m_pGridComponent; // グリッドコンポーネント
};
