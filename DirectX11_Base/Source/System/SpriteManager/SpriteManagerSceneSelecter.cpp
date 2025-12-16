/*+===================================================================
	File: SpriteManagerSceneSelecter.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/7/27 Sun PM 11:29:55 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SpriteManagerSceneSelecter.hpp"
#include "SpriteManager.hpp"

SpriteManagerSceneSelecter::SpriteManagerSceneSelecter()
	: SpriteManagerWindow("SceneSelecter## SpriteManager")
{
	m_WindowType = SceneSelecter;
	m_NewSceneName[0] = '\0';
}

SpriteManagerSceneSelecter::~SpriteManagerSceneSelecter()
{
}

void SpriteManagerSceneSelecter::Draw(_In_ std::map<std::string, FilePathHold> In_Scenes, _Inout_ int *Inout_Index) noexcept
{
	const char *scenes[100];
	if (!In_Scenes.empty())
	{
		auto itr = In_Scenes.begin();
		for (int i = 0; i < In_Scenes.size() && i < 100; ++i, ++itr)
		{
			scenes[i] = (*itr).first.c_str();
		}
	}

	if (ImGui::ListBox("Scenes", Inout_Index, scenes, static_cast<int>(In_Scenes.size())))
	{
		SpriteManager::GetInstance().ChangeScene();
	}

	if (ImGui::Button("DeleteScene"))
		SpriteManager::GetInstance().DeleteScene();

	for(int i = 0; i < 3;++i)
		ImGui::Spacing();

	if(ImGui::InputText("NewSceneName", m_NewSceneName, cx_MaxStringLength))
	{
		int i = 0;
	}

	if(ImGui::Button("AddScene") && m_NewSceneName[0] != '\0')
	{
		SpriteManager::GetInstance().CreateScene(m_NewSceneName);
		m_NewSceneName[0] = '\0';
	}
}
