#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Scene/Entity.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam::Editor
{
    class PropertyRegistry
    {
    public:
        using PropertyPtr = void (*)(Entity);
		using PropertyMap = UnorderedMap<std::string, PropertyPtr>;

    private:
        static PropertyMap s_Properties;

    public:
        MH_DECLARE_FUNC(Register, void, const std::string& component, PropertyPtr onInspector);
        MH_DECLARE_FUNC(Deregister, void, const std::string& component);
        MH_DECLARE_FUNC(GetProperty, PropertyPtr, const std::string& component);
    };
}