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
	: SpriteManagerWindow("Inspector")
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

void SpriteManagerInspector::Draw(_In_opt_ Sprite *In_2DSprite = nullptr, _In_opt_ Sprite *In_3DSprite = nullptr) noexcept
{
	// 2Dスプライトの情報を表示
	if (In_2DSprite)
	{
		m_Pos2D = In_2DSprite->GetPosition();
		m_Scale2D = In_2DSprite->GetScale();
		m_Rotation2D = In_2DSprite->GetRotation();
		m_Layer2D = In_2DSprite->GetLayer();
		strcpy_s(m_cFilePath2D, In_2DSprite->GetFilePath().c_str()); // ファイルパスを取得

		if (ImGui::CollapsingHeader(In_2DSprite->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Position", &m_Pos2D.x, "%.2f");
			ImGui::InputFloat3("Scale", &m_Scale2D.x, "%.2f");
			ImGui::InputFloat3("Rotation", &m_Rotation2D.x, "%.2f");
			ImGui::InputInt("Layer", &m_Layer2D);
			ImGui::InputText("2DTexturePath", m_cFilePath2D, cx_MaxStringLength, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::Button("Convert to 3D"))
				SpriteManager::GetInstance().ConvertTo3D();
		}

		// 2Dスプライトのパラメータを更新
		In_2DSprite->SetPosition(m_Pos2D);
		In_2DSprite->SetScale(m_Scale2D);
		In_2DSprite->SetRotation(m_Rotation2D);
		In_2DSprite->SetLayer(m_Layer2D);
	}

	// 3Dスプライトの情報を表示
	if (In_3DSprite)
	{
		m_Pos3D = In_3DSprite->GetPosition();
		m_Scale3D = In_3DSprite->GetScale();
		m_Rotation3D = In_3DSprite->GetRotation();
		m_Layer3D = In_3DSprite->GetLayer();
		m_bIsBillBoard = In_3DSprite->GetIsBillBoard();
		strcpy_s(m_cFilePath3D, In_3DSprite->GetFilePath().c_str()); // ファイルパスを取得

		if (ImGui::CollapsingHeader(In_3DSprite->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("3DPosition", &m_Pos3D.x, "%.2f");
			ImGui::InputFloat3("3DScale", &m_Scale3D.x, "%.2f");
			ImGui::InputFloat3("3DRotation", &m_Rotation3D.x, "%.2f");
			ImGui::InputInt("3DLayer", &m_Layer3D);
			ImGui::Checkbox("Is BillBoard", &m_bIsBillBoard);
			ImGui::InputText("3DTexturePath", m_cFilePath3D, cx_MaxStringLength, ImGuiInputTextFlags_ReadOnly);

			if(ImGui::Button("Convert to 2D"))
			{
				SpriteManager::GetInstance().ConvertTo2D();
			}
		}

		// 3Dスプライトのパラメータを更新
		In_3DSprite->SetPosition(m_Pos3D);
		In_3DSprite->SetScale(m_Scale3D);
		In_3DSprite->SetRotation(m_Rotation3D);
		In_3DSprite->SetLayer(m_Layer3D);
		In_3DSprite->SetBillBoard(m_bIsBillBoard);
	}
}
