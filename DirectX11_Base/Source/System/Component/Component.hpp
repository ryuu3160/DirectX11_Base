/*+===================================================================
	File: Component.hpp
	Summary: コンポーネントの基底クラス 佐々木先生のコードを参考に作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:28:25 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/GameObject.hpp"

/// <summary>
/// Componentクラス
/// </summary>
class Component
{
	friend class GameObject;
public:
	class DataAccessor
	{
	public:
		// ptrがnullptrだった場合bool型がfalseとなる(これめっちゃ頭いい)
		DataAccessor(char *ptr) : m_IsRead(ptr), m_nSize(64), m_cur(0)
		{
			// ptrがnullptrであれば書き込み用のメモリを確保
			// それ以外は読み込み用のポインタとして利用
			m_ptr = m_IsRead ? ptr : new char[m_nSize];
		}
		~DataAccessor() { if (!m_IsRead) { delete[] m_ptr; } }
		// データの読み書きを実行
		template<class T> void Access(T *ptr)
		{
			// 書き込みか読み込みかを判定
			if (m_IsRead)
				Read<T>(ptr);
			else
				Write<T>(ptr);
		}
		inline int GetWriteSize() const { return m_cur; }
		char *GetData() { return m_ptr; }
	private:
		// 読み込み
		template<class T> void Read(T *ptr)
		{
			// サイズを取得してメモリをコピー
			int size = sizeof(T);
			memcpy(ptr, m_ptr, size);
			m_ptr += size;
		}
		// 書き込み
		template<class T> void Write(T *ptr)
		{
			int size = sizeof(T);
			// 確保済みのサイズより大きくなりそうであれば再確保
			if (m_nSize < m_cur + size)
			{
				char *work = m_ptr;
				m_ptr = new char[m_nSize <<= 1];
				memcpy(m_ptr, work, m_cur);
				delete[] work;
			}
			memcpy(m_ptr + m_cur, ptr, size);
			m_cur += size;
		}
	private:
		bool m_IsRead;	// 読み込みか書き込みかを判定するフラグ
		char *m_ptr;	// データのポインタ
		int m_nSize;	// 確保済みのサイズ
		int m_cur;		// 現在の書き込み位置
	};

public:
	Component();
	virtual  ~Component() = default;
	virtual void Execute();
	virtual void ReadWrite(_In_ DataAccessor *In_Data);

protected:
	GameObject *m_pTransform;
};
