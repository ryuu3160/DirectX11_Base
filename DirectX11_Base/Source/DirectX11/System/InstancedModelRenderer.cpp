/*+===================================================================
	File: InstancedModelRenderer.cpp
	Summary: インスタンシング用のモデルレンダラークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 05:06:05 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InstancedModelRenderer.hpp"

InstancedModelRenderer::InstancedModelRenderer()
	: m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_fScale(1.0f)
	, m_bUseMaterialShader(false)
	, m_bEnablePS_WriteCamera(false)
	, m_bEnablePS_WriteParamList{}
	, m_pShaderParams{}
{
}

InstancedModelRenderer::~InstancedModelRenderer()
{
}

void InstancedModelRenderer::Draw() noexcept
{
}
