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

    Model *GetModel();

    void ReadWrite(DataAccessor *data) final;
//#if _DEBUG
//    void Debug(debug::Window *window) final;
//#endif

private:
    char m_cModelName[MAX_PATH];
    Model *m_pModel;
    float m_fScale;
};
