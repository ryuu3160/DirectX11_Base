/*+===================================================================
	File: SpriteRenderer.cpp
	Summary: SpriteRendererクラス ModelRendererの実装を参考にして作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/14 Mon AM 05:48:30 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer()
	: m_pSprite(nullptr)
	, m_fScale(1.0f)
	, m_cSpriteName{}
{
}

SpriteRenderer::~SpriteRenderer()
{
	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = nullptr;
	}
}

void SpriteRenderer::Execute()
{
	if (m_pSprite) return;
	m_pSprite = new Sprite();
	m_pSprite->Load(m_cSpriteName, m_fScale);
}

void SpriteRenderer::ReadWrite(_In_ DataAccessor *In_Data)
{
	size_t size = strlen(m_cSpriteName);
	In_Data->Access<size_t>(&size);
	for (size_t i = 0; i < size; ++i)
		In_Data->Access<char>(&m_cSpriteName[i]);
	m_cSpriteName[size] = '\0';
}

void SpriteRenderer::SetSpritePath(_In_ FilePath In_File) noexcept
{
	// 長さを超えないようにコピー
	size_t len = std::min(In_File.size(), sizeof(m_cSpriteName) - 1);
	std::memcpy(m_cSpriteName, In_File.data(), len);
	m_cSpriteName[len] = '\0'; // 終端
}
