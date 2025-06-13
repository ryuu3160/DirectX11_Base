/*+===================================================================
  File : Input.hpp
  Summary : キーボードの入力処理を行う
  Author : ryuu3160
  Date : 2024/10/16

  (C) 2024 ryuu3160. All Rights Reserved.
===================================================================+*/
#pragma once

#include <Windows.h>
#undef max
#undef min

namespace Input
{
/// <summary>
/// 入力処理の初期化
/// </summary>
/// <returns>HRESULT型</returns>
HRESULT Init();
/// <summary>
/// 入力処理の終了
/// </summary>
void Uninit();
/// <summary>
/// 入力処理の更新
/// </summary>
void Update();

/// <summary>
/// キーが押されているか
/// </summary>
/// <param name="[key]">キーコード</param>
/// <returns>true:押されている false:押されていない</returns>
bool IsKeyPress(BYTE key);
/// <summary>
/// キーが押された瞬間か
/// </summary>
/// <param name="[key]">キーコード</param>
/// <returns>押された瞬間か否か</returns>
bool IsKeyTrigger(BYTE key);
/// <summary>
/// キーが離された瞬間か
/// </summary>
/// <param name="[key]">キーコード</param>
/// <returns>離された瞬間か否か</returns>
bool IsKeyRelease(BYTE key);

/// <summary>
/// キーがリピートされているか
/// </summary>
/// <param name="[key]">キーコード</param>
/// <returns>リピートされているか否か</returns>
bool IsKeyRepeat(BYTE key);

} // namespace Input