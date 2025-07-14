/*+===================================================================
	File: SpriteManager.cpp
	Summary: スプライトマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/14 Mon AM 09:55:21 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManager.hpp"

void SpriteManager::Init() noexcept
{

}

void SpriteManager::SetCameraViewAndCameraProj(_In_ Camera *In_Camera) noexcept
{
	bool prev = In_Camera->Is3D();
	m_View = In_Camera->GetView();
	m_BillBoardView = In_Camera->GetView(false);
	In_Camera->Set3D(true);
	m_Projection3D = In_Camera->GetProj();
	In_Camera->Set3D(false);
	m_Projection2D = In_Camera->GetProj();
	In_Camera->Set3D(prev);
}

void SpriteManager::Draw() noexcept
{
	// 現在のディレクトリのスプライトを描画
	for (const auto &itr : m_SpriteList)
	{
		if (itr)
		{
			if (itr->GetIs3D())
			{
				itr->SetView(m_View);
				itr->SetProjection(m_Projection3D);
				// ビルボードの場合は専用のView行列を設定
				if (itr->GetIsBillBoard())
					itr->SetBillBoardView(m_BillBoardView);
			}
			else
			{
				DirectX::XMFLOAT4X4 view;
				DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
				DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
				itr->SetView(view);
				itr->SetProjection(m_Projection2D);
			}
			itr->Draw();
		}
	}
}

std::list<Sprite *> SpriteManager::GetSpriteList() const noexcept
{
	return m_SpriteList;
}

Sprite *SpriteManager::GetSprite(_In_ const std::string_view &In_SpriteName) const noexcept
{

	auto spriteItr = m_Sprites.find(In_SpriteName.data());
	if (spriteItr != m_Sprites.end())
	{
		return spriteItr->second;
	}

	return nullptr;
}

Sprite *SpriteManager::CreateSprite(_In_ const std::string_view &In_SpriteName, _In_ const FilePath &In_FilePath, _In_ const float &In_Scale) noexcept
{
	auto sprite = m_Sprites.find(In_SpriteName.data());
	if(sprite != m_Sprites.end())
	{
		return sprite->second; // 既に存在する場合はそのまま返す
	}

	Sprite *work = new Sprite();
	work->Load(In_FilePath,In_Scale);

	m_Sprites.insert({ std::string(In_SpriteName), work });
	m_SpriteList.push_back(work); // スプライトリストに追加
	return work;
}

void SpriteManager::DeleteAll() noexcept
{
	for(auto &sprite : m_Sprites)
	{
		delete sprite.second; // スプライトオブジェクトを削除
		sprite.second = nullptr; // ポインタをnullptrに設定
	}
	m_Sprites.clear(); // スプライトリストをクリア
	m_SpriteList.clear();
}

SpriteManager::SpriteManager() : m_View(), m_Projection3D(), m_Projection2D()
{
}

SpriteManager::~SpriteManager()
{
	DeleteAll(); // デストラクタで全てのスプライトを削除
}
