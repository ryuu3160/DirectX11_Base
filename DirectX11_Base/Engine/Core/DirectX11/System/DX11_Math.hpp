/*+===================================================================
	File: DX11_Math.hpp
	Summary: DirectX11用の自作Mathヘッダ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/14 Mon PM 03:00:34 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/Defines.hpp"
// ==============================
//	定数定義
// ==============================

namespace DX11Math
{
	/// <summary>
	/// 円周率（π）を表す定数
	/// </summary>
	inline constexpr float PI = 3.14159265358979323846f;
	inline constexpr float TWO_PI = 6.28318530717958647692f;
	inline constexpr float HALF_PI = 1.57079632679489661923f;
	inline constexpr float EPSILON = 1e-6f;

	inline constexpr DirectX::XMVECTORF32 g_RayEpsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };
	inline constexpr DirectX::XMVECTORF32 g_FltMin = { { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } };
	inline constexpr DirectX::XMVECTORF32 g_FltMax = { { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } };
}

inline DirectX::XMFLOAT2 operator+(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	return DirectX::XMFLOAT2(In_Value1.x + In_Value2.x, In_Value1.y + In_Value2.y);
}
inline void operator+=(_Inout_ DirectX::XMFLOAT2 &Inout_Value, _In_ const DirectX::XMFLOAT2 &In_Value)
{
	Inout_Value.x += In_Value.x;
	Inout_Value.y += In_Value.y;
}
inline DirectX::XMFLOAT2 operator-(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	return DirectX::XMFLOAT2(In_Value1.x - In_Value2.x, In_Value1.y - In_Value2.y);
}
inline void operator-=(_Inout_ DirectX::XMFLOAT2 &Inout_Value, _In_ const DirectX::XMFLOAT2 &In_Value)
{
	Inout_Value.x -= In_Value.x;
	Inout_Value.y -= In_Value.y;
}
inline DirectX::XMFLOAT2 operator*(_In_ const DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT2(In_Value.x * In_Float, In_Value.y * In_Float);
}
inline DirectX::XMFLOAT2 operator*(_In_ const float &In_Float, _In_ const DirectX::XMFLOAT2 &In_Value)
{
	return DirectX::XMFLOAT2(In_Value.x * In_Float, In_Value.y * In_Float);
}
inline DirectX::XMFLOAT2 operator*(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	return DirectX::XMFLOAT2(In_Value1.x * In_Value2.x, In_Value1.y * In_Value2.y);
}
inline void operator*=(_In_ DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
}
inline void operator*=(_In_ const float &In_Float, _Inout_ DirectX::XMFLOAT2 &In_Value)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
}
inline void operator*=(_Inout_ DirectX::XMFLOAT2 &In_Value, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	In_Value.x *= In_Value2.x;
	In_Value.y *= In_Value2.y;
}
inline DirectX::XMFLOAT2 operator/(_In_ const DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT2(In_Value.x / In_Float, In_Value.y / In_Float);
}
inline DirectX::XMFLOAT2 operator/(_In_ const float &In_Float, _In_ const DirectX::XMFLOAT2 &In_Value)
{
	return DirectX::XMFLOAT2(In_Value.x / In_Float, In_Value.y / In_Float);
}
inline DirectX::XMFLOAT2 operator/(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	return DirectX::XMFLOAT2(In_Value1.x / In_Value2.x, In_Value1.y / In_Value2.y);
}
inline void operator/=(_In_ DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
{
	In_Value.x /= In_Float;
	In_Value.y /= In_Float;
}
inline void operator/=(_In_ DirectX::XMFLOAT2 &In_Value, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	In_Value.x /= In_Value2.x;
	In_Value.y /= In_Value2.y;
}
inline bool operator==(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	if (In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y)
		return true;
	else
		return false;
}
inline bool operator!=(_In_ const DirectX::XMFLOAT2 &In_Value1, _In_ const DirectX::XMFLOAT2 &In_Value2)
{
	if (In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y)
		return false;
	else
		return true;
}

// XMFLOAT3

inline DirectX::XMFLOAT3 operator+(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	return DirectX::XMFLOAT3(In_Value1.x + In_Value2.x,In_Value1.y + In_Value2.y,In_Value1.z + In_Value2.z);
}
inline void operator+=(_Inout_ DirectX::XMFLOAT3 &Inout_Value, _In_ const DirectX::XMFLOAT3 &In_Value)
{
	Inout_Value.x += In_Value.x;
	Inout_Value.y += In_Value.y;
	Inout_Value.z += In_Value.z;
}
inline DirectX::XMFLOAT3 operator-(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	return DirectX::XMFLOAT3(In_Value1.x - In_Value2.x,In_Value1.y - In_Value2.y,In_Value1.z - In_Value2.z);
}
inline void operator-=(_Inout_ DirectX::XMFLOAT3 &Inout_Value, _In_ const DirectX::XMFLOAT3 &In_Value)
{
	Inout_Value.x -= In_Value.x;
	Inout_Value.y -= In_Value.y;
	Inout_Value.z -= In_Value.z;
}
inline DirectX::XMFLOAT3 operator*(_In_ const DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT3(In_Value.x * In_Float,In_Value.y * In_Float,In_Value.z * In_Float);
}
inline DirectX::XMFLOAT3 operator*(_In_ const float &In_Float, _In_ const DirectX::XMFLOAT3 &In_Value)
{
	return DirectX::XMFLOAT3(In_Value.x * In_Float,In_Value.y * In_Float,In_Value.z * In_Float);
}
inline DirectX::XMFLOAT3 operator*(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	return DirectX::XMFLOAT3(In_Value1.x * In_Value2.x,In_Value1.y * In_Value2.y,In_Value1.z * In_Value2.z);
}
inline void operator*=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
	In_Value.z *= In_Float;
}
inline void operator*=(_In_ const float &In_Float, _Inout_ DirectX::XMFLOAT3 &In_Value)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
	In_Value.z *= In_Float;
}
inline void operator*=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	In_Value.x *= In_Value2.x;
	In_Value.y *= In_Value2.y;
	In_Value.z *= In_Value2.z;
}
inline DirectX::XMFLOAT3 operator/(_In_ const DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT3(In_Value.x / In_Float,In_Value.y / In_Float,In_Value.z / In_Float);
}
inline DirectX::XMFLOAT3 operator/(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	return DirectX::XMFLOAT3(In_Value1.x / In_Value2.x,In_Value1.y / In_Value2.y,In_Value1.z / In_Value2.z);
}
inline void operator/=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
{
	In_Value.x /= In_Float;
	In_Value.y /= In_Float;
	In_Value.z /= In_Float;
}
inline void operator/=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	In_Value.x /= In_Value2.x;
	In_Value.y /= In_Value2.y;
	In_Value.z /= In_Value2.z;
}
inline bool operator==(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	if (In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y && In_Value1.z == In_Value2.z)
		return true;
	else
		return false;
}
inline bool operator!=(_In_ const DirectX::XMFLOAT3 &In_Value1, _In_ const DirectX::XMFLOAT3 &In_Value2)
{
	if (In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y && In_Value1.z == In_Value2.z)
		return false;
	else
		return true;
}
inline DirectX::XMFLOAT4 operator+(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return DirectX::XMFLOAT4(In_Value1.x + In_Value2.x,In_Value1.y + In_Value2.y,In_Value1.z + In_Value2.z,In_Value1.w + In_Value2.w);
}
inline void operator+=(_Inout_ DirectX::XMFLOAT4 &Inout_Value, _In_ const DirectX::XMFLOAT4 &In_Value)
{
	Inout_Value.x += In_Value.x;
	Inout_Value.y += In_Value.y;
	Inout_Value.z += In_Value.z;
	Inout_Value.w += In_Value.w;
}

// 減算
inline DirectX::XMFLOAT4 operator-(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return DirectX::XMFLOAT4(In_Value1.x - In_Value2.x,In_Value1.y - In_Value2.y,In_Value1.z - In_Value2.z,In_Value1.w - In_Value2.w);
}
inline void operator-=(_Inout_ DirectX::XMFLOAT4 &Inout_Value, _In_ const DirectX::XMFLOAT4 &In_Value)
{
	Inout_Value.x -= In_Value.x;
	Inout_Value.y -= In_Value.y;
	Inout_Value.z -= In_Value.z;
	Inout_Value.w -= In_Value.w;
}

inline DirectX::XMFLOAT4 operator*(_In_ const DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT4(In_Value.x * In_Float,In_Value.y * In_Float,In_Value.z * In_Float,In_Value.w * In_Float);
}
inline DirectX::XMFLOAT4 operator*(_In_ const float &In_Float, _In_ const DirectX::XMFLOAT4 &In_Value)
{
	return DirectX::XMFLOAT4(In_Value.x * In_Float,In_Value.y * In_Float,In_Value.z * In_Float,In_Value.w * In_Float);
}
inline DirectX::XMFLOAT4 operator*(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return DirectX::XMFLOAT4(In_Value1.x * In_Value2.x,In_Value1.y * In_Value2.y,In_Value1.z * In_Value2.z,In_Value1.w * In_Value2.w);
}
inline void operator*=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
	In_Value.z *= In_Float;
	In_Value.w *= In_Float;
}
inline void operator*=(_In_ const float &In_Float, _Inout_ DirectX::XMFLOAT4 &In_Value)
{
	In_Value.x *= In_Float;
	In_Value.y *= In_Float;
	In_Value.z *= In_Float;
	In_Value.w *= In_Float;
}
inline void operator*=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	In_Value.x *= In_Value2.x;
	In_Value.y *= In_Value2.y;
	In_Value.z *= In_Value2.z;
	In_Value.w *= In_Value2.w;
}

inline DirectX::XMFLOAT4 operator/(_In_ const DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMFLOAT4(In_Value.x / In_Float,In_Value.y / In_Float,In_Value.z / In_Float,In_Value.w / In_Float);
}
inline DirectX::XMFLOAT4 operator/(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return DirectX::XMFLOAT4(In_Value1.x / In_Value2.x,In_Value1.y / In_Value2.y,In_Value1.z / In_Value2.z,In_Value1.w / In_Value2.w);
}
inline void operator/=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
{
	In_Value.x /= In_Float;
	In_Value.y /= In_Float;
	In_Value.z /= In_Float;
	In_Value.w /= In_Float;
}
inline void operator/=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	In_Value.x /= In_Value2.x;
	In_Value.y /= In_Value2.y;
	In_Value.z /= In_Value2.z;
	In_Value.w /= In_Value2.w;
}
inline bool operator==(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return (In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y &&
		In_Value1.z == In_Value2.z && In_Value1.w == In_Value2.w);
}
inline bool operator!=(_In_ const DirectX::XMFLOAT4 &In_Value1, _In_ const DirectX::XMFLOAT4 &In_Value2)
{
	return !(In_Value1.x == In_Value2.x && In_Value1.y == In_Value2.y &&
		In_Value1.z == In_Value2.z && In_Value1.w == In_Value2.w);
}

// -------------------------------
// XMVECTOR関連関数群
// -------------------------------

inline DirectX::XMVECTOR operator+(_In_ const DirectX::XMVECTOR &In_Value1, _In_ const DirectX::XMVECTOR &In_Value2)
{
	return DirectX::XMVectorAdd(In_Value1, In_Value2);
}

inline DirectX::XMVECTOR operator-(_In_ const DirectX::XMVECTOR &In_Value1, _In_ const DirectX::XMVECTOR &In_Value2)
{
	return DirectX::XMVectorSubtract(In_Value1, In_Value2);
}

inline DirectX::XMVECTOR operator-(_In_ const DirectX::XMVECTOR &In_Value)
{
	return DirectX::XMVectorNegate(In_Value);
}

inline DirectX::XMVECTOR operator*(_In_ const DirectX::XMVECTOR &In_Value, _In_ const float &In_Float)
{
	return DirectX::XMVectorScale(In_Value, In_Float);
}

inline DirectX::XMVECTOR operator*(_In_ const float &In_Float, _In_ const DirectX::XMVECTOR &In_Value)
{
	return DirectX::XMVectorScale(In_Value, In_Float);
}

inline DirectX::XMVECTOR operator*(_In_ const DirectX::XMVECTOR &In_Value1, _In_ const DirectX::XMVECTOR &In_Value2)
{
	return DirectX::XMVectorMultiply(In_Value1, In_Value2);
}

inline DirectX::XMVECTOR operator/(_In_ const DirectX::XMVECTOR &In_Value1, _In_ const DirectX::XMVECTOR &In_Value2)
{
	return DirectX::XMVectorDivide(In_Value1, In_Value2);
}

namespace DX11Math
{
	/// <summary>
	/// 与えられた値をセンチメートル基準に変換します
	/// </summary>
	/// <param name="[In_Value]">値</param>
	/// <returns>cm基準に変換された値</returns>
	static constexpr float CMeter(_In_ float In_Value) { return In_Value * 0.01f; }

	/// <summary>
	/// 2Dベクトルの各成分をセンチメートル基準に変換します
	/// </summary>
	/// <param name="In_Value">変換するDirectX::XMFLOAT2型の2Dベクトル</param>
	/// <returns>各成分がcm基準に変換された値</returns>
	static inline DirectX::XMFLOAT2 CMeter(_In_ DirectX::XMFLOAT2 In_Value)
	{
		return In_Value * 0.01f;
	}

	static inline DirectX::XMFLOAT3 CMeter(_In_ DirectX::XMFLOAT3 In_Value)
	{
		return In_Value * 0.01f;
	}

	/// <summary>
	/// 2つの範囲が重なっているかどうかを判定します
	/// </summary>
	/// <param name="In_MinA">範囲Aの最小値</param>
	/// <param name="In_MaxA">範囲Aの最大値</param>
	/// <param name="In_MinB">範囲Bの最小値</param>
	/// <param name="In_MaxB">範囲Bの最大値</param>
	/// <returns>範囲が重なっている場合はtrue、それ以外の場合はfalse</returns>
	static inline bool IsOverlap(_In_ float In_MinA, _In_ float In_MaxA, _In_ float In_MinB, _In_ float In_MaxB)
	{
		return !((In_MaxA < In_MinB) || (In_MaxB < In_MinA));
	}

	/// <summary>
	/// ラジアン変換
	/// </summary>
	/// <typeparam name="[T]">変換する型</typeparam>
	/// <param name="[Degree]">メジアン角</param>
	/// <returns>ラジアン角</returns>
	static inline float ToRad(_In_ float In_Degree) { return In_Degree * (PI / 180.0f); }

	/// <summary>
	/// メジアン変換
	/// </summary>
	/// <typeparam name="[T]">変換する型</typeparam>
	/// <param name="[Rad]">ラジアン角</param>
	/// <returns>メジアン角</returns>
	static inline float ToDeg(_In_ float In_Rad) { return In_Rad * (180.0f / PI); }

	/// <summary>
	/// 2次元ベクトルの各成分を度からラジアンに変換します
	/// </summary>
	/// <param name="[In_Degree]">度単位の2次元ベクトル</param>
	/// <returns>各成分がラジアン単位に変換された2次元ベクトル</returns>
	static inline DirectX::XMFLOAT2 ToRad(_In_ DirectX::XMFLOAT2 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位の2次元ベクトルを度単位に変換します
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位の2次元ベクトル</param>
	/// <returns>度単位に変換された2次元ベクトル</returns>
	static inline DirectX::XMFLOAT2 ToDeg(_In_ DirectX::XMFLOAT2 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}

	/// <summary>
	/// 度単位の3次元ベクトルをラジアン単位に変換します
	/// </summary>
	/// <param name="[In_Degree]">度単位の各成分を持つDirectX::XMFLOAT3型の入力ベクトル</param>
	/// <returns>各成分がラジアン単位に変換されたDirectX::XMFLOAT3型のベクトル</returns>
	static inline DirectX::XMFLOAT3 ToRad(_In_ DirectX::XMFLOAT3 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位の3次元ベクトルを度単位に変換します
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位のDirectX::XMFLOAT3型ベクトル</param>
	/// <returns>各成分が度単位に変換されたDirectX::XMFLOAT3型ベクトル</returns>
	static inline DirectX::XMFLOAT3 ToDeg(_In_ DirectX::XMFLOAT3 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}

	/// <summary>
	/// 度単位のDirectX::XMFLOAT4ベクトルをラジアン単位に変換します
	/// </summary>
	/// <param name="[In_Degree]">度単位で表されたDirectX::XMFLOAT4ベクトル</param>
	/// <returns>ラジアン単位に変換されたDirectX::XMFLOAT4ベクトル</returns>
	static inline DirectX::XMFLOAT4 ToRad(_In_ DirectX::XMFLOAT4 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位のDirectX::XMFLOAT4ベクトルを度単位に変換します
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位のDirectX::XMFLOAT4ベクトル</param>
	/// <returns>度単位に変換されたDirectX::XMFLOAT4ベクトル</returns>
	static inline DirectX::XMFLOAT4 ToDeg(_In_ DirectX::XMFLOAT4 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}

	/// <summary>
	/// 値がゼロに近い場合は0.0fを返し、そうでない場合は元の値を返します
	/// </summary>
	/// <param name="[In_Value]">評価する浮動小数点数値</param>
	/// <returns>値の絶対値がEPSILONより小さい場合は0.0f、そうでない場合は元の値</returns>
	static inline float NearZero(_In_ float In_Value)
	{
		return (std::fabs(In_Value) < EPSILON) ? 0.0f : In_Value;
	}

	/// <summary>
	/// 浮動小数点数の2乗を計算します
	/// </summary>
	/// <param name="[In_Value]">2乗する値</param>
	/// <returns>入力値の2乗。</returns>
	static inline float Squaref(_In_ float In_Value)
	{
		return In_Value * In_Value;
	}

	/// <summary>
	/// 2つの浮動小数点数がほぼ等しいかどうかを判定します
	/// </summary>
	/// <param name="[In_A]">比較する最初の浮動小数点数</param>
	/// <param name="[In_B]">比較する2番目の浮動小数点数</param>
	/// <returns>2つの数値の差の絶対値がEPSILONより小さい場合はtrue、そうでない場合はfalse</returns>
	static inline bool NearEqual(_In_ float In_A, _In_ float In_B)
	{
		return (std::fabs(In_A - In_B) < EPSILON);
	}

	/// <summary>
	/// 2つの3次元ベクトルが近似的に等しいかどうかを判定します
	/// </summary>
	/// <param name="[In_A]">比較する最初のベクトル</param>
	/// <param name="[In_B]">比較する2番目のベクトル</param>
	/// <returns>2つのベクトルのすべての成分(x、y、z)が近似的に等しい場合はtrue、それ以外の場合はfalse</returns>
	static inline bool NearEqual(_In_ const DirectX::XMFLOAT3 &In_A, _In_ const DirectX::XMFLOAT3 &In_B)
	{
		return (NearEqual(In_A.x, In_B.x) && NearEqual(In_A.y, In_B.y) && NearEqual(In_A.z, In_B.z));
	}

	/// <summary>
	/// 値を指定した範囲[In_Low, In_High]に制限します
	/// </summary>
	/// <param name="[In_Value]">クランプする入力値</param>
	/// <param name="[In_Low]">許容される下限(In_Valueがこの値より小さい場合、In_Lowが返されます)</param>
	/// <param name="[In_High]">許容される上限(In_Valueがこの値より大きい場合、In_High が返されます)</param>
	/// <returns>v をIn_LowとIn_Highの範囲内に制限した結果のfloat値(両端を含む)</returns>
	inline float Clamp(_In_ float In_Value, _In_ float In_Low, _In_ float In_High)
	{
		if (In_Value < In_Low) return In_Low;
		if (In_Value > In_High) return In_High;
		return In_Value;
	}

	/// <summary>
	/// 2次元ベクトルの各成分を指定された範囲内に制限します
	/// </summary>
	/// <param name="[In_Value]">制限する入力ベクトル</param>
	/// <param name="[In_Low]">各成分の最小値を定義するベクトル</param>
	/// <param name="[In_High]">各成分の最大値を定義するベクトル</param>
	/// <returns>各成分が対応する最小値と最大値の範囲内に制限された新しいベクトル</returns>
	inline DirectX::XMFLOAT2 Clamp(_In_ DirectX::XMFLOAT2 In_Value, _In_ DirectX::XMFLOAT2 In_Low, _In_ DirectX::XMFLOAT2 In_High)
	{
		DirectX::XMFLOAT2 result;
		result.x = Clamp(In_Value.x, In_Low.x, In_High.x);
		result.y = Clamp(In_Value.y, In_Low.y, In_High.y);
		return result;
	}

	/// <summary>
	/// 3次元ベクトルの各成分を指定された範囲内に制限します
	/// </summary>
	/// <param name="[In_Value]">制限する入力ベクトル</param>
	/// <param name="[In_Low]">各成分の最小値を表すベクトル</param>
	/// <param name="[In_High]">各成分の最大値を表すベクトル</param>
	/// <returns>各成分が対応する最小値と最大値の範囲内に制限されたベクトル</returns>
	inline DirectX::XMFLOAT3 Clamp(_In_ DirectX::XMFLOAT3 In_Value, _In_ DirectX::XMFLOAT3 In_Low, _In_ DirectX::XMFLOAT3 In_High)
	{
		DirectX::XMFLOAT3 result;
		result.x = Clamp(In_Value.x, In_Low.x, In_High.x);
		result.y = Clamp(In_Value.y, In_Low.y, In_High.y);
		result.z = Clamp(In_Value.z, In_Low.z, In_High.z);
		return result;
	}

	/// <summary>
	/// 2Dベクトルを正規化します
	/// </summary>
	/// <param name="[In_Vec]">正規化する入力ベクトル</param>
	/// <returns>正規化されたベクトル</returns>
	inline DirectX::XMFLOAT2 Normalize(_In_ const DirectX::XMFLOAT2 &In_Vec)
	{
		DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&In_Vec);
		vec = DirectX::XMVector2Normalize(vec);
		DirectX::XMFLOAT2 result;
		DirectX::XMStoreFloat2(&result, vec);
		return result;
	}

	/// <summary>
	/// 3Dベクトルを正規化します
	/// </summary>
	/// <param name="[In_Vec]">正規化する入力ベクトル</param>
	/// <returns>正規化されたベクトル</returns>
	inline DirectX::XMFLOAT3 Normalize(_In_ const DirectX::XMFLOAT3 &In_Vec)
	{
		DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&In_Vec);
		vec = DirectX::XMVector3Normalize(vec);
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, vec);
		return result;
	}

	/// <summary>
	/// 4次元ベクトルを正規化します
	/// </summary>
	/// <param name="[In_Vec]">正規化する入力ベクトル</param>
	/// <returns>正規化されたベクトル</returns>
	inline DirectX::XMFLOAT4 Normalize(_In_ const DirectX::XMFLOAT4 &In_Vec)
	{
		DirectX::XMVECTOR vec = DirectX::XMLoadFloat4(&In_Vec);
		vec = DirectX::XMVector4Normalize(vec);
		DirectX::XMFLOAT4 result;
		DirectX::XMStoreFloat4(&result, vec);
		return result;
	}

	/// <summary>
	/// クォータニオンをロール・ピッチ・ヨー(オイラー角)に変換します
	/// </summary>
	/// <param name="[In_Quat]">変換対象のDirectX::XMFLOAT4型クォータニオン</param>
	/// <returns>変換されたDirectX::XMFLOAT3型のロール・ピッチ・ヨー(単位はRadian)</returns>
	static inline DirectX::XMFLOAT3 QuaternionToRollPitchYaw(_In_ const DirectX::XMFLOAT4 &In_Quat)
	{
		// クォータニオンの成分
		float x = In_Quat.x;
		float y = In_Quat.y;
		float z = In_Quat.z;
		float w = In_Quat.w;

		// 正規化
		float mag = std::sqrt(x * x + y * y + z * z + w * w);
		if(mag > EPSILON)
		{
			x /= mag; y /= mag; z /= mag; w /= mag;
		}

		// DirectX標準のPitch(X)→Yaw(Y)→Roll(Z)順序の変換式
		float sinp = 2.0f * (w * x - y * z);
		float pitch;

		// ジンバルロックチェック
		if(std::fabs(sinp) >= 1.0f - EPSILON)
		{
			pitch = std::copysign(DirectX::XM_PIDIV2, sinp);
			float m20 = 2.0f * (x * z + w * y);
			float m00 = 1.0f - 2.0f * (y * y + z * z);
			float yaw = std::atan2(-m20, m00);

			// が-πに近い場合、πに変換(または 0 に近づける)
			if(yaw < -DirectX::XM_PI + 0.01f)
			{
				yaw += DirectX::XM_2PI; // -π → π
			}
			float roll = 0.0f; // ジンバルロック時はRollを0に固定
			return DirectX::XMFLOAT3(pitch, yaw, roll);
		}
		else
		{
			pitch = std::asin(sinp);
			float yaw = std::atan2(2.0f * (w * y + x * z), 1.0f - 2.0f * (x * x + y * y));
			float roll = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (x * x + z * z));
			return DirectX::XMFLOAT3(pitch, yaw, roll);
		}
	}
	/// <summary>
	/// クォータニオンをロール・ピッチ・ヨー（オイラー角）に変換します
	/// </summary>
	/// <param name="[In_Quat]">変換対象のDirectX::XMFLOAT4型クォータニオン</param>
	/// <returns>変換されたDirectX::XMFLOAT3型のロール・ピッチ・ヨー(単位はDegree)</returns>
	static inline DirectX::XMFLOAT3 QuaternionToRollPitchYawDegrees(_In_ const DirectX::XMFLOAT4 &In_Quat)
	{
		auto RallPitchYawRad = QuaternionToRollPitchYaw(In_Quat);
		return ToDeg(RallPitchYawRad);
	}

	/// <summary>
	/// クォータニオン同士の乗算を行います
	/// </summary>
	/// <param name="[In_Q1]">掛ける値</param>
	/// <param name="[In_Q2]">掛けられる値</param>
	/// <returns>クォータニオン(DirectX::XMFLOAT4型)</returns>
	static inline DirectX::XMFLOAT4 QuaternionMultiply(_In_ const DirectX::XMFLOAT4 &In_Q1, _In_ const DirectX::XMFLOAT4 &In_Q2)
	{
		DirectX::XMFLOAT4 result;
		result.w = In_Q1.w * In_Q2.w - In_Q1.x * In_Q2.x - In_Q1.y * In_Q2.y - In_Q1.z * In_Q2.z;
		result.x = In_Q1.w * In_Q2.x + In_Q1.x * In_Q2.w + In_Q1.y * In_Q2.z - In_Q1.z * In_Q2.y;
		result.y = In_Q1.w * In_Q2.y - In_Q1.x * In_Q2.z + In_Q1.y * In_Q2.w + In_Q1.z * In_Q2.x;
		result.z = In_Q1.w * In_Q2.z + In_Q1.x * In_Q2.y - In_Q1.y * In_Q2.x + In_Q1.z * In_Q2.w;
		return result;
	}

	/// <summary>
	/// 2つの3次元ベクトルの内積を計算します
	/// </summary>
	/// <param name="[In_V1]">1つ目の3次元ベクトル</param>
	/// <param name="[In_V2]">2つ目の3次元ベクトル</param>
	/// <returns>2つのベクトルの内積(スカラー値)</returns>
	static inline float Dot(_In_ const DirectX::XMFLOAT3 &In_V1, _In_ const DirectX::XMFLOAT3 &In_V2)
	{
		return In_V1.x * In_V2.x + In_V1.y * In_V2.y + In_V1.z * In_V2.z;
	}

	/// <summary>
	/// 2つの3次元ベクトルの外積を計算します
	/// </summary>
	/// <param name="[In_V1]">1つ目の3次元ベクトル</param>
	/// <param name="[In_V2]">2つ目の3次元ベクトル</param>
	/// <returns>2つのベクトルの外積を表す3次元ベクトル</returns>
	static inline DirectX::XMFLOAT3 Cross(_In_ const DirectX::XMFLOAT3 &In_V1, _In_ const DirectX::XMFLOAT3 &In_V2)
	{
		return DirectX::XMFLOAT3(
			In_V1.y * In_V2.z - In_V1.z * In_V2.y,
			In_V1.z * In_V2.x - In_V1.x * In_V2.z,
			In_V1.x * In_V2.y - In_V1.y * In_V2.x
		);
	}

	/// <summary>
	/// 3次元ベクトルの長さの二乗を計算します
	/// </summary>
	/// <param name="[In_V]">長さの二乗を計算する3次元ベクトル</param>
	/// <returns>ベクトルの長さの二乗</returns>
	static inline float LengthSquared(_In_ const DirectX::XMFLOAT3 &In_V)
	{
		return In_V.x * In_V.x + In_V.y * In_V.y + In_V.z * In_V.z;
	}

	/// <summary>
	/// 3次元ベクトルの長さを計算します
	/// </summary>
	/// <param name="[In_V]">長さを計算する3次元ベクトル</param>
	/// <returns>ベクトルの長さ(ユークリッドノルム)</returns>
	static inline float Length(_In_ const DirectX::XMFLOAT3 &In_V)
	{
		return std::sqrt(LengthSquared(In_V));
	}

	/// <summary>
	/// ラジアン角度を-πからπの範囲に正規化します
	/// </summary>
	/// <param name="[In_Rad]">正規化するラジアン角度</param>
	/// <returns>-πからπの範囲に正規化されたラジアン角度</returns>
	static inline float NormalizeRadAngle(_In_ float In_Rad)
	{
		while (In_Rad > PI)
			In_Rad -= TWO_PI;
		while (In_Rad < -PI)
			In_Rad += TWO_PI;
		return In_Rad;
	}

	/// <summary>
	/// オイラー角(ラジアン)の各成分を正規化します
	/// </summary>
	/// <param name="[In_RadAngles]">正規化する入力のオイラー角(ラジアン単位)</param>
	/// <returns>各成分が正規化されたオイラー角</returns>
	static inline DirectX::XMFLOAT3 NormalizeEulerRadAngles(_In_ DirectX::XMFLOAT3 In_RadAngles)
	{
		In_RadAngles.x = NormalizeRadAngle(In_RadAngles.x);
		In_RadAngles.y = NormalizeRadAngle(In_RadAngles.y);
		In_RadAngles.z = NormalizeRadAngle(In_RadAngles.z);
		return In_RadAngles;
	}

	/// <summary>
	/// 度数法の角度を-180度から180度の範囲に正規化します
	/// </summary>
	/// <param name="[In_Deg]">正規化する角度(度数法)</param>
	/// <returns>-180度から180度の範囲に正規化された角度</returns>
	static inline float NormalizeDegAngle(_In_ float In_Deg)
	{
		while (In_Deg > 180.0f)
			In_Deg -= 360.0f;
		while (In_Deg < -180.0f)
			In_Deg += 360.0f;
		return In_Deg;
	}

	/// <summary>
	/// オイラー角(度数法)の各成分を正規化します。
	/// </summary>
	/// <param name="[In_DegAngles]">正規化する度数法のオイラー角</param>
	/// <returns>各成分が正規化されたオイラー角</returns>
	static inline DirectX::XMFLOAT3 NormalizeEulerDegAngles(_In_ DirectX::XMFLOAT3 In_DegAngles)
	{
		In_DegAngles.x = NormalizeDegAngle(In_DegAngles.x);
		In_DegAngles.y = NormalizeDegAngle(In_DegAngles.y);
		In_DegAngles.z = NormalizeDegAngle(In_DegAngles.z);
		return In_DegAngles;
	}

	/// <summary>
	/// スクリーン座標をワールド座標に変換します
	/// </summary>
	/// <param name="[In_ScreenPos]">変換するスクリーン座標(POINTS構造体)</param>
	/// <param name="[In_fZ]">変換時に使用するZ値(深度)</param>
	/// <param name="[In_View]">カメラのビュー行列(DirectX::XMFLOAT4X4型)</param>
	/// <param name="[In_Proj]">カメラのプロジェクション行列(DirectX::XMFLOAT4X4型)</param>
	/// <returns>変換後のワールド座標(DirectX::XMVECTOR型)</returns>
	static inline DirectX::XMVECTOR ScreenToWorldPos(_In_ const POINTS &In_ScreenPos, _In_ const float &In_fZ, _In_ const DirectX::XMFLOAT4X4 &In_View, _In_ const DirectX::XMFLOAT4X4 &In_Proj)
	{
		// 各行列の逆行列を算出
		DirectX::XMMATRIX InvView, InvPrj, mVP, InvViewport;
		DirectX::XMFLOAT4X4 VP;
		DirectX::XMFLOAT4X4 View4x4, Prj4x4;
		// カメラのビュー行列とプロジェクション行列の逆行列を算出
		View4x4 = In_View;
		Prj4x4 = In_Proj;
		InvView = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&View4x4));
		InvPrj = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&Prj4x4));

		// ビューポート行列の逆行列を算出
		mVP = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&VP, mVP);
		VP._11 = cx_nWINDOW_WIDTH / 2.0f; VP._22 = -cx_nWINDOW_HEIGHT / 2.0f;
		VP._41 = cx_nWINDOW_WIDTH / 2.0f; VP._42 = cx_nWINDOW_HEIGHT / 2.0f;
		InvViewport = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&VP));

		// 逆変換
		DirectX::XMMATRIX tmp = InvViewport * InvPrj * InvView;
		DirectX::XMVECTOR vPos = DirectX::XMVectorSet(In_ScreenPos.x, In_ScreenPos.y, In_fZ, 0.0f);
		return DirectX::XMVector3TransformCoord(vPos, tmp);
	}

	static inline bool IntersectRayPlane(_In_ DirectX::XMVECTOR In_vRayPos, _In_ DirectX::XMVECTOR In_vRayDir, _In_ DirectX::XMFLOAT3 In_Center, _In_ DirectX::XMFLOAT3 In_Extent, _In_ DirectX::XMFLOAT4 In_Quaternion, _Out_ float &Out_fDistance)
	{
		// --- レイとの交差判定 ---(DirectXの関数の中身を見て真似して実装)
		static const DirectX::XMVECTORU32 SelectY = { { { DirectX::XM_SELECT_0, DirectX::XM_SELECT_1, DirectX::XM_SELECT_0, DirectX::XM_SELECT_0 } } };
		static const DirectX::XMVECTORU32 SelectZ = { { { DirectX::XM_SELECT_0, DirectX::XM_SELECT_0, DirectX::XM_SELECT_1, DirectX::XM_SELECT_0 } } };

		// スプライトの座標とスケール、回転をそのままBoxColliderとして使用
		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&In_Center);
		DirectX::XMVECTOR vExtents = DirectX::XMLoadFloat3(&In_Extent);
		DirectX::XMVECTOR vOrientation = DirectX::XMLoadFloat4(&In_Quaternion);

		// Boxの側面方向を正規化したもの
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(vOrientation);

		// レイの始点をBoxの中心座標からの相対座標に調整する
		DirectX::XMVECTOR TOrigin = DirectX::XMVectorSubtract(vCenter, In_vRayPos);

		// Boxの各軸に対して内積を計算をする
		DirectX::XMVECTOR AxisDotOrigin = DirectX::XMVector3Dot(R.r[0], TOrigin);
		AxisDotOrigin = XMVectorSelect(AxisDotOrigin, DirectX::XMVector3Dot(R.r[1], TOrigin), SelectY);
		AxisDotOrigin = XMVectorSelect(AxisDotOrigin, DirectX::XMVector3Dot(R.r[2], TOrigin), SelectZ);

		// 軸の内積を計算
		DirectX::XMVECTOR AxisDotDirection = DirectX::XMVector3Dot(R.r[0], In_vRayDir);
		AxisDotDirection = XMVectorSelect(AxisDotDirection, DirectX::XMVector3Dot(R.r[1], In_vRayDir), SelectY);
		AxisDotDirection = XMVectorSelect(AxisDotDirection, DirectX::XMVector3Dot(R.r[2], In_vRayDir), SelectZ);

		// if (fabs(AxisDotDirection) <= Epsilon) なら、レイは板とほぼ平行になっている
		DirectX::XMVECTOR IsParallel = DirectX::XMVectorLessOrEqual(DirectX::XMVectorAbs(AxisDotDirection), g_RayEpsilon);

		// 3つの軸すべてに対して、同時に検証する
		DirectX::XMVECTOR InverseAxisDotDirection = DirectX::XMVectorReciprocal(AxisDotDirection);
		DirectX::XMVECTOR t1 = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
		DirectX::XMVECTOR t2 = DirectX::XMVectorMultiply(DirectX::XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

		// 最大のmin(t1,t2)と最小のmax(t1,t2)を計算して板に平行な方向からの結果を使用しない
		DirectX::XMVECTOR t_min = DirectX::XMVectorSelect(DirectX::XMVectorMin(t1, t2), g_FltMin, IsParallel);
		DirectX::XMVECTOR t_max = DirectX::XMVectorSelect(DirectX::XMVectorMax(t1, t2), g_FltMax, IsParallel);

		// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
		// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
		t_min = DirectX::XMVectorMax(t_min, DirectX::XMVectorSplatY(t_min));  // x = max(x,y)
		t_min = DirectX::XMVectorMax(t_min, DirectX::XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
		t_max = DirectX::XMVectorMin(t_max, DirectX::XMVectorSplatY(t_max));  // x = min(x,y)
		t_max = DirectX::XMVectorMin(t_max, DirectX::XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

		// if ( t_min > t_max ) return false;
		DirectX::XMVECTOR NoIntersection = DirectX::XMVectorGreater(DirectX::XMVectorSplatX(t_min), DirectX::XMVectorSplatX(t_max));

		// if ( t_max < 0.0f ) return false;
		NoIntersection = DirectX::XMVectorOrInt(NoIntersection, DirectX::XMVectorLess(DirectX::XMVectorSplatX(t_max), DirectX::XMVectorZero()));

		// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
		DirectX::XMVECTOR ParallelOverlap = DirectX::XMVectorInBounds(AxisDotOrigin, vExtents);
		NoIntersection = DirectX::XMVectorOrInt(NoIntersection, DirectX::XMVectorAndCInt(IsParallel, ParallelOverlap));

		// 最初の要素から4番目の要素を複製する
		DirectX::XMVECTOR C = DirectX::XMVectorSwizzle<
			DirectX::XM_SWIZZLE_X, DirectX::XM_SWIZZLE_Y, DirectX::XM_SWIZZLE_Z, DirectX::XM_SWIZZLE_X>(NoIntersection);

		// 結果
		bool result = DirectX::XMComparisonAnyTrue(DirectX::XMVector4EqualIntR(C, DirectX::XMVectorTrueInt()));

		if (!result)
		{
			DirectX::XMStoreFloat(&Out_fDistance, t_min);
			return true;
		}
		Out_fDistance = 0.0f;
		return false;
	}

	/// <summary>
	/// 3次元空間インデックス用にビットを分離します
	/// </summary>
	/// <param name="[In_Bit]">分離する入力バイト値</param>
	/// <returns>3次元モートン符号化のためにビットが分離された整数値</returns>
	static int BitSeparateFor3D(_In_ BYTE In_Bit)
	{
		int n = In_Bit;
		n = (n | n << 8) & 0x0000f00f;
		n = (n | n << 4) & 0x000c30c3;
		n = (n | n << 2) & 0x00249249;
		return n;
	}
}

using namespace DX11Math;           // DX11Math名前空間を使用
