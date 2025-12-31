/*+===================================================================
	File: Observer.hpp
	Summary: Observerクラス
	Author: AT13C 01 青木雄一郎
	Date: 2025/02/07 13:57 初回作成
				 /09 06:34 関数ポインタを追加する関数を追加
					 06:34 登録されている関数を全て実行する関数を追加
					 06:38 登録されている関数をクリアする関数を追加

	(C) 2025 AT13C 01 青木雄一郎. All rights reserved.
===================================================================+*/
#pragma once

/// <summary>
/// <para>Observerクラス</para>
/// <para>継承して使用する事を想定している</para>
/// </summary>
class Observer
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Observer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Observer();

	/// <summary>
	/// 通知時に呼び出す関数を追加する
	/// </summary>
	/// <param name="[In_Function]">クラスメンバ以外の関数</param>
	/// <param name="[In_strNotifyName]">通知名(指定しない場合はdefault)</param>
	void AppendFunction(std::function<void()> In_Function, std::string In_strNotifyName = "default");

	// XMLでは:を連続で使えないため、これだけDoxygen
	/**
	* @brief 通知時に呼び出す関数を追加する
	* @tparam [_Func] 追加する関数の型
	* @tparam [_Instance] 追加する関数のインスタンスの型
	* @param [In_Function] 追加する、クラスのメンバ関数への右辺値参照
	* @param [In_Instance] 上記クラスのインスタンスへの右辺値参照
	* @details 使用例 : 
						対象クラスのインスタンスが静的オブジェクト(newを使用していない)の場合 : AppendFunction(&Class::Function, &Instance);
						対象クラスのインスタンスが動的オブジェクト(newで確保されている)の場合 : AppendFunction(&Class::Function, Instance);
	*/
	template<typename _Func, typename _Instance>
	void AppendFunction(_Func &&In_Function, _Instance &&In_Instance, std::string In_strNotifyName = "default")
	{
		// 指定の通知名で関数を追加
		// std::bindで関数とインスタンスを束縛して、std::functionに変換して追加
		// std::forwardで右辺値参照をそのまま渡す(コピーを避ける)(完全転送)
		m_Function[In_strNotifyName].push_back(std::bind(std::forward<_Func>(In_Function), std::forward<_Instance>(In_Instance)));
	}

	/// <summary>
	/// 通知を行う
	/// </summary>
	/// <param name="[In_strNotifyName]">通知名</param>
	void Notify(std::string In_strNotifyName = "default");

	/// <summary>
	/// 通知時に呼び出す関数を全て削除する
	/// </summary>
	/// <param name="[In_strNotifyName]">通知名</param>
	void ClearFunction(std::string In_strNotifyName = "default");

	/// <summary>
	/// 通知名、登録されている関数、共に全て削除する
	/// </summary>
	void AllClearFunction();

private:
	std::unordered_map<std::string,std::deque<std::function<void()>>> m_Function;
};
