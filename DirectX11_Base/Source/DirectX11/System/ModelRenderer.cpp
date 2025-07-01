/*+===================================================================
	File: ModelRenderer.cpp
	Summary: モデルレンダラー 佐々木先生のコードを参考に実装
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/01 Tue AM 10:45:21 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ModelRenderer.hpp"

ModelRenderer::ModelRenderer()
{
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::Execute()
{
	if (m_pModel) return;
	m_pModel = new Model();
	m_pModel->Load(m_modelName, m_scale);
}

Model *ModelRenderer::GetModel()
{
	return nullptr;
}
