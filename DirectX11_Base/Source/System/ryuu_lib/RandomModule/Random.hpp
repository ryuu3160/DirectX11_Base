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
	/// <param name="[nSeed]">設定したい整数値</param>
	/// <memo>設定したシード値はメンバ変数nSeedに記録される</memo>
	void SetSeed(unsigned int nSeed);

	/// <summary>
	/// 整数値の乱数を生成する
	/// </summary>
	/// <param name="[nMax]">生成する乱数の最大値</param>
	/// <param name="[bIncludeZero]">生成する乱数の最小値が0かを指定(true:0 | false:1)</param>
	/// <returns>生成した乱数をint型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	int GetInteger(int nMax, bool bIncludeZero);

	/// <summary>
	/// 指定範囲で整数値の乱数を生成する
	/// </summary>
	/// <param name="[nMax]">生成する乱数の最大値</param>
	/// <param name="[nMin]">生成する乱数の最小値</param>
	/// <returns>生成した乱数をint型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	int GetIntegerRange(int nMax, int nMin);

	/// <summary>
	/// 小数値の乱数を生成する
	/// </summary>
	/// <param name="[nMax]">生成する乱数の最大整数値</param>
	/// <param name="[nPointPos]">生成する乱数の小数点位置 | 例：小数点第1位 → 1 | 第2位 → 2</param>
	/// <param name="[bIncludeZero]">生成する乱数の最小値が0かを指定(true:0 | false:1)</param>
	/// <returns>生成した乱数をfloat型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	float GetDecimal(int nMax, int nPointPos, bool bIncludeZero);

	/// <summary>
	/// 指定範囲で小数点値の乱数を生成する
	/// </summary>
	/// <param name="[fMax]">生成する乱数の最大値</param>
	/// <param name="[fMin]">生成する乱数の最小値</param>
	/// <param name="[nPointPos]">生成する乱数の小数点位置 | 例：小数点第1位 → 1 | 第2位 → 2</param>
	/// <returns>生成した乱数をfloat型で返す</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	float GetDecimalRange(float fMax, float fMin, int fPointPos);

	/// <summary>
	/// 与えた複数の文字列からランダムで1つを返す
	/// </summary>
	/// <param name="[...args]">複数の引数を指定可能、文字列を与えられることを想定している</param>
	/// <returns>std::string型</returns>
	/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
	/// <memo>引数に文字列以外を渡すとエラーになる</memo>
	template<typename ...A>
	std::string Choice(A... args);

	/// <summary>
	/// 複数のデータが格納されたvectorからランダムで1つを返す
	/// </summary>
	/// <param name="[aray]">vectorのインスタンスへの参照</param>
	/// <returns>vectorの型と同じ型</returns>
	template<typename T>
	T Choice(const std::vector<T>& aray);

	/// <summary>
	/// 複数のデータが格納されたlistからランダムで1つを返す
	/// </summary>
	/// <param name="[aray]">listのインスタンスへの参照</param>
	/// <returns>listの型と同じ型</returns>
	template<typename T>
	T Choice(const std::list<T>& aray);

	/// <summary>
	/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
	/// <para>要素の重複はない</para>
	/// </summary>
	/// <param name="[aray]">vectorのインスタンスへの参照</param>
	/// <param name="[num]">取得したい要素の個数</param>
	/// <returns>vectorクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
	template<typename T>
	std::vector<T> Sample(const std::vector<T> &aray, int num);

	/// <summary>
	/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
	/// <para>要素の重複はない</para>
	/// </summary>
	/// <param name="[aray]">listのインスタンスへの参照</param>
	/// <param name="[num]">取得したい要素の個数</param>
	/// <returns>listクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
	template<typename T>
	std::list<T> Sample(const std::list<T>& aray, int num);

	/// <summary>
	/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
	/// <para>要素の重複あり</para>
	/// </summary>
	/// <param name="[aray]">vectorのインスタンスへの参照</param>
	/// <param name="[k]">取得する要素数</param>
	/// <returns>vectorクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
	template<typename T>
	std::vector<T> Choices(const std::vector<T>& aray, int k);

	/// <summary>
	/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
	/// <para>要素の重複あり</para>
	/// </summary>
	/// <param name="[aray]">listのインスタンスへの参照</param>
	/// <param name="[k]">取得する要素数</param>
	/// <returns>listクラス</returns>
	/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
	template<typename T>
	std::list<T> Choices(const std::list<T>& aray, int k);

private:
	std::mt19937 m_mt;//メルセンヌツイスター
	std::uniform_real_distribution<float> m_dist;//一様分布生成器
};




/// <summary>
/// 与えた複数の文字列からランダムで1つを返す
/// </summary>
/// <param name="[...args]">複数の引数を指定可能、文字列を与えられることを想定している</param>
/// <returns>std::string型</returns>
/// <memo>シード値は必ず設定されているので、この関数が失敗することは無い</memo>
/// <memo>引数に文字列以外を渡すとエラーになる</memo>
template<typename ...A>
std::string Random::Choice(A... args)
{
	int nRandom;
	int nCount = 0;
	int i;
	std::string strReturn;

	//引数のカウント
	for (std::string ss : std::initializer_list<std::string>{args... })
	{
		nCount++;
	}

	for (std::string ss : std::initializer_list<std::string>{args... })
	{
		if (ss.size() >= ss.max_size())
		{
			strReturn = "error";
			return strReturn;
		}
	}

	//返す文字列の番号決
	nRandom = static_cast<int>(m_dist(m_mt)) % nCount + 1;

	i = 1;//カウンタ

	for (std::string ss : std::initializer_list<std::string>{args...})
	{
		if (nRandom == i)
		{
			strReturn = ss;
			return strReturn;
		}
		i++;
	}

	return strReturn;
}

/// <summary>
/// 複数のデータが格納されたvectorからランダムで1つを返す
/// </summary>
/// <param name="[aray]">vectorのインスタンスへの参照</param>
/// <returns>vectorの型と同じ型</returns>
template<typename T>
T Random::Choice(const std::vector<T>& aray)
{
	T put;
	int nRandom;
	int nCount = 0;

	//要素がない場合のエラー
	if (aray.empty())
	{
		put = -1;
	}

	//返す文字列の番号決め
	nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();

	put = aray[nRandom];

	return put;
}

/// <summary>
/// 複数のデータが格納されたlistからランダムで1つを返す
/// </summary>
/// <param name="[aray]">listのインスタンスへの参照</param>
/// <returns>listの型と同じ型</returns>
template<typename T>
T Random::Choice(const std::list<T>& aray)
{
	T put;
	int nRandom;
	int nCount = 0;
	auto itr = aray.begin();

	//要素がない場合のエラー
	if (aray.empty())
	{
		put = -1;
	}

	//返す文字列の番号決め
	nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();

	for (int i = 0; i < nRandom; i++)
	{
		itr++;
	}
	put = *itr;

	return put;
}

/// <summary>
/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
/// <para>要素の重複はない</para>
/// </summary>
/// <param name="[aray]">vectorのインスタンスへの参照</param>
/// <param name="[num]">取得したい要素の個数</param>
/// <returns>vectorクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
template<typename T>
std::vector<T> Random::Sample(const std::vector<T> &aray, int num)
{
	std::vector<T> sample;
	std::vector<int> Already;
	int nRandom;	//プッシュするデータの要素番号
	int nPushCount = 0;
	bool bPush = true;

	//要素数が配列のサイズよりも大きい場合のエラー
	if (aray.size() < num)
	{
		OutputDebugStringA("error : Too many elements to retrieve");
		sample.clear();
		return sample;
	}

	//プッシュ処理
	while (nPushCount < num)
	{
		//乱数生成
		nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();

		//重複排除
		for (int i = 0; i < Already.size(); i++)
		{
			if (Already[i] == nRandom)
			{
				bPush = false;
				break;
			}
			else
			{
				bPush = true;
			}
		}

		//値の挿入
		if (bPush)
		{
			sample.push_back(aray[nRandom]);
			Already.push_back(nRandom);
			nPushCount++;
		}
	}

	return sample;
}

/// <summary>
/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
/// <para>要素の重複はない</para>
/// </summary>
/// <param name="[aray]">listのインスタンスへの参照</param>
/// <param name="[num]">取得したい要素の個数</param>
/// <returns>listクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることは出来ない</memo>
template<typename T>
std::list<T> Random::Sample(const std::list<T>& aray, int num)
{
	std::list<T> sample;
	std::vector<int> Already;
	int nRandom;	//プッシュするデータの要素番号
	int nPushCount = 0;
	bool bPush = true;

	//要素数が配列のサイズよりも大きい場合のエラー
	if (aray.size() < num)
	{
		OutputDebugStringA("error : Too many elements to retrieve");
		sample.clear();
		return sample;
	}

	//プッシュ処理
	while (nPushCount < num)
	{
		//乱数生成
		nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();
		
		//重複排除
		for (int i = 0; i < Already.size(); i++)
		{
			if (Already[i] == nRandom)
			{
				bPush = false;
				break;
			}
			else
			{
				bPush = true;
			}
		}

		//値の挿入
		if (bPush)
		{
			auto itr = aray.begin();
			for (int i = 0; i < nRandom; i++)
			{
				itr++;
			}
			sample.push_back(*itr);
			Already.push_back(nRandom);
			nPushCount++;
		}
	}

	return sample;
}

/// <summary>
/// <para>引数で与えたvectorのデータからランダムに複数の要素を選択してvectorとして返す</para>
/// <para>要素の重複あり</para>
/// </summary>
/// <param name="[aray]">vectorのインスタンスへの参照</param>
/// <param name="[k]">取得する要素数</param>
/// <returns>vectorクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
template<typename T>
std::vector<T> Random::Choices(const std::vector<T>& aray, int k)
{
	std::vector<T> choices;
	int nRandom;	//プッシュするデータの要素番号

	//プッシュ処理
	for (int i = 0;i < k;i++)
	{
		//乱数生成
		nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();
		
		//プッシュ
		choices.push_back(aray[nRandom]);
	}

	return choices;
}

/// <summary>
/// <para>引数で与えたlistのデータからランダムに複数の要素を選択してlistとして返す</para>
/// <para>要素の重複あり</para>
/// </summary>
/// <param name="[aray]">listのインスタンスへの参照</param>
/// <param name="[k]">取得する要素数</param>
/// <returns>listクラス</returns>
/// <memo>取得する要素数は元のvectorの要素数より大きくすることが出来る</memo>
template<typename T>
std::list<T> Random::Choices(const std::list<T>& aray, int k)
{
	std::list<T> choices;
	int nRandom;	//プッシュするデータの要素番号

	//プッシュ処理
	for (int i = 0; i < k; i++)
	{
		//先頭イテレーター取得
		auto itr = aray.begin();
		//乱数生成
		nRandom = static_cast<int>(m_dist(m_mt)) % aray.size();

		for (int j = 0; j < nRandom; j++)
		{
			itr++;
		}
		//プッシュ
		choices.push_back(*itr);
	}
	return choices;
}
