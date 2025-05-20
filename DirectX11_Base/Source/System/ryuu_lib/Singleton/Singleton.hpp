/*+==========================================================================
  File : Singleton.h
  Author : ryuu3160
  Date : 2024/07/27
  Note : mozc式シングルトン
  
  (C) 2024 ryuu3160. All Rights Reserved.
=============================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SingletonController.hpp"

/// <summary>
/// シングルトンクラス
/// </summary>
/// <typeparam name="[T]">シングルトンにするクラス</typeparam>
template<typename T>
class Singleton
{
public:
    /// <summary>
    /// インスタンスの取得
    /// </summary>
    /// <returns>インスタンスへの参照</returns>
    static T &GetInstance()
    {
        // インスタンスが無い場合は作成
        if(m_instance == nullptr)
            std::call_once(m_onceFlag, create);

        return *m_instance;
    }

protected:
    // デフォルトコンストラクタ、デストラクタ
    Singleton() = default;
    virtual ~Singleton() = default;

    // コピー・代入禁止
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    /// <summary>
    /// インスタンスの作成
    /// </summary>
    static void create()
    {
        m_instance = new T;
        SingletonController::AddController(&Singleton<T>::destroy);
    }
    /// <summary>
    /// 生成したインスタンスの破棄
    /// </summary>
    static void destroy()
    {
        delete m_instance;
        m_instance = nullptr;
    }

    static std::once_flag m_onceFlag;   // 一度だけ初期化するためのフラグ
    static T *m_instance;               // インスタンスへのポインタ
};

template <typename T> std::once_flag Singleton<T>::m_onceFlag;
template <typename T> T *Singleton<T>::m_instance = nullptr;