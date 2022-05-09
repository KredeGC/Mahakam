#pragma once

#include "Texture.h"
#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;

		virtual void Bind() const = 0;

		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;

		virtual void SetTexture(const std::string& name, Asset<Texture> tex) = 0;

		//static Ref<ComputeShader> Create(const std::string& filepath);
		MH_DECLARE_FUNC(Create, Asset<ComputeShader>, const std::string& filepath);
	};
}