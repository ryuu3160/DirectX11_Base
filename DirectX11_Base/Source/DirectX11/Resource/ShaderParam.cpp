/*+===================================================================
	File: ShaderParam.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/6 Sat AM 01:14:30 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ShaderParam.hpp"

ShaderParam::~ShaderParam()
{
	if (m_pParam)
	{
		delete m_pParam;
		m_pParam = nullptr;
	}
}
