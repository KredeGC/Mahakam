#pragma once

#include "AssetDatabase.h"

namespace Mahakam
{
    template<size_t I>
    struct fixed_string
    {
        char Chars[I]{};

        consteval fixed_string(const char* s) noexcept
        {
            for (size_t i = 0; i != I; ++i)
                Chars[i] = s[i];
        }
    };

    template<size_t I>
    fixed_string(const char(&arr)[I]) -> fixed_string<I - 1>;

    template<fixed_string S>
    struct AssetLookup {};

    template<typename T, AssetID I>
    struct AssetBaseLookup
    {
        constexpr static AssetID ID = I;

        operator Asset<T>()
        {
            return Asset<T>(ID);
        }
    };

    class Material;

    template<>
    struct AssetLookup<"test/assets/materials/skybox"> : AssetBaseLookup<Material, 1413214623375882268ULL> {};
}