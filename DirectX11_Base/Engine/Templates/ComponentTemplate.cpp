/*+===================================================================
	File: ComponentTemplate.cpp
	Summary:
	Author: AT13C192 01 ê¬ñÿóYàÍòY
	Date: 2026/3/9 Mon AM 04:52:56 èââÒçÏê¨
===================================================================+*/

// ==============================
//	include
// ==============================
#include "{COMPONENT_NAME}.hpp"
#include "Engine/Core/System/Object/GameObject.hpp"

{COMPONENT_NAME}::{COMPONENT_NAME}()
	: Component("{COMPONENT_NAME}")
{
}

void {COMPONENT_NAME}::SaveLoad(_In_ DataAccessor *In_Data)
{
}

void {COMPONENT_NAME}::Awake() noexcept
{
}

void {COMPONENT_NAME}::Init() noexcept
{
}

void {COMPONENT_NAME}::Update(_In_ float In_DeltaTime) noexcept
{
}

#ifdef _DEBUG
void {COMPONENT_NAME}::Inspector(_In_ ItemGroup *In_pGroup)
{
}
#endif
