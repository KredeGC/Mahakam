#include "mhpch.h"
#include "Animation.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
    Ref<Animation> Animation::Load(const std::string& filepath, SkinnedMesh& skinnedMesh)
    {
        MH_OVERRIDE_FUNC(animationLoad, filepath, skinnedMesh);

        return CreateRef<Animation>(filepath, skinnedMesh);
    }
}