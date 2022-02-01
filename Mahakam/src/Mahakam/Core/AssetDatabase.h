#pragma once
#include "Core.h"

#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class AssetDatabase
	{
	private:
		static std::string CreateDirectories(const std::string& src)
		{
			auto lastDir = src.rfind('/');
			const std::string cacheDirectory = "cache/" + src.substr(0, lastDir) + "/";

			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);

			return "cache/" + src + ".dat";
		}

	public:
		// Texture2D
		template<typename T, typename = typename std::enable_if<std::is_same<T, Texture2D>::value, void>::type>
		static auto CreateOrLoadAsset(const std::string& src, bool saveMips, const TextureProps& props, typename std::enable_if<std::is_same<T, Texture2D>::value, void>::type* dummy = nullptr)
		{
			MH_CORE_ASSERT(props.width > 0 && props.height, "Width and height must be specified when loading or saving assets!");

			const std::string filepath = CreateDirectories(src);

			if (!std::filesystem::exists(filepath))
			{
				Ref<T> asset = T::create(src, props);

				uint32_t size = asset->getSize();
				uint32_t totalSize = asset->getTotalSize();

				// Save to cache
				char* pixels = new char[totalSize];
				asset->readPixels(pixels, saveMips);
				std::ofstream stream(filepath, std::ios::binary);
				stream.write((char*)&size, sizeof(uint32_t));
				stream.write(pixels, totalSize);

				delete[] pixels;

				return asset;
			}
			else
			{
				// Load from cache
				std::ifstream inStream(filepath, std::ios::binary);
				std::stringstream ss;
				uint32_t size = 0;
				inStream.read((char*)&size, sizeof(uint32_t));
				ss << inStream.rdbuf();
				Ref<T> lut = T::create(props);
				lut->setData((void*)ss.str().c_str(), size, saveMips);

				return lut;
			}
		}

		// TextureCube
		template<typename T, typename = typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type>
		static auto CreateOrLoadAsset(const std::string& src, bool saveMips, const CubeTextureProps& props, typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type* dummy = nullptr)
		{
			MH_CORE_ASSERT(props.resolution, "Resolution must be specified when loading or saving assets!");

			const std::string filepath = CreateDirectories(src);

			if (!std::filesystem::exists(filepath))
			{
				Ref<T> asset = T::create(src, props);

				uint32_t size = asset->getSize();
				uint32_t totalSize = asset->getTotalSize();

				// Save to cache
				char* pixels = new char[totalSize];
				asset->readPixels(pixels, saveMips);
				std::ofstream stream(filepath, std::ios::binary);
				stream.write((char*)&size, sizeof(uint32_t));
				stream.write(pixels, totalSize);

				delete[] pixels;

				return asset;
			}
			else
			{
				// Load from cache
				std::ifstream inStream(filepath, std::ios::binary);
				std::stringstream ss;
				uint32_t size = 0;
				inStream.read((char*)&size, sizeof(uint32_t));
				ss << inStream.rdbuf();
				Ref<T> lut = T::create(props);
				lut->setData((void*)ss.str().c_str(), size, saveMips);

				return lut;
			}
		}

		// TextureCube - Convolute or prefilter
		template<typename T, typename = typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type>
		static auto CreateOrLoadAsset(const std::string& filepath, Ref<T> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props, typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type* dummy = nullptr)
		{
			MH_CORE_ASSERT(props.resolution, "Resolution must be specified when loading or saving assets!");

			if (!std::filesystem::exists(filepath))
			{
				Ref<T> asset = T::create(src, prefilter, props);

				uint32_t size = asset->getSize();
				uint32_t totalSize = asset->getTotalSize();

				// Save to cache
				char* pixels = new char[totalSize];
				asset->readPixels(pixels, saveMips);
				std::ofstream stream(filepath, std::ios::binary);
				stream.write((char*)&size, sizeof(uint32_t));
				stream.write(pixels, totalSize);

				delete[] pixels;

				return asset;
			}
			else
			{
				// Load from cache
				std::ifstream inStream(filepath, std::ios::binary);
				std::stringstream ss;
				uint32_t size = 0;
				inStream.read((char*)&size, sizeof(uint32_t));
				ss << inStream.rdbuf();
				Ref<T> lut = T::create(props);
				lut->setData((void*)ss.str().c_str(), size, saveMips);

				return lut;
			}
		}
	};
}