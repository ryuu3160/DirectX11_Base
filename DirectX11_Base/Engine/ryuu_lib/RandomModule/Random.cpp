/*+===================================================================
	File: Random.hpp
	Summary: ランダムモジュールプログラムのヘッダ
	Author: ryuu3160
	Date: 2024/03/17 初回作成

	(C) 2024 ryuu3160. All rights reserved.
===================================================================+*/
#include "Random.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string>

Random::Random()
	: m_mt(std::random_device{}()), m_DistFloat(0, RAND_MAX)
{
}

Random::~Random()
{
}

void Random::SetSeedTime()
{
	m_mt.seed(static_cast<unsigned int>(time(NULL)));
}

void Random::SetSeed(_In_ unsigned int In_Seed)
{
	m_mt.seed(In_Seed);
}

int Random::GetInteger(_In_ int In_Max, _In_ bool In_IncludeZero)
{
	int nInZero;

	if (In_IncludeZero)
	{
		nInZero = 0;
		In_Max++;
	}
	else
	{
		nInZero = 1;
	}

	//乱数を最大値で割った余りにnInZeroを足す
	return m_DistInt(m_mt) % In_Max + nInZero;
}

int Random::GetIntegerRange(_In_ int In_Min, _In_ int In_Max)
{
	if (In_Min > In_Max)
		return 0;

	In_Max++;
	In_Max -= In_Min;

	//最大値を+1したものから最小値を引き、それで乱数を割った余りに最小値を足す
	return m_DistInt(m_mt) % In_Max + In_Min;
}

float Random::GetDecimal(_In_ int In_Max, _In_ int In_PointPos, _In_ bool In_IncludeZero)
{
	float fRandom;
	int nSetPointPos;
	int nVal;
	int nInZero;

	//10を、表示したい少数の位だけ累乗した数値
	nSetPointPos = static_cast<int>(pow(10, In_PointPos));
	//最大値の桁を増やす
	nVal = In_Max * nSetPointPos;

	if (In_IncludeZero)
	{
		nInZero = 0;
		nVal++;
	}
	else
	{
		nInZero = 1;
	}

	//乱数を最大値で割った余りにnInZeroを足して、小数点をずらす
	fRandom = static_cast<float>(static_cast<int>(m_DistFloat(m_mt)) % nVal + nInZero);
	fRandom /= nSetPointPos;

	return fRandom;
}

float Random::GetDecimalRange(_In_ float In_Min, _In_ float In_Max, _In_ int In_PointPos)
{
	if(In_Min > In_Max)
		return 0.0f;


	float fRandom;
	int nSetPointPos;
	int nMaxVal;
	int nMinVal;

	//10を、表示したい少数の位だけ累乗した数値
	nSetPointPos = static_cast<int>(pow(10, In_PointPos));

	//最大値、最小値の桁を増やす
	nMaxVal = static_cast<int>(In_Max * nSetPointPos);
	nMinVal = static_cast<int>(In_Min * nSetPointPos);

	//最大値を+1したところから、最小値を引く
	nMaxVal++;
	nMaxVal -= nMinVal;

	// 生成できる乱数の最大値が生成する最大値よりも小さい場合は、最大値を変更
	if (nMaxVal > RAND_MAX)
	{
		std::uniform_real_distribution<float>::param_type SetParam(0, static_cast<float>(nMaxVal));
		m_DistFloat.param(SetParam);
	}

	//乱数を最大値から最小値を引いた値で割った余りに最小値を足して、小数点の位置をずらす
	auto param = m_DistFloat.param(); // 現在の乱数の最大値を保存

	fRandom = static_cast<float>(static_cast<int>(m_DistFloat(m_mt)) % nMaxVal + nMinVal);
	fRandom /= nSetPointPos;

	// 生成できる乱数の最大値を元に戻す
	m_DistFloat.param(param);

	return fRandom;
}

//旧Random.Choiceのコード
#if 0
std::string Random::Choice(std::string ssInput, ...) const
{
	int nRandom;
	int nCount = 0;
	int i;
	std::string ssReturn;

	va_list args, args2, args3;
	va_start(args, ssInput);
	va_copy(args2, args);
	va_copy(args3, args);

	//入力された文字列のカウント
	nCount++;//1つ目の引数のカウント
	while (true)
	{
		if (va_arg(args, int) != NULL)
			nCount++;
		else
			break;
	}
	va_end(args);

	//入力された文字数がstd::stringの上限文字以下かの判定
	for (i = 0; i < nCount - 1; i++)
	{
		if (va_arg(args2, std::string).size() > ssReturn.max_size() || ssInput.size() > ssReturn.max_size())
		{
			ssReturn = "error";
			return ssReturn;
		}
	}
	va_end(args2);

	//返す文字列の番号決め
	nRandom = rand() % nCount + 1;

	//返す文字列を仮格納
	if (nRandom == 1)
		ssReturn = ssInput;
	else
	{
		for (i = 0; i < nRandom - 1; i++)
		{
			ssReturn = va_arg(args3, const char *);
		}
	}
	va_end(args3);

	return ssReturn;
}
#endif
