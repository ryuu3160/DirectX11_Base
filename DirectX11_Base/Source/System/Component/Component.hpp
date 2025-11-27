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
#include "System/Object/Object.hpp"
// ==============================
// 前方宣言
// ==============================
class GameObject;

/// <summary>
/// Componentクラス
/// </summary>
class Component : public Object
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
		// データの読み書きを実行(POD型のみ対応)
		template<typename T, typename std::enable_if<std::is_trivial<T>::value && std::is_standard_layout<T>::value>::type * = nullptr>
		void Access(T *ptr)
		{
			// 書き込みか読み込みかを判定
			if (m_IsRead)
				Read<T>(ptr);
			else
				Write<T>(ptr);
		}

		template<typename T, typename std::enable_if<std::is_same<T,std::string>::value>::type * = nullptr>
		void Access(T *ptr)
		{
			// 書き込みか読み込みかを判定
			if(m_IsRead)
				ReadString(ptr);
			else
				WriteString(ptr);
		}

		/// <summary>
		/// 現在の書き込みサイズを取得します。
		/// </summary>
		/// <returns>現在の書き込みサイズを表す整数値。</returns>
		inline int GetWriteSize() const noexcept { return m_cur; }

		/// <summary>
		/// データへのポインタを取得します。
		/// </summary>
		/// <returns>データを指す char 型ポインタ。</returns>
		char *GetData() const noexcept { return m_ptr; }
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

		void ReadString(std::string *ptr)
		{
			// サイズを取得してメモリをコピー
			size_t len = 0;
			int size = sizeof(len);
			memcpy(&len, m_ptr, size);
			m_ptr += size;
			ptr->resize(len);
			memcpy(ptr->data(), m_ptr, len);
			m_ptr += len;
		}
		void WriteString(std::string *ptr)
		{
			size_t len = ptr->size();
			int size = sizeof(len);
			// 確保済みのサイズより大きくなりそうであれば再確保
			if (m_nSize < m_cur + len + size)
			{
				char *work = m_ptr;
				m_ptr = new char[m_nSize <<= 1];
				memcpy(m_ptr, work, m_cur);
				delete[] work;
			}
			memcpy(m_ptr + m_cur, &len,size);
			m_cur += size;
			memcpy(m_ptr + m_cur, ptr->data(), len);
			m_cur += static_cast<int>(len);
		}
	private:
		bool m_IsRead;	// 読み込みか書き込みかを判定するフラグ
		char *m_ptr;	// データのポインタ
		int m_nSize;	// 確保済みのサイズ
		int m_cur;		// 現在の書き込み位置
	};

public:
	Component(_In_ std::string In_Name);
	virtual  ~Component();
	virtual void OnEnable() noexcept override {};
	virtual void Init() noexcept override = 0;
	virtual void Update(_In_ float In_Tick) noexcept override;
	virtual void LateUpdate(_In_ float In_Tick) noexcept override;
	virtual void FixedUpdate(_In_ double In_FixedTick) noexcept override;

	virtual void ReadWrite(_In_ DataAccessor *In_Data);

	GameObject *GetGameObject() const noexcept { return m_pTransform; }

	bool IsDestroyed() const noexcept { return m_IsDestroyed; }

	void DestroySelf() noexcept;

protected:

#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif // _DEBUG

protected:
	GameObject *m_pTransform;
	std::string m_Name;
private:
	bool m_IsDestroyed = false;
};
