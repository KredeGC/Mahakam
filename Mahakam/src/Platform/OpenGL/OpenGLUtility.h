#pragma once
#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Profiler.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace Mahakam
{
	class OpenGLUtility
	{
	public:
		inline static std::string SortIncludes(const std::string& source)
		{
			MH_PROFILE_FUNCTION();

			std::stringstream sourceStream;

			const char* typeToken = "#include \"";
			size_t typeTokenLength = strlen(typeToken);
			size_t lastPos = 0;
			size_t pos = source.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				sourceStream << source.substr(lastPos, pos - lastPos);

				size_t end = source.find("\"", pos + typeTokenLength);
				size_t begin = pos + typeTokenLength;

				lastPos = end + 1;

				pos = source.find(typeToken, end);


				std::string includeFile = source.substr(begin, end - begin);

				std::string includeSource = ReadFile(includeFile);

				sourceStream << SortIncludes(includeSource);
			}

			sourceStream << source.substr(lastPos, source.size() - lastPos);

			return sourceStream.str();
		}

		inline static std::string ReadFile(const std::filesystem::path& filepath)
		{
			MH_PROFILE_FUNCTION();

			std::string result;
			std::ifstream stream(filepath, std::ios::in | std::ios::binary);
			if (stream)
			{
				stream.seekg(0, std::ios::end);
				result.resize(stream.tellg());

				stream.seekg(0, std::ios::beg);
				stream.read(&result[0], result.size());

				stream.close();
			}
			else
			{
				MH_CORE_WARN("Could not open file {0}", filepath);
			}

			return result;
		}
	};
}