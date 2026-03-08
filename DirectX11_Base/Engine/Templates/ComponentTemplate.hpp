/*+===================================================================
	File: ComponentTemplate.hpp
	Summary:
	Author: AT13C192 01 青木雄一郎
	Date: 2026/3/9 Mon AM 04:52:38 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine.hpp"

// ---ComponentDescription---
// {COMPONENT_DESCRIPTION}
// --------------------------

/// <summary>
/// {COMPONENT_NAME}
/// </summary>
class {COMPONENT_NAME} : public Component
{
public:
	{COMPONENT_NAME}();
	~{COMPONENT_NAME}() override = default;

	/// <summary>
	/// データの保存/読み込みを行う関数
	/// </summary>
	void SaveLoad(_In_ DataAccessor *In_Data) override;

	/// <summary>
	/// コンポーネント生成時に呼び出される初期化関数
	/// </summary>
	void Awake() noexcept override;

	/// <summary>
	/// コンポーネントが有効かつ、すべてのコンポーネントのAwake()が呼び出された後に呼び出される初期化関数
	/// </summary>
	void Init() noexcept override;

	/// <summary>
	/// 毎フレーム呼び出される更新関数
	/// </summary>
	void Update(_In_ float In_DeltaTime) noexcept override;

protected:

#ifdef _DEBUG

	/// <summary>
	/// デバッグインスペクターに公開するための関数
	/// </summary>
	void Inspector(_In_ ItemGroup *In_pGroup) override;
#endif

private:
    // メンバー変数をここに追加
};
