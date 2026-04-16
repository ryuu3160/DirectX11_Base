/*+===================================================================
	File: ComponentRegistry.cpp
	Summary: ComponentRegistryƒNƒ‰ƒX
	Author: AT13C192 01 Â–Ø—Yˆê˜Y
	Date: 2026/2/2 Mon PM 12:13:53 ‰‰ñì¬
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ComponentRegistry.hpp"

void ComponentRegistry::RegisterComponent(_In_ const ComponentInfo &In_Info)
{
	m_Components.try_emplace(In_Info.Name, In_Info);
}

Component *ComponentRegistry::CreateComponent(_In_ std::string_view In_Name, _In_ GameObject *In_Owner)
{
    auto itr = m_Components.find(In_Name.data());
    if(itr != m_Components.end())
    {
        return itr->second.CreateFunc(In_Owner);
    }
    return nullptr;
}

const std::unordered_map<std::string, ComponentInfo> &ComponentRegistry::GetAllComponents() const
{
    return m_Components;
}

std::vector<std::string> ComponentRegistry::GetAllCategories() const
{
    std::vector<std::string> categories;
    for(const auto &[name, info] : m_Components)
    {
        if(std::find(categories.begin(), categories.end(), info.Category) == categories.end())
        {
            categories.push_back(info.Category);
        }
    }
    std::sort(categories.begin(), categories.end());
    return categories;
}

int ComponentRegistry::GetComponentCount() const
{
	return static_cast<int>(m_Components.size());
}

const ComponentInfo *ComponentRegistry::FindComponentInfo(_In_ std::string_view name) const
{
	auto itr = m_Components.find(name.data());
	if(itr != m_Components.end())
		return &itr->second;

	return nullptr;
}

void ComponentRegistry::Clear()
{
	m_Components.clear();
}

ComponentRegistry::ComponentRegistry()
{
}

ComponentRegistry::~ComponentRegistry()
{
	m_Components.clear();
}
