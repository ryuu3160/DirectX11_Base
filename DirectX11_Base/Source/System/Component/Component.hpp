/*+===================================================================
	File: Component.hpp
	Summary: コンポーネントの基底クラス 佐々木先生のコードを参考に作成
	Author: AT13C192 01 青木雄一郎
	Date: 2025/06/16 Mon AM 10:28:25 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/Object/Object.hpp"
// ==============================
// 前方宣言
// ==============================
class GameObject;

#undef GetObject

/// <summary>
/// Componentクラス
/// </summary>
class Component : public Object
{
	friend class GameObject;
public:
	class DataAccessor
	{
	public:
		DataAccessor(_In_ cpon_block::Object In_Object)
		{
			m_IsWrite = In_Object->IsEmpty();
			m_Data = In_Object;
		}
		~DataAccessor()
		{
			m_Data.reset();
		}

		template<TypeValue T>
		void AccessValue(_In_ const std::string_view In_Key, _Inout_opt_ T *Inout_Value)
		{
			if (m_IsWrite)
			{
				if(Inout_Value != nullptr)
					(*m_Data)[0]->SetValue(In_Key, *Inout_Value);
			}
			else
			{
				Inout_Value = (*m_Data)[0]->GetValuePtr<T>(In_Key);
			}
		}

		template<TypeValue T>
		void AccessArray(_In_ const std::string_view In_Key, _Inout_opt_ std::vector<T> *Inout_Array)
		{
			if (m_IsWrite)
			{
				(*m_Data)[0]->CreateArray(In_Key, *Inout_Array);
			}
			else
			{
				Inout_Array = (*m_Data)[0]->GetArrayPtr<T>(In_Key);
			}
		}

		void AccessObject(_In_ const std::string_view In_Key, _Inout_opt_ cpon_block::Object Inout_Object)
		{
			if (m_IsWrite)
			{
				(*m_Data)[0]->AddObject(Inout_Object);
			}
			else
			{
				Inout_Object = (*m_Data)[0]->GetObject(In_Key);
			}
		}
	private:
		cpon_block::Object m_Data;
		bool m_IsWrite; // 書き込みモードか読み込みモードか
	};

public:
	Component(_In_ std::string In_Name);
	virtual  ~Component();
	virtual void OnEnable() noexcept override {};
	virtual void Init() noexcept override = 0;
	virtual void Update(_In_ float In_Tick) noexcept override;
	virtual void LateUpdate(_In_ float In_Tick) noexcept override;
	virtual void FixedUpdate(_In_ double In_FixedTick) noexcept override;

	virtual void SaveLoad(_In_ DataAccessor *In_Data) {}

	GameObject *GetGameObject() const noexcept { return m_pTransform; }

	void DestroySelf() noexcept override final;

protected:

#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif // _DEBUG

private:
	void DataWrite(_In_ std::shared_ptr<cpon_block> In_pCponBlock);
	void DataRead(_In_ std::shared_ptr<cpon_object> In_pCponObj);

protected:
	GameObject *m_pTransform;
	std::string m_Name;
};
