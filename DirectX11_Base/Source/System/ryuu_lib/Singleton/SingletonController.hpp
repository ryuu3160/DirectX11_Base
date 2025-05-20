/*+===================================================================
    File: SingletonController.hpp
    Summary: 自作シングルトン
    Author: ryuu3160
    Date: 2024/08/16   初回作成

    (C) 2024 ryuu3160. All Rights Reserved.
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <mutex>
#include <vector>
#include <algorithm>

class SingletonController final
{
public:
    using ControllerFunc = void(*)();   // 関数ポインタ

    /// <summary>
    /// シングルトンクラスの解放処理を追加
    /// </summary>
    /// <param name="[func]">destroy関数のポインタ</param>
    static void AddController(ControllerFunc func);

    /// <summary>
    /// 全てのシングルトンクラスの解放処理を呼び出す
    /// </summary>
    static void Release();
};