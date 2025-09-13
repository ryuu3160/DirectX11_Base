/*+===================================================================
	File: FadeManager.cpp
	Summary: フェードの管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/12 Fri PM 02:36:01 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "FadeManager.hpp"
#include "DirectX11/System/SpriteRenderer.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

void FadeManager::Update() noexcept
{
}

void FadeManager::Draw() noexcept
{
}

void FadeManager::AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ bool In_IsAutoDelete) noexcept
{

}

void FadeManager::AddFade(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo) noexcept
{
	FadeInfo info = In_FadeInfo;
	info.pFadeObj = CreateFadeObj(In_Name, In_FadeInfo);
}

void FadeManager::StartFadeIn(_In_ std::string_view In_Name) noexcept
{
}

void FadeManager::StartFadeOut(_In_ std::string_view In_Name) noexcept
{
}

void FadeManager::StopFade(_In_ std::string_view In_Name) noexcept
{
}

void FadeManager::DeleteFade(_In_ std::string_view In_Name) noexcept
{
}

FadeManager::FadeManager()
	: m_pCameraObj(nullptr)
{
}
FadeManager::~FadeManager()
{
}

GameObject *FadeManager::CreateFadeObj(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo)
{
	GameObject *pWork = new GameObject(std::string(In_Name));
	auto cmp = pWork->AddComponent<SpriteRenderer>();
	cmp->SetCamera(m_pCameraObj);
	cmp->SetAssetPath(In_FadeInfo.TexturePath);
	if (!In_FadeInfo.PixelShaderPath.empty())
		cmp->SetPixelShader(ShaderManager::GetInstance().GetShader(In_FadeInfo.PixelShaderPath.data()));
	if (!In_FadeInfo.VertexShaderPath.empty())
		cmp->SetVertexShader(ShaderManager::GetInstance().GetShader(In_FadeInfo.VertexShaderPath.data()));
	cmp->Set3D(false);
	cmp->SetBillBoard(false);

	return pWork;
}
