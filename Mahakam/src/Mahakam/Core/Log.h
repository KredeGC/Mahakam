#pragma once
#include "SharedLibrary.h"
#include "Types.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Mahakam
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;

	public:
		static void Init(const char* name);
		static void Shutdown();
        static void ImportSinks(const std::vector<spdlog::sink_ptr>& sinks);

		static spdlog::source_loc GetLocation(const char* filename, int line, const char* funcname);

		static std::shared_ptr<spdlog::logger> GetLogger();
	};
}

// Log macros
#define MH_TRACE(...)    ::Mahakam::Log::GetLogger()->log(::Mahakam::Log::GetLocation(__FILE__, __LINE__, SPDLOG_FUNCTION), spdlog::level::trace, __VA_ARGS__)
#define MH_INFO(...)     ::Mahakam::Log::GetLogger()->log(::Mahakam::Log::GetLocation(__FILE__, __LINE__, SPDLOG_FUNCTION), spdlog::level::info, __VA_ARGS__)
#define MH_WARN(...)     ::Mahakam::Log::GetLogger()->log(::Mahakam::Log::GetLocation(__FILE__, __LINE__, SPDLOG_FUNCTION), spdlog::level::warn, __VA_ARGS__)
#define MH_ERROR(...)    ::Mahakam::Log::GetLogger()->log(::Mahakam::Log::GetLocation(__FILE__, __LINE__, SPDLOG_FUNCTION), spdlog::level::err, __VA_ARGS__)
#define MH_FATAL(...)    ::Mahakam::Log::GetLogger()->log(::Mahakam::Log::GetLocation(__FILE__, __LINE__, SPDLOG_FUNCTION), spdlog::level::critical, __VA_ARGS__)