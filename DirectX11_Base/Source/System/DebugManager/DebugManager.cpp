/*+===================================================================
	File: DebugManager.cpp
	Summary: デバッグマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/10 Mon AM 12:31:25 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "DebugManager.hpp"

// ==============================
//	定数定義
// ==============================
namespace
{
}

DebugManager::DebugManager()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::Init()
{

}

void DebugManager::Update() noexcept
{
}

void DebugManager::Draw() noexcept
{
}

DebugWindow *DebugManager::CreateDebugWindow(const std::string_view In_Name)
{
	return nullptr;
}

DebugWindow *DebugManager::GetDebugWindow(const std::string_view In_Name)
{
	return nullptr;
}

void DebugManager::DrawImGui(DebugItem *In_Item) noexcept
{
}
