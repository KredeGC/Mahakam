#include "mhpch.h"
#include "PropertyRegistry.h"

namespace Mahakam::Editor
{
    PropertyRegistry::PropertyMap PropertyRegistry::s_Properties;

    //void PropertyRegistry::Register(const std::string& component, PropertyPtr)
    MH_DEFINE_FUNC(PropertyRegistry::Register, void, const std::string& component, PropertyRegistry::PropertyPtr onInspector)
    {
        s_Properties[component] = onInspector;
    };

    //void PropertyRegistry::Deregister(const std::string& component)
    MH_DEFINE_FUNC(PropertyRegistry::Deregister, void, const std::string& component)
    {
        s_Properties.erase(component);
    };

    //PropertyRegistry::PropertyPtr PropertyRegistry::GetProperty(const std::string& component)
    MH_DEFINE_FUNC(PropertyRegistry::GetProperty, PropertyRegistry::PropertyPtr, const std::string& component)
    {
        auto iter = s_Properties.find(component);
        if (iter != s_Properties.end())
            return iter->second;
        
        return nullptr;
    };
}