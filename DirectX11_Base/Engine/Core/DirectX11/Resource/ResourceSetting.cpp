/*+===================================================================
	File: ResourceSetting.cpp
	Summary: リソース設定に関するユーティリティ関数群
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/18 Mon PM 03:59:53 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ResourceSetting.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"

namespace ResourceSetting
{
	std::string CreateMaterialName(_In_ const FilePath &In_FilePath, _In_ const aiMaterial *In_pMaterial) noexcept
	{
		std::string MaterialName;

		aiString MatName;
		std::string FbxName = std::string(In_FilePath);
		FbxName = FbxName.substr(FbxName.find_last_of('/') + 1);
		MatName = In_pMaterial->GetName();
		MaterialName = FbxName + "_" + MatName.C_Str();

		return MaterialName;
	}
	std::string ExtractFbxNameFromMaterialName(_In_ const std::string &In_MaterialName, _In_ const bool &In_RemoveExtension) noexcept
	{
		std::string FbxName = In_MaterialName;
		size_t pos = FbxName.find('_');
		if (pos == std::string::npos)
		{
			DebugManager::GetInstance().DebugLogError("Invalid material name format. Expected format: 'FbxName_MaterialName'.");
			return "";
		}

		FbxName = FbxName.substr(0, pos);
		
		// 拡張子を削除する場合
		if (In_RemoveExtension)
		{
			size_t extPos = FbxName.find_last_of('.');
			if (extPos != std::string::npos)
			{
				FbxName = FbxName.substr(0, extPos);
			}
		}

		return FbxName;
	}
}