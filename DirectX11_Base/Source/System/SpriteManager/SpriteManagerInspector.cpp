/*+===================================================================
	File: SpriteManagerInspector.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:38:06 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManagerInspector.hpp"
#include "SpriteManager.hpp"

SpriteManagerInspector::SpriteManagerInspector()
	: SpriteManagerWindow("Inspector##SpriteManager")
	, m_Pos2D(0.0f, 0.0f, 0.0f), m_Scale2D(1.0f, 1.0f, 1.0f), m_Rotation2D(0.0f, 0.0f, 0.0f), m_Layer2D(0)
	, m_Pos3D(0.0f, 0.0f, 0.0f), m_Scale3D(1.0f, 1.0f, 1.0f), m_Rotation3D(0.0f, 0.0f, 0.0f), m_Layer3D(0)
	, m_bIsBillBoard(false)
{
	m_WindowType = Inspector;
	// 初期化
	m_cFilePath2D[0] = '\0'; // 2Dスプライトのファイルパスの初期化
	m_cFilePath3D[0] = '\0'; // 3Dスプライトのファイルパスの初期化
}

SpriteManagerInspector::~SpriteManagerInspector()
{
}

void SpriteManagerInspector::Draw(_In_opt_ GameObject *In_2DSprite = nullptr, _In_opt_ GameObject *In_3DSprite = nullptr) noexcept
{
	// 2Dスプライトの情報を表示
	if (In_2DSprite)
	{
		auto SR2D = In_2DSprite->GetComponent<SpriteRenderer>();
		if (!SR2D) return; // SpriteRendererコンポーネントが存在しない場合は処理を抜ける
		m_Pos2D = SR2D->GetPositionPixel();
		m_Scale2D = In_2DSprite->GetScale();
		m_Rotation2D = In_2DSprite->GetRotation();
		m_Layer2D = SR2D->GetLayer();
		strcpy_s(m_cFilePath2D, SR2D->GetAssetPath().data()); // ファイルパスを取得

		if (ImGui::CollapsingHeader(In_2DSprite->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Position##2D", &m_Pos2D.x, "%.2f");
			ImGui::InputFloat3("Scale##2D", &m_Scale2D.x, "%.2f");
			ImGui::InputFloat3("Rotation##2D", &m_Rotation2D.x, "%.2f");
			ImGui::InputInt("Layer##2D", &m_Layer2D);
			ImGui::InputText("TexturePath##2D", m_cFilePath2D, cx_MaxStringLength, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::Button("Convert to 3D"))
				SpriteManager::GetInstance().ConvertTo3D();

			if (ImGui::Button("Delete Sprite##2D"))
			{
				SpriteManager::GetInstance().DeleteSprite(In_2DSprite->GetName());
			}
		}

		// 2Dスプライトのパラメータを更新
		SR2D->SetPositionPixel(m_Pos2D);
		In_2DSprite->SetScale(m_Scale2D);
		In_2DSprite->SetRotation(m_Rotation2D);
		SR2D->SetLayer(m_Layer2D);
	}

	// 3Dスプライトの情報を表示
	if (In_3DSprite)
	{
		auto SR3D = In_3DSprite->GetComponent<SpriteRenderer>();
		m_Pos3D = In_3DSprite->GetPosition();
		m_Scale3D = In_3DSprite->GetScale();
		m_Rotation3D = In_3DSprite->GetRotation();
		m_Layer3D = SR3D->GetLayer();
		m_bIsBillBoard = SR3D->GetIsBillBoard();
		strcpy_s(m_cFilePath3D, SR3D->GetAssetPath().data()); // ファイルパスを取得

		if (ImGui::CollapsingHeader(In_3DSprite->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Position##3D", &m_Pos3D.x, "%.2f");
			ImGui::InputFloat3("Scale##3D", &m_Scale3D.x, "%.2f");
			ImGui::InputFloat3("Rotation##3D", &m_Rotation3D.x, "%.2f");
			ImGui::InputInt("Layer##3D", &m_Layer3D);
			ImGui::Checkbox("Is BillBoard##3D", &m_bIsBillBoard);
			ImGui::InputText("TexturePath##3D", m_cFilePath3D, cx_MaxStringLength, ImGuiInputTextFlags_ReadOnly);

			if(ImGui::Button("Convert to 2D"))
				SpriteManager::GetInstance().ConvertTo2D();

			if(ImGui::Button("Delete Sprite##3D"))
				SpriteManager::GetInstance().DeleteSprite(In_3DSprite->GetName());
		}

		// 3Dスプライトのパラメータを更新
		In_3DSprite->SetPosition(m_Pos3D);
		In_3DSprite->SetScale(m_Scale3D);
		In_3DSprite->SetRotation(m_Rotation3D);
		SR3D->SetLayer(m_Layer3D);
		SR3D->SetBillBoard(m_bIsBillBoard);
	}
}
