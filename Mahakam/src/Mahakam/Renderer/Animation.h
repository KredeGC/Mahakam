#pragma once

#include "Mahakam/Asset/Asset.h"

namespace std
{
	namespace filesystem
	{
		class path;
	}
}

namespace Mahakam
{
	class Animation
	{
	public:
		Animation(const std::filesystem::path& filepath);

		inline static Asset<Animation> Load(const std::filesystem::path& filepath) { return LoadImpl(filepath); }

	private:
		MH_DECLARE_FUNC(LoadImpl, Asset<Animation>, const std::filesystem::path& filepath);
	};
}