/*+===================================================================
	File: SceneBase.hpp
	Summary: シーンのベースクラス
	Author: AT13C 01 青木雄一郎
	Date: 2025/6/14 Sat AM 04:54:50 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//  undefine
// ==============================
#undef GetObject

/// <summary>
/// SceneBaseクラス
/// </summary>
class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase();

	static void Initialize() noexcept;

	void RootUpdate() noexcept;
	void RootDraw() noexcept;

	// サブシーン操作
	template <class T = SceneBase>
	T* AddSubScene();
	void RemoveSubScene();

	// オブジェクト操作
	template <class T = GameObject>
	T *CreateObject(const char *In_szName);
	
	template <class T = GameObject>
	T *GetObject(const char *In_szName);


	// 継承先で使用する関数
	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	SceneBase *m_pParent;		// 親シーン
	SceneBase *m_pSubScene;		// サブシーンへのポインタ

private:
	std::string m_strName;
};

template<class T>
inline T *SceneBase::AddSubScene()
{
	return nullptr;
}
