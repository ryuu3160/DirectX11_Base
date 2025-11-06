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
#include "DirectX11/Renderer/SpriteRenderer.hpp"
#include "DirectX11/Resource/ShaderManager.hpp"

void FadeManager::SetCamera(_In_ GameObject *In_Camera) noexcept
{
	m_pCameraObj = In_Camera;
}

void FadeManager::Update() noexcept
{
	for (auto &itr : m_mapFadeObj)
	{
		if (itr.second.IsStartFadeIn)
		{
			// フェードイン処理
			UpdateFadeIn(itr.second);
		}
		else if (itr.second.IsStartFadeOut)
		{
			// フェードアウト処理
			UpdateFadeOut(itr.second);
		}
	}

	for(auto itr = m_mapFadeObj.begin(); itr != m_mapFadeObj.end(); )
	{
		// フェードが終了していない場合は時間を進めて次へ
		if (itr->second.EaseData.fNowTime < itr->second.EaseData.fDuration)
		{
			itr->second.EaseData.fNowTime += 1.0f / 60.0f;
			++itr;
			continue;
		}


		// フェードが終了している場合はフラグを更新
		itr->second.IsFadeEnd = true;

		// 自動削除フラグが立っている場合は削除
		if(itr->second.IsAutoDelete)
		{
			// フェード用オブジェクトの破棄
			if (itr->second.pFadeObj)
			{
				delete itr->second.pFadeObj;
				itr->second.pFadeObj = nullptr;
			}
			// マップから削除
			itr = m_mapFadeObj.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void FadeManager::AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ Ease::EasingType In_EaseType, _In_ bool In_IsAutoDelete,
	_In_ FilePath In_TexturePath, _In_ std::string_view In_PixcelShaderName, _In_ std::string_view In_VertexShaderName) noexcept
{
	FadeInfo info{};
	// フェード情報の設定
	info.IsAutoDelete = In_IsAutoDelete;
	info.IsStartFadeIn = false;
	info.IsStartFadeOut = false;
	info.IsFadeEnd = false;
	info.PixelShaderName = In_PixcelShaderName;
	info.VertexShaderName = In_VertexShaderName;
	// テクスチャパスが指定されていない場合はデフォルトのテクスチャを使用
	if(In_TexturePath != "")
		info.TexturePath = In_TexturePath;
	else
		info.TexturePath = "Assets/Texture/fade.jpg";
	info.EaseType = In_EaseType;

	// イージングデータの設定
	info.EaseData.fNowTime = 0.0f;
	info.EaseData.fDuration = In_FadeTime;
	info.EaseData.fStart = 0.0f;
	info.EaseData.fEnd = 1.0f;
	// フェード用オブジェクトの作成
	info.pFadeObj = CreateFadeObj(In_Name, info);
	m_mapFadeObj.insert({ std::string(In_Name), info });
}

void FadeManager::AddFade(_In_ std::string_view In_Name, _In_ float In_FadeTime, _In_ const FadeInfo &In_FadeInfo) noexcept
{
	// フェード情報の設定
	FadeInfo info = In_FadeInfo;
	info.EaseData.fNowTime = 0.0f;
	info.EaseData.fDuration = In_FadeTime;
	info.EaseData.fStart = 0.0f;
	info.EaseData.fEnd = 1.0f;
	info.IsStartFadeIn = false;
	info.IsStartFadeOut = false;
	info.IsFadeEnd = false;
	// フェード用オブジェクトの作成
	info.pFadeObj = CreateFadeObj(In_Name, In_FadeInfo);
	m_mapFadeObj.insert({ std::string(In_Name), info });
}

void FadeManager::StartFadeIn(_In_ std::string_view In_Name) noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		// フェードイン開始(フェードアウト中の場合はフェードインに切り替え)
		itr->second.IsStartFadeIn = true;
		itr->second.IsStartFadeOut = false;
		itr->second.IsFadeEnd = false;
		itr->second.EaseData.fNowTime = 0.0f;
	}
}

void FadeManager::StartFadeOut(_In_ std::string_view In_Name) noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		// フェードアウト開始(フェードイン中の場合はフェードアウトに切り替え)
		itr->second.IsStartFadeIn = false;
		itr->second.IsStartFadeOut = true;
		itr->second.IsFadeEnd = false;
		itr->second.EaseData.fNowTime = 0.0f;
	}
}

void FadeManager::StopFade(_In_ std::string_view In_Name) noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		// フェード停止
		itr->second.IsStartFadeIn = false;
		itr->second.IsStartFadeOut = false;
		itr->second.IsFadeEnd = false;
		itr->second.EaseData.fNowTime = 0.0f;
	}
}

void FadeManager::DeleteFade(_In_ std::string_view In_Name) noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		// フェード用オブジェクトの破棄
		if (itr->second.pFadeObj)
		{
			delete itr->second.pFadeObj;
			itr->second.pFadeObj = nullptr;
		}
		// マップから削除
		m_mapFadeObj.erase(itr);
	}
}

bool FadeManager::IsFadeEnd(_In_ std::string_view In_Name) const noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		return itr->second.IsFadeEnd;
	}

	return false;
}

float FadeManager::GetFadeStatus(_In_ std::string_view In_Name) const noexcept
{
	auto itr = m_mapFadeObj.find(std::string(In_Name));
	if (itr != m_mapFadeObj.end())
	{
		if (itr->second.pFadeObj)
		{
			auto cmp = itr->second.pFadeObj->GetComponent<SpriteRenderer>();
			auto col = cmp->GetColor();
			return col.w;
		}
	}
	return 0.0f;
}

FadeManager::FadeManager()
	: m_pCameraObj(nullptr)
{
	m_mapFadeObj.clear();
}
FadeManager::~FadeManager()
{
	for (auto &pair : m_mapFadeObj)
	{
		if (pair.second.pFadeObj)
		{
			delete pair.second.pFadeObj;
			pair.second.pFadeObj = nullptr;
		}
	}
	m_mapFadeObj.clear();
}

void FadeManager::UpdateFadeIn(_Inout_ FadeInfo &InOut_FadeInfo) noexcept
{
	float alpha;
	if(InOut_FadeInfo.EaseType >= Ease::EasingType::MAX)
		alpha = 1.0f - InOut_FadeInfo.EaseData.fNowTime / InOut_FadeInfo.EaseData.fDuration;
	else
		alpha = 1.0f - Ease::Easing(InOut_FadeInfo.EaseType, InOut_FadeInfo.EaseData);

	// alpha値を設定
	auto cmp = InOut_FadeInfo.pFadeObj->GetComponent<SpriteRenderer>();
	auto col = cmp->GetColor();
	// 現在の時間が総時間を超えている場合は0.0fにする
	if (InOut_FadeInfo.EaseData.fNowTime >= InOut_FadeInfo.EaseData.fDuration)
		alpha = 0.0f;
	else
		col.w = alpha;
	cmp->SetColor(col);
	InOut_FadeInfo.pFadeObj->ExecuteUpdate();
}

void FadeManager::UpdateFadeOut(_Inout_ FadeInfo &InOut_FadeInfo) noexcept
{
	float alpha;
	if (InOut_FadeInfo.EaseType >= Ease::EasingType::MAX)
		alpha = InOut_FadeInfo.EaseData.fNowTime / InOut_FadeInfo.EaseData.fDuration;
	else
		alpha = Ease::Easing(InOut_FadeInfo.EaseType, InOut_FadeInfo.EaseData);
	// alpha値を設定
	auto cmp = InOut_FadeInfo.pFadeObj->GetComponent<SpriteRenderer>();
	auto col = cmp->GetColor();
	// 現在の時間が総時間を超えている場合は1.0fにする
	if (InOut_FadeInfo.EaseData.fNowTime >= InOut_FadeInfo.EaseData.fDuration)
		alpha = 1.0f;
	else
		col.w = alpha;
	cmp->SetColor(col);
	InOut_FadeInfo.pFadeObj->ExecuteUpdate();
}

GameObject *FadeManager::CreateFadeObj(_In_ std::string_view In_Name, _In_ const FadeInfo &In_FadeInfo)
{
	GameObject *pWork = new GameObject(std::string(In_Name));
	auto cmp = pWork->AddComponent<SpriteRenderer>();
	cmp->SetCamera(m_pCameraObj);
	cmp->SetLayerGroup(LayerGroup_Fade);
	cmp->SetAssetPath(In_FadeInfo.TexturePath.data());
	if (!In_FadeInfo.PixelShaderName.empty())
		cmp->SetPixelShader(ShaderManager::GetInstance().GetShader(In_FadeInfo.PixelShaderName.data()));
	if (!In_FadeInfo.VertexShaderName.empty())
		cmp->SetVertexShader(ShaderManager::GetInstance().GetShader(In_FadeInfo.VertexShaderName.data()));
	cmp->Set3D(false);
	cmp->SetBillBoard(false);
	cmp->Load();
	cmp->SetSize({ static_cast<float>(cx_nWINDOW_WIDTH), static_cast<float>(cx_nWINDOW_HEIGHT) + 6.0f });

	return pWork;
}
