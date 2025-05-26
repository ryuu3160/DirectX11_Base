/*+===================================================================
	File: Main.cpp
	Summary: ゲームのメイン処理
	Author: AT13C192 01 青木雄一郎
	Date: 2025/5/22 Thu PM 06:14:25 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Main.hpp"
#include "DirectX11/DX11_Initialize.hpp"


HRESULT Main::Init(HWND hWnd, int width, int height)
{
	HRESULT hr;

	// DirectXの初期化
	hr = DX11_Initialize::GetInstance().Init();
	if (FAILED(hr))
		return hr;



	return hr;
}

void Main::Uninit()
{
	
}

void Main::Update()
{
	
}

void Main::Draw()
{
	DX11_Initialize::GetInstance().BeginDraw();


	DX11_Initialize::GetInstance().EndDraw();
}
