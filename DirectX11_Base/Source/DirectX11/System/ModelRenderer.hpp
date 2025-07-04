/*+===================================================================
	File: ModelRenderer.hpp
	Summary: モデルレンダラー 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/01 Tue AM 10:45:12 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

#include "System/Component/Component.hpp"
#include "Model.hpp"

class ModelRenderer : public Component
{
public:
    ModelRenderer();
    ~ModelRenderer();
    void Execute() final;

	/// <summary>
	/// モデルへのポインタを取得します。
	/// </summary>
	/// <returns>現在のオブジェクトに関連付けられている Model 型のポインタ。</returns>
	inline Model *GetModel() const noexcept { return m_pModel; }

    /// <summary>
    /// データアクセサーを使用して読み書きを行います。
    /// </summary>
    /// <param name="In_Data">読み書き操作に使用する DataAccessor 型のポインタ。</param>
    void ReadWrite(_In_ DataAccessor *In_Data) override final;

    /// <summary>
    /// モデルのパスを設定します。
    /// </summary>
    /// <param name="[In_File]">設定するモデルファイルのパス。</param>
    void SetModelPath(_In_ FilePath In_File) noexcept;
//#if _DEBUG
//    void Debug(debug::Window *window) final;
//#endif

private:
    char m_cModelName[MAX_PATH];
    Model *m_pModel;
    float m_fScale;
};
