#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Types.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace Mahakam
{
	struct image_deleter
	{
		void operator()(void* data) noexcept;
	};

	using image_data = std::unique_ptr<void, image_deleter>;

	image_data LoadImageFile(const char* filepath, int& w, int& h, int& channels, bool& hdr, int desiredChannels = 0) noexcept;

	uint32_t Vec3ToRG11B10F(const glm::vec3& vec) noexcept;

	TrivialArray<float, Allocator::BaseAllocator<float>> ProjectEquirectangularToCubemap(float* data, uint32_t width, uint32_t height, int channels, bool hdr, uint32_t resolution) noexcept;
}