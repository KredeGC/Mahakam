#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Mahakam
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> MahakamLogger;
		static std::shared_ptr<spdlog::logger> gameLogger;

	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getMahakamLogger()
		{
			return MahakamLogger;
		}

		inline static std::shared_ptr<spdlog::logger>& getGameLogger()
		{
			return gameLogger;
		}
	};
}

// Core log macros
#define MH_CORE_TRACE(...)    ::Mahakam::Log::getMahakamLogger()->trace(__VA_ARGS__)
#define MH_CORE_INFO(...)     ::Mahakam::Log::getMahakamLogger()->info(__VA_ARGS__)
#define MH_CORE_WARN(...)     ::Mahakam::Log::getMahakamLogger()->warn(__VA_ARGS__)
#define MH_CORE_ERROR(...)    ::Mahakam::Log::getMahakamLogger()->error(__VA_ARGS__)
#define MH_CORE_FATAL(...)    ::Mahakam::Log::getMahakamLogger()->fatal(__VA_ARGS__)

// Client log macros
#define MH_TRACE(...)	      ::Mahakam::Log::getGameLogger()->trace(__VA_ARGS__)
#define MH_INFO(...)	      ::Mahakam::Log::getGameLogger()->info(__VA_ARGS__)
#define MH_WARN(...)	      ::Mahakam::Log::getGameLogger()->warn(__VA_ARGS__)
#define MH_ERROR(...)	      ::Mahakam::Log::getGameLogger()->error(__VA_ARGS__)
#define MH_FATAL(...)	      ::Mahakam::Log::getGameLogger()->fatal(__VA_ARGS__) 