/*+===================================================================
	File: InstancedMesh.cpp
	Summary: インスタンシング用のメッシュクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/1 Mon AM 04:01:07 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "InstancedMesh.hpp"

InstancedMesh::InstancedMesh()
	: m_spMeshBuffer(nullptr), m_spMaterial(nullptr)
{
}

InstancedMesh::~InstancedMesh()
{
}
