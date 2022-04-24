#pragma once

#include <string>
#include <filesystem>

namespace Mahakam
{
	class FileUtility
	{
	public:
		inline static bool Exists(const std::string& src)
		{
			return std::filesystem::exists(src);
		}

		inline static void CreateDirectories(const std::string& src)
		{
			if (!std::filesystem::exists(src))
				std::filesystem::create_directories(src);
		}

		inline static std::string GetCachePath(const std::string& src)
		{
			auto lastDir = src.rfind('/');
			const std::string cacheDirectory = "cache/" + src.substr(0, lastDir) + "/";

			CreateDirectories(cacheDirectory);

			return "cache/" + src + ".cache";
		}

		inline static std::string GetResourcePath(const std::string& src)
		{
			auto lastDir = src.rfind('/');
			const std::string cacheDirectory = "res/" + src.substr(0, lastDir) + "/";

			CreateDirectories(cacheDirectory);

			return "res/" + src + ".bin";
		}

		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}