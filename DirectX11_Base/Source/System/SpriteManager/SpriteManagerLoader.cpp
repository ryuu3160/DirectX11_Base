/*+===================================================================
	File: SpriteManagerLoader.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:54:38 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManagerLoader.hpp"
#include "SpriteManager.hpp"

SpriteManagerLoader::SpriteManagerLoader()
	: SpriteManagerWindow("Loader")
{
	m_WindowType = Loader;
	// 初期化
	m_cSpriteName[0] = '\0'; // スプライト名の初期化
	m_cFilePath[0] = '\0'; // ファイルパスの初期化
}

SpriteManagerLoader::~SpriteManagerLoader()
{
}

void SpriteManagerLoader::Draw() noexcept
{
	ImGui::Text("Load Sprite");

	// スプライト名の入力
	ImGui::InputText("Sprite Name", m_cSpriteName, cx_MaxStringLength);

	// ファイルパスの入力
	ImGui::InputText("File Path", m_cFilePath, cx_MaxStringLength);

	// レイヤー番号の入力
	ImGui::InputInt("Layer", &m_nLayer);

	if (ImGui::BeginTable("split", 3))
	{
		// 3Dスプライトかどうかのチェックボックス
		ImGui::TableNextColumn();
		ImGui::Checkbox("Is 3D", &m_bIs3D);

		// ビルボードスプライトかどうかのチェックボックス
		if (m_bIs3D)
		{
			ImGui::TableNextColumn();
			ImGui::Checkbox("Is BillBoard", &m_bIsBillBoard);
		}
		else
			m_bIsBillBoard = false; // 2Dスプライトではビルボードは無効

		ImGui::EndTable();
	}

	// スケールの入力
	ImGui::InputFloat("Scale", &m_fScale, 0.1f, 1.0f, "%.2f");

	if (ImGui::Button("Load Sprite"))
	{
		// スプライトのロード
		if (m_cFilePath)
		{
			std::string_view FilePath = m_cFilePath;
			std::string_view SpriteName = m_cSpriteName;

			auto &Instance = SpriteManager::GetInstance();

			// スプライト名が重複している場合は追加しない
			if(Instance.CheckConflictSpriteName(SpriteName, m_bIs3D ? SpriteManager::_3D : SpriteManager::_2D))
			{
				MessageBoxA(nullptr, "同名のスプライトが存在しています。", "エラー", MB_OK | MB_ICONERROR);
				return;
			}

			Instance.CreateSprite(SpriteName, FilePath, m_bIs3D, m_bIsBillBoard, m_nLayer, m_fScale);
			// スプライトのロードが成功したら、入力フィールドをクリア
			m_cSpriteName[0] = '\0'; // スプライト名のクリア
			m_cFilePath[0] = '\0'; // ファイルパスのクリア
			m_nLayer = 0; // レイヤー番号のクリア
			m_fScale = 1.0f; // スケールのクリア
			m_bIs3D = false; // 3Dスプライトのチェックボックスを初期化
			m_bIsBillBoard = false; // ビルボードスプライトのチェックボックスを初期化
		}
	}
}
