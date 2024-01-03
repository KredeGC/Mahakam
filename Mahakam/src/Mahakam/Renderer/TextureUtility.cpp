#include "Mahakam/mhpch.h"

#include "TextureUtility.h"

#include <stb_image.h>

#include <algorithm>
#include <cstring>

namespace Mahakam
{
	image_data LoadImageFile(const char* filepath, int& w, int& h, int& channels, bool& hdr, int desiredChannels) noexcept
	{
		MH_PROFILE_FUNCTION();

		stbi_set_flip_vertically_on_load(1);

		void* data = nullptr;
		hdr = stbi_is_hdr(filepath);
		if (hdr)
			data = stbi_loadf(filepath, &w, &h, &channels, desiredChannels);
		else
			data = stbi_load(filepath, &w, &h, &channels, desiredChannels);

		MH_ASSERT(data, "Failed to load image!");

		return std::unique_ptr<void, image_deleter>(data);
	}

	void image_deleter::operator()(void* data) noexcept
	{
		stbi_image_free(data);
	}

	static uint32_t FloatTo11Bits(float f) noexcept
	{
		uint32_t floatBits;
		std::memcpy(&floatBits, &f, sizeof(uint32_t));

		uint32_t exponent = (floatBits >> 23U) & 0xffU;
		uint32_t mantissa = floatBits & 0x7fffffU;

		uint32_t float11Exponent = exponent - 127 + 15;
		uint32_t float11Mantissa = mantissa >> 17;

		if (exponent < 112)
		{
			float11Exponent = 0;
			float11Mantissa = 0;
		}

		uint32_t float11Bits = (float11Exponent << 6) | float11Mantissa;

		return float11Bits;
	}

	static uint32_t FloatTo10Bits(float f) noexcept
	{
		uint32_t floatBits;
		std::memcpy(&floatBits, &f, sizeof(uint32_t));

		uint32_t exponent = (floatBits >> 23U) & 0xffU;
		uint32_t mantissa = floatBits & 0x7fffffU;

		uint32_t float10Exponent = exponent - 127 + 15;
		uint32_t float10Mantissa = mantissa >> 18;

		if (exponent < 112)
		{
			float10Exponent = 0;
			float10Mantissa = 0;
		}

		uint32_t float10Bits = (float10Exponent << 5) | float10Mantissa;

		return float10Bits;
	}

	uint32_t Vec3ToRG11B10F(const glm::vec3& vec) noexcept
	{
		uint32_t rbits = FloatTo11Bits(vec.x);
		uint32_t gbits = FloatTo11Bits(vec.y);
		uint32_t bbits = FloatTo10Bits(vec.z);

		uint32_t result = rbits | (gbits << 11) | (bbits << 22);

		return result;
	}

	static const std::array<std::array<glm::vec3, 3>, 6> startRightUp = { {
		{{{1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}},  // front (Right)
		{{{-1.0f, 1.0f, 1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}},  // back (Left)
		{{{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}}},   // up (Down)
		{{{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}}},  // down (Up)
		{{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}}},   // right (Back)
		{{{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}}   // left (Front)
	} };

	// Matrix form
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	//glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

	TrivialArray<float, Allocator::BaseAllocator<float>> ProjectEquirectangularToCubemap(float* data, uint32_t width, uint32_t height, int channels, bool hdr, uint32_t resolution) noexcept
	{
		size_t faceSize = static_cast<size_t>(resolution) * resolution * channels;

		TrivialArray<float, Allocator::BaseAllocator<float>> faces(faceSize * 6ull, Allocator::GetAllocator<float>());

        const float PI = 3.1415f;

        for (size_t i = 0; i < 6; i++)
        {
            const glm::vec3& start = startRightUp[i][0];
            const glm::vec3& right = startRightUp[i][1];
            const glm::vec3& up = startRightUp[i][2];

            float* face = faces.begin() + i * faceSize;
            glm::vec3 pixelDirection3d;
            for (uint32_t row = 0; row < resolution; row++)
            {
                for (uint32_t col = 0; col < resolution; col++)
                {
                    pixelDirection3d.x = start.x + ((col * 2.0f + 0.5f) / resolution) * right.x + ((row * 2.0f + 0.5f) / resolution) * up.x;
                    pixelDirection3d.y = start.y + ((col * 2.0f + 0.5f) / resolution) * right.y + ((row * 2.0f + 0.5f) / resolution) * up.y;
                    pixelDirection3d.z = start.z + ((col * 2.0f + 0.5f) / resolution) * right.z + ((row * 2.0f + 0.5f) / resolution) * up.z;

                    float azimuth = glm::atan(pixelDirection3d.x, -pixelDirection3d.z) + PI; // add pi to move range to 0-360 deg
                    float elevation = glm::atan(pixelDirection3d.y / glm::sqrt(pixelDirection3d.x * pixelDirection3d.x + pixelDirection3d.z * pixelDirection3d.z)) + PI / 2.0f;

                    float colHdri = (azimuth / (PI * 2.0f)) * width; // add pi to azimuth to move range to 0-360 deg
                    float rowHdri = (elevation / PI) * height;

                    uint32_t colNearest = glm::clamp(static_cast<uint32_t>(colHdri), 0u, width - 1u);
                    uint32_t rowNearest = glm::clamp(static_cast<uint32_t>(rowHdri), 0u, height - 1u);

                    face[col * channels + resolution * row * channels] = data[colNearest * channels + width * rowNearest * channels]; // red
					if (channels > 1)
						face[col * channels + resolution * row * channels + 1] = data[colNearest * channels + width * rowNearest * channels + 1]; //green
					if (channels > 2)
						face[col * channels + resolution * row * channels + 2] = data[colNearest * channels + width * rowNearest * channels + 2]; //blue
                    if (channels > 3)
                        face[col * channels + resolution * row * channels + 3] = data[colNearest * channels + width * rowNearest * channels + 3]; //alpha
                }
            }
        }

		return faces;
	}
}