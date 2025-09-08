/*+===================================================================
	File: SoundManager.cpp
	Summary: サウンドマネージャークラス
	Author: 井上楓太、青木雄一郎
	Date: 2025/9/9 Tue AM 01:15:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SoundManager.hpp"

SoundManager::SoundManager() noexcept
	: CMP_MATCH(0), m_pXAudio(nullptr), m_pMasterVoice(nullptr)
	, m_fBGMVolume(0.5f), m_fSEVolume(0.5f)
{
	HRESULT hr = E_FAIL;

	// COMオブジェクト初期化
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		Error("error: COMオブジェクトの初期化に失敗しました : " + hr);
		return;
	}

	// XAUDIO2初期化
	hr = XAudio2Create(&m_pXAudio);
	if (FAILED(hr))
	{
		Error("error: XAUDIO2の初期化に失敗しました : " + hr);
		return;
	}

	// マスターボイス(ソース)作成
	hr = m_pXAudio->CreateMasteringVoice(&m_pMasterVoice);

	if (FAILED(hr))
	{
		Error("error: マスターボイス(ソース)作成に失敗しました : " + hr);
		return;
	}
}

SoundManager::~SoundManager()
{
}
