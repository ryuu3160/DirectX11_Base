/*+===================================================================
	File: FadeManager.hpp
	Summary: フェードの管理クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/12 Fri PM 02:35:53 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// FadeManagerクラス
/// </summary>
class FadeManager : public Singleton<FadeManager>
{
	friend class Singleton<FadeManager>;
public:

	struct FadeInfo
	{
	public:
		float FadeTime;
		bool IsAutoDelete;
		FilePathHold TexturePath{};
		FilePathHold PixelShaderPath{};
		FilePathHold VertexShaderPath{};
	private:
		friend class FadeManager;
		GameObject *pFadeObj = nullptr;
	};

	void SetCamera(_In_ GameObject *In_Camera) noexcept;

	void Update() noexcept;

	void Draw() noexcept;

	void AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ bool In_IsAutoDelete = false) noexcept;

	void AddFade(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo) noexcept;

	void StartFadeIn(_In_ std::string_view In_Name) noexcept;

	void StartFadeOut(_In_ std::string_view In_Name) noexcept;

	void StopFade(_In_ std::string_view In_Name) noexcept;

	void DeleteFade(_In_ std::string_view In_Name) noexcept;

private:
	FadeManager();
	~FadeManager();

	GameObject *CreateFadeObj(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo);

private:
	GameObject *m_pCameraObj;
	std::unordered_map<std::string,FadeInfo> m_FadeObj;
};
