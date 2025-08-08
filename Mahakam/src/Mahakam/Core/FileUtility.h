#pragma once

#include "Types.h"

#include <string>
#include <filesystem>

namespace Mahakam
{
	typedef std::filesystem::path Filepath;

	class FileUtility
	{
	public:
		inline static Filepath PROJECT_PATH = std::filesystem::current_path();
		inline static Filepath CACHE_PATH = "cache";
		inline static Filepath RESOURCE_PATH = "resource";
		inline static Filepath IMPORT_PATH = "import";
		inline static Filepath ASSET_PATH = "assets";

		inline static const std::string AssetExtension = ".asset";

		static void SetProjectDirectory(const Filepath& filepath) noexcept;

		static void SetWorkingDirectory(const Filepath& filepath) noexcept;
		static Filepath GetWorkingDirectory() noexcept;

		static bool ReadFile(const Filepath& filepath, TrivialVector<char>& buffer) noexcept;

		static constexpr size_t Hash(const char* filepath, size_t length) noexcept
		{
			constexpr uint64_t fnv_prime = 1099511628211ULL;
			constexpr uint64_t fnv_offset_basis = 14695981039346656037ULL;

			uint64_t hash = fnv_offset_basis;

			for (size_t i = 0; i < length; ++i)
			{
				char c = filepath[i];

				if (c == '/' || c == '\\')
					continue;

				hash ^= c;
				hash *= fnv_prime;
			}

			return hash;
		}

		static consteval size_t Hash(const char* filepath) noexcept
		{
			return Hash(filepath, std::char_traits<char>::length(filepath));
		}

		static size_t Hash(const Filepath& filepath) noexcept;

		static bool Exists(const Filepath& src) noexcept;
		static void CreateDirectories(const Filepath& src) noexcept;
		static Filepath Relative(const Filepath& filepath) noexcept;
		static Filepath GetCachePath(const Filepath& filepath) noexcept;
		static Filepath GetImportPath(const Filepath& filepath, const std::string& extension) noexcept;

		// Defined by each platform
		static Filepath OpenFile(const char* filter, const Filepath& basePath) noexcept;
		static Filepath SaveFile(const char* filter, const Filepath& basePath) noexcept;
		static Filepath OpenDirectory() noexcept;
	};
}