/*+===================================================================
	File: Player.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 04:04:37 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
	// ミサイル関連
	const inline constexpr int cx_MissileMax = 4; // ミサイルの最大数
}

/// <summary>
/// Playerクラス
/// </summary>
class PlayerObj : public GameObject
{
public:
	PlayerObj(_In_ std::string_view In_Name = "PlayerObj");
	~PlayerObj();

	void Awake() noexcept override;
	void Init() noexcept override;

private:
};
