/*+===================================================================
	File: ShaderParam.hpp
	Summary: シェーダーのパラメーターを保持するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/6 Sat AM 01:14:23 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

/// <summary>
/// ShaderParamクラス
/// </summary>
class ShaderParam
{
public:

	template <typename T>
	ShaderParam(_In_ std::string In_ParamName, _In_ int In_SlotNum, _In_ T *In_Param, _In_ size_t In_ArraySize)
		: m_pParam(In_Param), m_SlotNum(In_SlotNum), m_ParamName(In_ParamName)
	{
		if (In_Param == nullptr || In_ArraySize == 0)
		{
			m_pParam = nullptr;
			m_SlotNum = -1;
			m_ParamName = "";
			return;
		}

		auto work = new T[In_ArraySize];
		for (size_t i = 0; i < In_ArraySize; ++i)
		{
			work[i] = In_Param[i];
		}
		m_pParam = work;
	}

	~ShaderParam();

	/// <summary>
	/// パラメータを取得する
	/// </summary>
	/// <returns>パラメータへのポインタを返します。</returns>
	inline void *GetParam() noexcept { return m_pParam; }

	/// <summary>
	/// スロット番号を取得します。
	/// </summary>
	/// <returns>m_Type を unsigned int 型にキャストした値（スロット番号）。</returns>
	inline const unsigned int GetSlotNum() const noexcept { return m_SlotNum; }

	inline const std::string_view GetParamName() const noexcept { return m_ParamName; }

protected:
	std::string m_ParamName;
	int m_SlotNum; // スロット番号
	void *m_pParam;
};
