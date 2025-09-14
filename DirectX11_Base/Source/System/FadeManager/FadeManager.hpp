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
		bool IsAutoDelete = false;
		bool IsStartFadeIn = false;
		bool IsStartFadeOut = false;
		bool IsFadeEnd = false;
		Ease::EasingType EaseType = Ease::LERP;
		FilePath TexturePath{};
		FilePathHold PixelShaderName{};
		FilePathHold VertexShaderName{};
	private:
		friend class FadeManager;
		Ease::DataDuration EaseData{};
		GameObject *pFadeObj = nullptr;
	};

	void SetCamera(_In_ GameObject *In_Camera) noexcept;

	void Update() noexcept;

	void Draw() noexcept;

	/// <summary>
	/// 指定した名前のフェードを追加します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	/// <param name="[In_FadeTime]">フェードの時間（秒単位）を指定します</param>
	/// <param name="[In_IsAutoDelete]">フェード完了後に自動的に削除するかどうかを指定します。デフォルトは false です</param>
	void AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ Ease::EasingType In_EaseType = Ease::LERP, _In_ bool In_IsAutoDelete = false,
		_In_ FilePath In_TexturePath = "", _In_ std::string_view In_PixcelShaderName = "", _In_ std::string_view In_VertexShaderName = "") noexcept;

	/// <summary>
	/// 指定された名前にフェード情報を追加します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	/// <param name="[In_FadeTime]">フェードの時間（秒単位）を指定します</param>
	/// <param name="[In_FadeInfo]">追加するフェード情報</param>
	void AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ const FadeInfo &In_FadeInfo) noexcept;

	/// <summary>
	/// 指定された名前のフェードイン処理を開始します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	void StartFadeIn(_In_ std::string_view In_Name) noexcept;

	/// <summary>
	/// 指定された名前のフェードアウト処理を開始します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	void StartFadeOut(_In_ std::string_view In_Name) noexcept;

	/// <summary>
	/// 指定された名前のフェード処理を停止します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	void StopFade(_In_ std::string_view In_Name) noexcept;

	/// <summary>
	/// 指定された名前のフェードを削除します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	void DeleteFade(_In_ std::string_view In_Name) noexcept;

	/// <summary>
	/// 指定された名前のフェードが終了しているかどうかを確認します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	/// <returns>フェードが終了しているかの真偽値</returns>
	bool IsFadeEnd(_In_ std::string_view In_Name) const noexcept;

	/// <summary>
	/// フェードの状態を取得します。取得する値はアルファ値で、0.0fが完全に透明、1.0fが完全に不透明を表します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	/// <returns>0.0f~1.0fのアルファ値</returns>
	float GetFadeStatus(_In_ std::string_view In_Name) const noexcept;

private:
	FadeManager();
	~FadeManager();

	void UpdateFadeIn(_Inout_ FadeInfo &InOut_FadeInfo) noexcept;
	void UpdateFadeOut(_Inout_ FadeInfo &InOut_FadeInfo) noexcept;

	/// <summary>
	/// 指定された名前とフェード情報を使用して、フェード用のGameObjectを作成します。
	/// </summary>
	/// <param name="[In_Name]">フェード名</param>
	/// <param name="[In_FadeInfo]">フェード効果の詳細情報を含むFadeInfo構造体。</param>
	/// <returns>作成されたフェード用GameObjectへのポインタ。</returns>
	GameObject *CreateFadeObj(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo);

private:
	GameObject *m_pCameraObj;
	std::unordered_map<std::string,FadeInfo> m_mapFadeObj;
};
