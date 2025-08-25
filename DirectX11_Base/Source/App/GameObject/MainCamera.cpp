/*+===================================================================
	File: MainCamera.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/8/25 Mon AM 11:28:53 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "MainCamera.hpp"

MainCamera::MainCamera()
	: GameObject("MainCamera")
	, m_pComponent(AddComponent<Camera>())
	, m_pPlayer(nullptr)
{

}

MainCamera::~MainCamera()
{
}

void MainCamera::Update()
{
}

void MainCamera::SetTargetPlayer(Player *In_pPlayer) noexcept
{
	m_pPlayer = In_pPlayer;
}
