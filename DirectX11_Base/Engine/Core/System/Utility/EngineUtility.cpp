/*+===================================================================
	File: EngineUtility.cpp
	Summary: エンジン用のユーティリティ関数群
	Author: AT13C192 01 青木雄一郎
	Date: 2026/1/23 Fri AM 06:06:15 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "EngineUtility.hpp"
#include "Engine/Core/System/Managers/DebugManager/DebugManager.hpp"

void Engine::Debug::DebugLog(_In_ std::string_view In_Message)
{
	DebugManager::GetInstance().DebugLog(In_Message);
}

void Engine::Debug::DebugLogWarning(_In_ std::string_view In_Message)
{
	DebugManager::GetInstance().DebugLogWarning(In_Message);
}

void Engine::Debug::DebugLogError(_In_ std::string_view In_Message)
{
	DebugManager::GetInstance().DebugLogError(In_Message);
}

bool Engine::IsValid(_In_opt_ void *In_Ptr, _In_ std::string_view In_Message)
{
	if(In_Ptr == nullptr)
	{
		DebugManager::GetInstance().DebugLogError("Null Pointer Exception: {}", In_Message);
		return false;
	}
	return true;
}

void Engine::Assert(_In_ bool In_Condition, _In_ std::string_view In_Message)
{
	if(!In_Condition)
	{
		DebugManager::GetInstance().DebugLogError("Assertion Failed: {}", In_Message);
	}
}
