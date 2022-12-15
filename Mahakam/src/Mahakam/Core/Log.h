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
		static Ref<spdlog::logger> s_EngineLogger;
		static Ref<spdlog::logger> s_GameLogger;

	public:
		static void Init();
		static void Shutdown();

		MH_DECLARE_FUNC(GetEngineLogger, Ref<spdlog::logger>&);
		MH_DECLARE_FUNC(GetGameLogger, Ref<spdlog::logger>&);
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