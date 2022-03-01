#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Mahakam
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> engineLogger;
		static std::shared_ptr<spdlog::logger> gameLogger;

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger()
		{
			return engineLogger;
		}

		inline static std::shared_ptr<spdlog::logger>& GetGameLogger()
		{
			return gameLogger;
		}
	};
}

// Core log macros
#define MH_CORE_TRACE(...)    ::Mahakam::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define MH_CORE_INFO(...)     ::Mahakam::Log::GetEngineLogger()->info(__VA_ARGS__)
#define MH_CORE_WARN(...)     ::Mahakam::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define MH_CORE_ERROR(...)    ::Mahakam::Log::GetEngineLogger()->error(__VA_ARGS__)
#define MH_CORE_FATAL(...)    ::Mahakam::Log::GetEngineLogger()->critical(__VA_ARGS__)

// Game log macros
#define MH_TRACE(...)	      ::Mahakam::Log::GetGameLogger()->trace(__VA_ARGS__)
#define MH_INFO(...)	      ::Mahakam::Log::GetGameLogger()->info(__VA_ARGS__)
#define MH_WARN(...)	      ::Mahakam::Log::GetGameLogger()->warn(__VA_ARGS__)
#define MH_ERROR(...)	      ::Mahakam::Log::GetGameLogger()->error(__VA_ARGS__)
#define MH_FATAL(...)	      ::Mahakam::Log::GetGameLogger()->critical(__VA_ARGS__) 