/*+===================================================================
	File: ComponentRegistry.hpp
	Summary: ComponentRegistryクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2026/2/2 Mon PM 12:13:47 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/ryuu_lib/Singleton/Singleton.hpp"

// ==============================
//	前方宣言
// ==============================
class Component;
class GameObject;

struct ComponentInfo
{
	std::string Name;
	std::string Category;
	std::string Description;
	std::function<Component *(GameObject *)> CreateFunc;
};

/// <summary>
/// ComponentRegistryクラス
/// </summary>
class ComponentRegistry : public Singleton<ComponentRegistry>
{
	friend class Singleton<ComponentRegistry>;
public:
    void RegisterComponent(_In_ const ComponentInfo &In_Info);

    Component *CreateComponent(_In_ std::string_view In_Name, _In_ GameObject *In_Owner);

    const std::unordered_map<std::string, ComponentInfo> &GetAllComponents() const;

	std::vector<std::string> GetAllCategories() const;

	int GetComponentCount() const;

private:
	ComponentRegistry();
	~ComponentRegistry();

private:
    std::unordered_map<std::string, ComponentInfo> m_Components;
};

/// <summary>
/// すべてのコンポーネントを登録（自動生成関数）
/// </summary>
void RegisterAllComponents();
