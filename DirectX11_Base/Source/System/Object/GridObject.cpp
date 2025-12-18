/*+===================================================================
	File: GridObject.cpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/18 Thu PM 07:17:36 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "GridObject.hpp"

GridObject::GridObject()
	: GridObject("GridObject")
{
}

GridObject::GridObject(std::string In_Name)
	: GameObject(In_Name)
{
}

GridObject::~GridObject()
{
}
