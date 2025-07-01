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
	: m_pModel(nullptr)
	, m_cModelName{}
	, m_fScale(1.0f)
{
}

ModelRenderer::~ModelRenderer()
{
	if (m_pModel)
		delete m_pModel;
}

void ModelRenderer::Execute()
{
	if (m_pModel) return;
	m_pModel = new Model();
	m_pModel->Load(m_cModelName, m_fScale);
}

void ModelRenderer::ReadWrite(_In_ DataAccessor *In_Data)
{
	size_t size = strlen(m_cModelName);
	In_Data->Access<size_t>(&size);
	for (size_t i = 0; i < size; ++i)
		In_Data->Access<char>(&m_cModelName[i]);
	m_cModelName[size] = '\0';
}
