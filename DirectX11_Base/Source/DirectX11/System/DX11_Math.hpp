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

// ==============================
//	定数定義
// ==============================
namespace Dx11Math
{
	/// <summary>
	/// 円周率（π）を表す定数
	/// </summary>
	const inline constexpr float PI = 3.14159265358979323846f;

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
	inline void operator*=(_In_ DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x *= In_Float;
		In_Value.y *= In_Float;
	}
	inline DirectX::XMFLOAT2 operator/(_In_ const DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
	{
		return DirectX::XMFLOAT2(In_Value.x / In_Float, In_Value.y / In_Float);
	}
	inline void operator/=(_In_ DirectX::XMFLOAT2 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x /= In_Float;
		In_Value.y /= In_Float;
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
	inline void operator*=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x *= In_Float;
		In_Value.y *= In_Float;
		In_Value.z *= In_Float;
	}
	inline DirectX::XMFLOAT3 operator/(_In_ const DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
	{
		return DirectX::XMFLOAT3(In_Value.x / In_Float,In_Value.y / In_Float,In_Value.z / In_Float);
	}
	inline void operator/=(_Inout_ DirectX::XMFLOAT3 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x /= In_Float;
		In_Value.y /= In_Float;
		In_Value.z /= In_Float;
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

	// スカラー倍
	inline DirectX::XMFLOAT4 operator*(_In_ const DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
	{
		return DirectX::XMFLOAT4(In_Value.x * In_Float,In_Value.y * In_Float,In_Value.z * In_Float,In_Value.w * In_Float);
	}
	inline void operator*=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x *= In_Float;
		In_Value.y *= In_Float;
		In_Value.z *= In_Float;
		In_Value.w *= In_Float;
	}

	// スカラー除算
	inline DirectX::XMFLOAT4 operator/(_In_ const DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
	{
		return DirectX::XMFLOAT4(In_Value.x / In_Float,In_Value.y / In_Float,In_Value.z / In_Float,In_Value.w / In_Float);
	}
	inline void operator/=(_Inout_ DirectX::XMFLOAT4 &In_Value, _In_ const float &In_Float)
	{
		In_Value.x /= In_Float;
		In_Value.y /= In_Float;
		In_Value.z /= In_Float;
		In_Value.w /= In_Float;
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

	/// <summary>
	/// 与えられた値をセンチメートル基準に変換します。
	/// </summary>
	/// <param name="[In_Value]">値</param>
	/// <returns>cm基準に変換された値</returns>
	static constexpr float CMeter(_In_ float In_Value) { return In_Value * 0.01f; }

	/// <summary>
	/// 2Dベクトルの各成分をセンチメートル基準に変換します。
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
	static inline float ToDeg(_In_ float In_Rad) { return static_cast<float>(In_Rad * (180.0f / PI)); }

	/// <summary>
	/// 2次元ベクトルの各成分を度からラジアンに変換します。
	/// </summary>
	/// <param name="[In_Degree]">度単位の2次元ベクトル。</param>
	/// <returns>各成分がラジアン単位に変換された2次元ベクトル。</returns>
	static inline DirectX::XMFLOAT2 ToRad(_In_ DirectX::XMFLOAT2 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位の2次元ベクトルを度単位に変換します。
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位の2次元ベクトル。</param>
	/// <returns>度単位に変換された2次元ベクトル。</returns>
	static inline DirectX::XMFLOAT2 ToDeg(_In_ DirectX::XMFLOAT2 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}

	/// <summary>
	/// 度単位の3次元ベクトルをラジアン単位に変換します。
	/// </summary>
	/// <param name="[In_Degree]">度単位の各成分を持つDirectX::XMFLOAT3型の入力ベクトル。</param>
	/// <returns>各成分がラジアン単位に変換されたDirectX::XMFLOAT3型のベクトル。</returns>
	static inline DirectX::XMFLOAT3 ToRad(_In_ DirectX::XMFLOAT3 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位の3次元ベクトルを度単位に変換します。
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位のDirectX::XMFLOAT3型ベクトル。</param>
	/// <returns>各成分が度単位に変換されたDirectX::XMFLOAT3型ベクトル。</returns>
	static inline DirectX::XMFLOAT3 ToDeg(_In_ DirectX::XMFLOAT3 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}

	/// <summary>
	/// 度単位のDirectX::XMFLOAT4ベクトルをラジアン単位に変換します。
	/// </summary>
	/// <param name="[In_Degree]">度単位で表されたDirectX::XMFLOAT4ベクトル。</param>
	/// <returns>ラジアン単位に変換されたDirectX::XMFLOAT4ベクトル。</returns>
	static inline DirectX::XMFLOAT4 ToRad(_In_ DirectX::XMFLOAT4 In_Degree)
	{
		return In_Degree * (PI / 180.0f);
	}

	/// <summary>
	/// ラジアン単位のDirectX::XMFLOAT4ベクトルを度単位に変換します。
	/// </summary>
	/// <param name="[In_Rad]">ラジアン単位のDirectX::XMFLOAT4ベクトル。</param>
	/// <returns>度単位に変換されたDirectX::XMFLOAT4ベクトル。</returns>
	static inline DirectX::XMFLOAT4 ToDeg(_In_ DirectX::XMFLOAT4 In_Rad)
	{
		return In_Rad * (180.0f / PI);
	}
}
