 /*+===================================================================
	 File: Random.hpp
	 Summary: ランダムモジュールプログラムのヘッダ
	 Author: ryuu3160
	 Date: 2024/03/17 初回作成
		   2025/12/28 MTに完全移行

	 (C) 2024 ryuu3160. All rights reserved.
 ===================================================================+*/
#pragma once

#include <string>
#include <vector>
#include <list>
#include <initializer_list>
#include <random>
#include <Windows.h>
#include <unordered_set>

class Random
{
public:

	/// <summary>
	/// <para>引数なしコンストラクタ</para>
	/// <para>呼び出し時の時間がシード値として記録される</para>
	/// </summary>
	/// <memo>シード値を特に決めない場合はこのコンストラクタを使用する</memo>
	Random();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Random();

	/// <summary>
	/// 呼び出し時の時間を使用してシード値を設定する
	/// </summary>
	/// <memo>設定したシード値はメンバ変数nSeedに記録される</memo>
	void SetSeedTime();

	/// <summary>
	/// 引数を使用してシード値を設定する
	/// </summary>
	/// <param name="[In_Seed]">設定したい整数値</param>
	/// <memo>設定したシード値はメンバ変数nSeedに記録される</memo>
	void SetSeed(_In_ unsigned int In_Seed);

	/// <summary>
	/// 整数値の乱数を生成する
	/// </summary>
	/// <param name="[In_Max]">生成する乱数の最大値</param>
	/// <param name="[In_IsIncludeZero]">生成する乱数の最小値が0かを指定(true:0 | false:1)</param>
	/// <returns>生成した乱数をint型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	int GetInteger(_In_ int In_Max, _In_ bool In_IsIncludeZero);

	/// <summary>
	/// 指定範囲で整数値の乱数を生成する
	/// </summary>
	/// <param name="[nMax]">生成する乱数の最大値</param>
	/// <param name="[nMin]">生成する乱数の最小値</param>
	/// <returns>生成した乱数をint型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	int GetIntegerRange(_In_ int In_Min, _In_ int In_Max);

	/// <summary>
	/// 小数値の乱数を生成する
	/// </summary>
	/// <param name="[In_Max]">生成する乱数の最大整数値</param>
	/// <param name="[In_PointPos]">生成する乱数の小数点位置 | 例：小数点第1位 → 1 | 第2位 → 2</param>
	/// <param name="[In_IsIncludeZero]">生成する乱数の最小値が0かを指定(true:0 | false:1)</param>
	/// <returns>生成した乱数をfloat型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	float GetDecimal(_In_ int In_Max, _In_ int In_PointPos, _In_ bool In_IsIncludeZero);

	/// <summary>
	/// 指定範囲で小数点値の乱数を生成する
	/// </summary>
	/// <param name="[In_Min]">生成する乱数の最小値</param>
	/// <param name="[In_Max]">生成する乱数の最大値</param>
	/// <param name="[In_PointPos]">生成する乱数の小数点位置 | 例：小数点第1位 → 1 | 第2位 → 2</param>
	/// <returns>生成した乱数をfloat型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	float GetDecimalRange(_In_ float In_Min, _In_ float In_Max, _In_ int In_PointPos);

	/// <summary>
	/// 与えた複数の文字列からランダムで1つを返す
	/// </summary>
	/// <param name="[...In_Args]">複数の引数を指定可能、文字列を与えられることを想定している</param>
	/// <returns>std::string型</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	/// <memo>引数に文字列以外を渡すとエラーになる</memo>
	template<typename ...A>
	std::string Choice(_In_ A... In_Args);

	/// <summary>
	/// 複数のデータが格納されたvectorからランダムで1つを返す
	/// </summary>
	/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
	/// <returns>vectorの型と同じ型</returns>
	template<typename T>
	T Choice(_In_ const std::vector<T>& In_Aray);

	/// <summary>
	/// 複数のデータが格納されたlistからランダムで1つを返す
	/// </summary>
	/// <param name="[In_Aray]">listのインスタンスへの参照</param>
	/// <returns>listの型と同じ型</returns>
	template<typename T>
	T Choice(_In_ const std::list<T>& In_Aray);

	/// <summary>
	/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
	/// <para>要素の重複はない</para>
	/// </summary>
	/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
	/// <param name="[In_Num]">取得したい要素の個数</param>
	/// <returns>vectorクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
	template<typename T>
	std::vector<T> Sample(_In_ const std::vector<T> &In_Aray, _In_ int In_Num);

	/// <summary>
	/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
	/// <para>要素の重複はない</para>
	/// </summary>
	/// <param name="[In_Aray]">listのインスタンスへの参照</param>
	/// <param name="[In_Num]">取得したい要素の個数</param>
	/// <returns>listクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
	template<typename T>
	std::list<T> Sample(_In_ const std::list<T>& In_Aray, _In_ int In_Num);

	/// <summary>
	/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
	/// <para>要素の重複あり</para>
	/// </summary>
	/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
	/// <param name="[In_ElementNum]">取得する要素数</param>
	/// <returns>vectorクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
	template<typename T>
	std::vector<T> Choices(_In_ const std::vector<T>& In_Aray, _In_ int In_ElementNum);

	/// <summary>
	/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
	/// <para>要素の重複あり</para>
	/// </summary>
	/// <param name="[In_Aray]">listのインスタンスへの参照</param>
	/// <param name="[In_ElementNum]">取得する要素数</param>
	/// <returns>listクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
	template<typename T>
	std::list<T> Choices(_In_ const std::list<T>& In_Aray, _In_ int In_ElementNum);

private:
	std::mt19937 m_mt;//メルセンヌツイスター
	std::uniform_real_distribution<float> m_DistFloat;//一様分布生成器
	std::uniform_int_distribution<int> m_DistInt;//一様分布生成器
};




/// <summary>
/// 与えた複数の文字列からランダムで1つを返す
/// </summary>
/// <param name="[...In_Args]">複数の引数を指定可能、文字列を与えられることを想定している</param>
/// <returns>std::string型</returns>
/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
/// <memo>引数に文字列以外を渡すとエラーになる</memo>
template<typename ...A>
std::string Random::Choice(_In_ A... In_Args)
{
	int nRandom;
	int nCount = 0;
	int i;
	std::string strReturn;

	//引数のカウント
	for (std::string ss : std::initializer_list<std::string>{In_Args... })
	{
		++nCount;
	}

	for (std::string ss : std::initializer_list<std::string>{In_Args... })
	{
		if (ss.size() >= ss.max_size())
		{
			strReturn = "error";
			return strReturn;
		}
	}

	//返す文字列の番号決
	nRandom = m_DistInt(m_mt) % nCount + 1;

	i = 1;//カウンタ

	for (std::string ss : std::initializer_list<std::string>{In_Args...})
	{
		if (nRandom == i)
		{
			strReturn = ss;
			return strReturn;
		}
		++i;
	}

	return strReturn;
}

/// <summary>
/// 複数のデータが格納されたvectorからランダムで1つを返す
/// </summary>
/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
/// <returns>vectorの型と同じ型</returns>
template<typename T>
T Random::Choice(_In_ const std::vector<T>&In_Aray)
{
	T put;
	int nRandom;
	int nCount = 0;

	//要素がない場合のエラー
	if (In_Aray.empty())
	{
		return T{};//デフォルト値を返す
	}

	//返す文字列の番号決め
	nRandom = m_DistInt(m_mt) % In_Aray.size();

	put = In_Aray[nRandom];

	return put;
}

/// <summary>
/// 複数のデータが格納されたlistからランダムで1つを返す
/// </summary>
/// <param name="[In_Aray]">listのインスタンスへの参照</param>
/// <returns>listの型と同じ型</returns>
template<typename T>
T Random::Choice(_In_ const std::list<T>& In_Aray)
{
	int nRandom;

	//要素がない場合のエラー
	if (In_Aray.empty())
	{
		return T{};//デフォルト値を返す
	}

	//返す文字列の番号決め
	nRandom = m_DistInt(m_mt) % In_Aray.size();

	auto itr = std::next(In_Aray.begin(), nRandom);

	return *itr;
}

/// <summary>
/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
/// <para>要素の重複はない</para>
/// </summary>
/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
/// <param name="[In_Num]">取得したい要素の個数</param>
/// <returns>vectorクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
template<typename T>
std::vector<T> Random::Sample(_In_ const std::vector<T> &In_Aray, _In_ int In_Num)
{
	std::vector<T> sample;
	//要素数が配列のサイズよりも大きい場合のエラー
	if (In_Aray.size() < In_Num)
	{
		OutputDebugStringA("error : Too many elements to retrieve");
		sample.clear();
		return sample;
	}

	std::unordered_set<int> used_indices;
	int nRandom;	//プッシュするデータの要素番号

	while (sample.size() < In_Num)
	{
		nRandom = m_DistInt(m_mt) % In_Aray.size();
		if (used_indices.insert(nRandom).second)
		{
			sample.push_back(In_Aray[nRandom]);
		}
	}
	return sample;
}

/// <summary>
/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
/// <para>要素の重複はない</para>
/// </summary>
/// <param name="[In_Aray]">listのインスタンスへの参照</param>
/// <param name="[In_Num]">取得したい要素の個数</param>
/// <returns>listクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
template<typename T>
std::list<T> Random::Sample(_In_ const std::list<T>& In_Aray, _In_ int In_Num)
{
	std::list<T> sample;
	//要素数が配列のサイズよりも大きい場合のエラー
	if (In_Aray.size() < In_Num)
	{
		OutputDebugStringA("error : Too many elements to retrieve");
		sample.clear();
		return sample;
	}

	std::unordered_set<int> used_indices;
	int nRandom;	//プッシュするデータの要素番号

	while (sample.size() < In_Num)
	{
		nRandom = m_DistInt(m_mt) % In_Aray.size();
		if (used_indices.insert(nRandom).second)
		{
			auto itr = std::next(In_Aray.begin(), nRandom);
			sample.push_back(*itr);
		}
	}
	return sample;
}

/// <summary>
/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
/// <para>要素の重複あり</para>
/// </summary>
/// <param name="[In_Aray]">vectorのインスタンスへの参照</param>
/// <param name="[In_ElementNum]">取得する要素数</param>
/// <returns>vectorクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
template<typename T>
std::vector<T> Random::Choices(_In_ const std::vector<T>& In_Aray, _In_ int In_ElementNum)
{
	std::vector<T> choices;
	int nRandom;	//プッシュするデータの要素番号

	//プッシュ処理
	for (int i = 0;i < In_ElementNum;++i)
	{
		//乱数生成
		nRandom = m_DistInt(m_mt) % In_Aray.size();

		//プッシュ
		choices.push_back(In_Aray[nRandom]);
	}

	return choices;
}

/// <summary>
/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
/// <para>要素の重複あり</para>
/// </summary>
/// <param name="[In_Aray]">listのインスタンスへの参照</param>
/// <param name="[In_ElementNum]">取得する要素数</param>
/// <returns>listクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
template<typename T>
std::list<T> Random::Choices(_In_ const std::list<T>& In_Aray, _In_ int In_ElementNum)
{
	std::list<T> choices;
	int nRandom;	//プッシュするデータの要素番号

	//プッシュ処理
	for (int i = 0; i < In_ElementNum; ++i)
	{
		//乱数生成
		nRandom = m_DistInt(m_mt) % In_Aray.size();

		auto itr = std::next(In_Aray.begin(), nRandom);
		//プッシュ
		choices.push_back(*itr);
	}
	return choices;
}
