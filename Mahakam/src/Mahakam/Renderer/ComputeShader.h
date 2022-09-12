#pragma once

#include "Mahakam/Asset/Asset.h"

#include <filesystem>

namespace Mahakam
{
	class ComputeShader;
	class Texture;

	extern template class Asset<ComputeShader>;
	extern template class Asset<Texture>;

	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;

		virtual void Bind() const = 0;

		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;

		virtual void SetTexture(const std::string& name, Asset<Texture> tex) = 0;

		MH_DECLARE_FUNC(Create, Ref<ComputeShader>, const std::filesystem::path& filepath);
	};
}