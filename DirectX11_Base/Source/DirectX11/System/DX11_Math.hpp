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

namespace
{
	/// <summary>
	/// 円周率（π）を表す定数
	/// </summary>
	const inline constexpr float PI = 3.14159265358979323846f;

	const inline constexpr DirectX::XMVECTORF32 g_RayEpsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };
	const inline constexpr DirectX::XMVECTORF32 g_FltMin = { { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } };
	const inline constexpr DirectX::XMVECTORF32 g_FltMax = { { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } };
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

/// <summary>
/// クォータニオンをロール・ピッチ・ヨー（オイラー角）に変換します。
/// </summary>
/// <param name="[In_Quat]">変換対象のDirectX::XMFLOAT4型クォータニオン。</param>
/// <returns>変換されたDirectX::XMFLOAT3型のロール・ピッチ・ヨー（オイラー角）。</returns>
static inline DirectX::XMFLOAT3 QuaternionToRollPitchYaw(_In_ const DirectX::XMFLOAT4 &In_Quat)
{
	// 各成分用意
	float ysqr = In_Quat.y * In_Quat.y;

	// ロール（X軸回転）
	float t0 = 2.0f * (In_Quat.w * In_Quat.x + In_Quat.y * In_Quat.z);
	float t1 = 1.0f - 2.0f * (In_Quat.x * In_Quat.x + ysqr);
	float roll = std::atan2(t0, t1);

	// クォータニオンの成分
	float x = In_Quat.x, y = In_Quat.y, z = In_Quat.z, w = In_Quat.w;

	// 回転行列の要素
	float m21 = 2.0f * (x * z + y * w);      // 行列[2][1]
	float m22 = 1.0f - 2.0f * (y * y + z * z); // 行列[2][2]
	float m20 = -2.0f * (x * y - z * w);     // 行列[2][0]

	// ピッチ（Y軸） = atan2(-m20, sqrt(m21*m21 + m22*m22))
	float pitch = std::atan2(-m20, std::sqrt(m21 * m21 + m22 * m22));

	// ヨー（Z軸回転）
	float t3 = 2.0f * (In_Quat.w * In_Quat.z + In_Quat.x * In_Quat.y);
	float t4 = 1.0f - 2.0f * (ysqr + In_Quat.z * In_Quat.z);
	float yaw = std::atan2(t3, t4);

	return DirectX::XMFLOAT3(roll, pitch, yaw);
}

/// <summary>
/// クォータニオン同士の乗算を行います。
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
/// スクリーン座標をワールド座標に変換します。
/// </summary>
/// <param name="[In_ScreenPos]">変換するスクリーン座標（POINTS構造体）。</param>
/// <param name="[In_fZ]">変換時に使用するZ値（深度）。</param>
/// <param name="[In_View]">カメラのビュー行列（DirectX::XMFLOAT4X4型）。</param>
/// <param name="[In_Proj]">カメラのプロジェクション行列（DirectX::XMFLOAT4X4型）。</param>
/// <returns>変換後のワールド座標（DirectX::XMVECTOR型）。</returns>
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
