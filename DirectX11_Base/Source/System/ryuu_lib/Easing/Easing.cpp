/*+===================================================================
	File: Easing.cpp
	Summary: Easing‚Ìƒtƒ@ƒCƒ‹
	Author: ryuu3160
	Date: 2025/1/23 23:04 ‰‰ñì¬
				/24 02:06 Ease InOutŠÖ”’Ç‰Á

	(C) 2025 ryuu3160. All rights reserved.
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Easing.hpp"

namespace OutBounce
{
	constexpr float fN1 = 7.5625f;
	constexpr float fD1 = 2.75f;

	static float OutBounce(float In_fRate)
	{
		if (In_fRate < 1.0f / fD1)
			return fN1 * In_fRate * In_fRate;
		else if (In_fRate < 2.0f / fD1)
			return fN1 * std::powf(In_fRate - 1.5f / fD1, 2.0f) + 0.75f;
		else if (In_fRate < 2.5f / fD1)
			return fN1 * std::powf(In_fRate - 2.25f / fD1, 2.0f) + 0.9375f;
		else
			return fN1 * std::powf(In_fRate - 2.625f / fD1, 2.0f) + 0.984375f;
	}
}

namespace Ease
{
	namespace
	{
		constexpr float fPI = 3.141592654f;
		constexpr float fC1 = 1.70158f;
		constexpr float fC2 = fC1 * 1.525f;
		constexpr float fC3 = fC1 + 1.0f;
		constexpr float fC4 = (2.0f * fPI) / 3.0f;
		constexpr float fC5 = (2.0f * fPI) / 4.5f;
	}

	// ==============================
	//  Eas-In
	// ==============================

	float InSine(DataRate &In_sData)
	{
		return Ease::Lerp(1.0f - std::cosf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InSine(DataRate2 &In_sData)
	{
		return Ease::Lerp(1.0f - std::cosf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InSine(DataRate3 &In_sData)
	{
		return Ease::Lerp(1.0f - std::cosf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InSine(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::cosf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InSine(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::cosf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InSine(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::cosf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuad(DataRate &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuad(DataRate2 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuad(DataRate3 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuad(DataDuration &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuad(DataDuration2 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuad(DataDuration3 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InCubic(DataRate &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InCubic(DataRate2 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InCubic(DataRate3 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InCubic(DataDuration &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InCubic(DataDuration2 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InCubic(DataDuration3 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuart(DataRate &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuart(DataRate2 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuart(DataRate3 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuart(DataDuration &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuart(DataDuration2 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuart(DataDuration3 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuint(DataRate &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuint(DataRate2 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuint(DataRate3 &In_sData)
	{
		return Lerp(std::powf(In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InQuint(DataDuration &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InQuint(DataDuration2 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InQuint(DataDuration3 &In_sData)
	{
		return Lerp(std::powf((In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InExpo(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * (In_sData.fRate - 10.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InExpo(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * (In_sData.fRate - 10.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InExpo(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * (In_sData.fRate - 10.0f)), In_sData.fStart, In_sData.fEnd);
	}

	float InExpo(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * fRate - 10.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InExpo(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * fRate - 10.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InExpo(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : std::powf(2.0f, 10.0f * fRate - 10.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InCirc(DataRate &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf(In_sData.fRate, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InCirc(DataRate2 &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf(In_sData.fRate, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InCirc(DataRate3 &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf(In_sData.fRate, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	float InCirc(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InCirc(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InCirc(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	float InBack(DataRate &In_sData)
	{
		return Lerp(fC3 * std::powf(In_sData.fRate, 3.0f) - fC1 * std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InBack(DataRate2 &In_sData)
	{
		return Lerp(fC3 * std::powf(In_sData.fRate, 3.0f) - fC1 * std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InBack(DataRate3 &In_sData)
	{
		return Lerp(fC3 * std::powf(In_sData.fRate, 3.0f) - fC1 * std::powf(In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InBack(DataDuration &In_sData)
	{
		return Lerp(fC3 * std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f) - fC1 * std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InBack(DataDuration2 &In_sData)
	{
		return Lerp(fC3 * std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f) - fC1 * std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InBack(DataDuration3 &In_sData)
	{
		return Lerp(fC3 * std::powf((In_sData.fNowTime / In_sData.fDuration), 3.0f) - fC1 * std::powf((In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float InElastic(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * In_sData.fRate - 10.0f) * std::sinf((In_sData.fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InElastic(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * In_sData.fRate - 10.0f) * std::sinf((In_sData.fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InElastic(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * In_sData.fRate - 10.0f) * std::sinf((In_sData.fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	float InElastic(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * fRate - 10.0f) * std::sinf((fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InElastic(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * fRate - 10.0f) * std::sinf((fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InElastic(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : -std::powf(2.0f, 10.0f * fRate - 10.0f) * std::sinf((fRate * 10.0f - 10.75f) * fC4), In_sData.fStart, In_sData.fEnd);
	}

	float InBounce(DataRate &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InBounce(DataRate2 &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InBounce(DataRate3 &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	float InBounce(DataDuration &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - (In_sData.fNowTime / In_sData.fDuration)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 InBounce(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - (In_sData.fNowTime / In_sData.fDuration)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 InBounce(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - OutBounce::OutBounce(1.0f - (In_sData.fNowTime / In_sData.fDuration)), In_sData.fStart, In_sData.fEnd);
	}

	// ==============================
	//  Eas-Out
	// ==============================

	float OutSine(DataRate &In_sData)
	{
		return Lerp(std::sinf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutSine(DataRate2 &In_sData)
	{
		return Lerp(std::sinf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutSine(DataRate3 &In_sData)
	{
		return Lerp(std::sinf(In_sData.fRate * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutSine(DataDuration &In_sData)
	{
		return Lerp(std::sinf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutSine(DataDuration2 &In_sData)
	{
		return Lerp(std::sinf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutSine(DataDuration3 &In_sData)
	{
		return Lerp(std::sinf((In_sData.fNowTime / In_sData.fDuration) * fPI / 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuad(DataRate &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuad(DataRate2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuad(DataRate3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuad(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuad(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuad(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutCubic(DataRate &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutCubic(DataRate2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutCubic(DataRate3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutCubic(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutCubic(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutCubic(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 3.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuart(DataRate &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuart(DataRate2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuart(DataRate3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuart(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuart(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuart(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 4.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuint(DataRate &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuint(DataRate2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuint(DataRate3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - In_sData.fRate, 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutQuint(DataDuration &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutQuint(DataDuration2 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutQuint(DataDuration3 &In_sData)
	{
		return Lerp(1.0f - std::powf(1.0f - (In_sData.fNowTime / In_sData.fDuration), 5.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutExpo(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutExpo(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutExpo(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	float OutExpo(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutExpo(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutExpo(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * fRate), In_sData.fStart, In_sData.fEnd);
	}

	float OutCirc(DataRate &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf(In_sData.fRate - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutCirc(DataRate2 &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf(In_sData.fRate - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutCirc(DataRate3 &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf(In_sData.fRate - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	float OutCirc(DataDuration &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration) - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutCirc(DataDuration2 &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration) - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutCirc(DataDuration3 &In_sData)
	{
		return Lerp(std::sqrtf(1.0f - std::powf((In_sData.fNowTime / In_sData.fDuration) - 1.0f, 2.0f)), In_sData.fStart, In_sData.fEnd);
	}

	float OutBack(DataRate &In_sData)
	{
		return Lerp(1.0f + fC3 * std::powf(In_sData.fRate - 1.0f, 3.0f) + fC1 * std::powf(In_sData.fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutBack(DataRate2 &In_sData)
	{
		return Lerp(1.0f + fC3 * std::powf(In_sData.fRate - 1.0f, 3.0f) + fC1 * std::powf(In_sData.fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutBack(DataRate3 &In_sData)
	{
		return Lerp(1.0f + fC3 * std::powf(In_sData.fRate - 1.0f, 3.0f) + fC1 * std::powf(In_sData.fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutBack(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(1.0f + fC3 * std::powf(fRate - 1.0f, 3.0f) + fC1 * std::powf(fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutBack(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(1.0f + fC3 * std::powf(fRate - 1.0f, 3.0f) + fC1 * std::powf(fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutBack(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(1.0f + fC3 * std::powf(fRate - 1.0f, 3.0f) + fC1 * std::powf(fRate - 1.0f, 2.0f), In_sData.fStart, In_sData.fEnd);
	}

	float OutElastic(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * In_sData.fRate) * std::sinf((In_sData.fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutElastic(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * In_sData.fRate) * std::sinf((In_sData.fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutElastic(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * In_sData.fRate) * std::sinf((In_sData.fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	float OutElastic(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * fRate) * std::sinf((fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutElastic(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * fRate) * std::sinf((fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutElastic(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : std::powf(2.0f, -10.0f * fRate) * std::sinf((fRate * 10.0f - 0.75f) * fC4) + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	float OutBounce(DataRate &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutBounce(DataRate2 &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutBounce(DataRate3 &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fRate), In_sData.fStart, In_sData.fEnd);
	}

	float OutBounce(DataDuration &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fNowTime / In_sData.fDuration), In_sData.fStart, In_sData.fEnd);
	}

	Float2 OutBounce(DataDuration2 &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fNowTime / In_sData.fDuration), In_sData.fStart, In_sData.fEnd);
	}

	Float3 OutBounce(DataDuration3 &In_sData)
	{
		return Lerp(OutBounce::OutBounce(In_sData.fNowTime / In_sData.fDuration), In_sData.fStart, In_sData.fEnd);
	}

	// ==============================
	//  Eas-InOut
	// ==============================

	float InOutSine(DataRate &In_sData)
	{
		return Lerp(-(std::cos(fPI * In_sData.fRate) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutSine(DataRate2 &In_sData)
	{
		return Lerp(-(std::cos(fPI * In_sData.fRate) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutSine(DataRate3 &In_sData)
	{
		return Lerp(-(std::cos(fPI * In_sData.fRate) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutSine(DataDuration &In_sData)
	{
		return Lerp(-(std::cos(fPI * (In_sData.fNowTime / In_sData.fDuration)) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutSine(DataDuration2 &In_sData)
	{
		return Lerp(-(std::cos(fPI * (In_sData.fNowTime / In_sData.fDuration)) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutSine(DataDuration3 &In_sData)
	{
		return Lerp(-(std::cos(fPI * (In_sData.fNowTime / In_sData.fDuration)) - 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuad(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 2.0f * std::powf(In_sData.fRate, 2.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuad(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 2.0f * std::powf(In_sData.fRate, 2.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuad(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 2.0f * std::powf(In_sData.fRate, 2.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuad(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 2.0f * std::powf(fRate, 2.0f) : 1.0f - std::powf(-2.0f * fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuad(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 2.0f * std::powf(fRate, 2.0f) : 1.0f - std::powf(-2.0f * fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuad(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 2.0f * std::powf(fRate, 2.0f) : 1.0f - std::powf(-2.0f * fRate * 2.0f, 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutCubic(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 4.0f * std::powf(In_sData.fRate, 3.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutCubic(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 4.0f * std::powf(In_sData.fRate, 3.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutCubic(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 4.0f * std::powf(In_sData.fRate, 3.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutCubic(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 4.0f * std::powf(fRate, 3.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutCubic(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 4.0f * std::powf(fRate, 3.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutCubic(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 4.0f * std::powf(fRate, 3.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 3.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuart(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 8.0f * std::powf(In_sData.fRate, 4.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuart(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 8.0f * std::powf(In_sData.fRate, 4.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuart(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 8.0f * std::powf(In_sData.fRate, 4.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuart(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 8.0f * std::powf(fRate, 4.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuart(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 8.0f * std::powf(fRate, 4.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuart(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 8.0f * std::powf(fRate, 4.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 4.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuint(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 16.0f * std::powf(In_sData.fRate, 5.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuint(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 16.0f * std::powf(In_sData.fRate, 5.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuint(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? 16.0f * std::powf(In_sData.fRate, 5.0f) : 1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutQuint(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 16.0f * std::powf(fRate, 5.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutQuint(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 16.0f * std::powf(fRate, 5.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutQuint(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? 16.0f * std::powf(fRate, 5.0f) : 1.0f - std::powf(-2.0f * fRate + 2.0f, 5.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutExpo(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ? std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutExpo(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ? std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutExpo(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ? std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutExpo(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ? std::powf(2.0f, 20.0f * fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutExpo(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ? std::powf(2.0f, 20.0f * fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutExpo(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ? std::powf(2.0f, 20.0f * fRate - 10.0f) / 2.0f : (2.0f - std::powf(2.0f, -20.0f * fRate + 10.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutCirc(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * In_sData.fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutCirc(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * In_sData.fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutCirc(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * In_sData.fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * In_sData.fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutCirc(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutCirc(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutCirc(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - std::sqrtf(1.0f - std::powf(2.0f * fRate, 2.0f))) / 2.0f : (std::sqrtf(1.0f - std::powf(-2.0f * fRate + 2.0f, 2.0f)) + 1.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutBack(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ?
			(std::powf(2.0f * In_sData.fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * In_sData.fRate - fC2)) / 2.0f :
			(std::powf(2.0f * In_sData.fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (In_sData.fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutBack(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ?
			(std::powf(2.0f * In_sData.fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * In_sData.fRate - fC2)) / 2.0f :
			(std::powf(2.0f * In_sData.fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (In_sData.fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutBack(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ?
			(std::powf(2.0f * In_sData.fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * In_sData.fRate - fC2)) / 2.0f :
			(std::powf(2.0f * In_sData.fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (In_sData.fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutBack(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ?
			(std::powf(2.0f * fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * fRate - fC2)) / 2.0f :
			(std::powf(2.0f * fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutBack(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ?
			(std::powf(2.0f * fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * fRate - fC2)) / 2.0f :
			(std::powf(2.0f * fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutBack(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ?
			(std::powf(2.0f * fRate, 2.0f) * ((fC2 + 1.0f) * 2.0f * fRate - fC2)) / 2.0f :
			(std::powf(2.0f * fRate - 2.0f, 2.0f) * ((fC2 + 1.0f) * (fRate * 2.0f - 2.0f) + fC2) + 2.0f) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutElastic(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutElastic(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutElastic(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate == 0.0f ? 0.0f : In_sData.fRate == 1.0f ? 1.0f : In_sData.fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * In_sData.fRate - 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * In_sData.fRate + 10.0f) * std::sinf((20.0f * In_sData.fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutElastic(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * fRate - 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * fRate + 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutElastic(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * fRate - 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * fRate + 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutElastic(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate == 0.0f ? 0.0f : fRate == 1.0f ? 1.0f : fRate < 0.5f ?
			-(std::powf(2.0f, 20.0f * fRate - 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f :
			(std::powf(2.0f, -20.0f * fRate + 10.0f) * std::sinf((20.0f * fRate - 11.125f) * fC5)) / 2.0f + 1.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutBounce(DataRate &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * In_sData.fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * In_sData.fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutBounce(DataRate2 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * In_sData.fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * In_sData.fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutBounce(DataRate3 &In_sData)
	{
		return Lerp(In_sData.fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * In_sData.fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * In_sData.fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	float InOutBounce(DataDuration &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float2 InOutBounce(DataDuration2 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

	Float3 InOutBounce(DataDuration3 &In_sData)
	{
		float fRate = In_sData.fNowTime / In_sData.fDuration;
		return Lerp(fRate < 0.5f ? (1.0f - OutBounce::OutBounce(1.0f - 2.0f * fRate)) / 2.0f : (1.0f + OutBounce::OutBounce(2.0f * fRate - 1.0f)) / 2.0f, In_sData.fStart, In_sData.fEnd);
	}

} // namespace Ease
