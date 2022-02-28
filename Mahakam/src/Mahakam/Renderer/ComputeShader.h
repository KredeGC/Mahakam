#pragma once

#include "Texture.h"

namespace Mahakam
{
	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;

		virtual void Bind() const = 0;

		virtual void Dispatch() = 0;

		virtual void SetTexture(const std::string& name, Ref<Texture> tex) = 0;

		static Ref<ComputeShader> Create(const std::string& filepath, uint32_t x, uint32_t y, uint32_t z);
	};
}