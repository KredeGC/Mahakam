#include "Mahakam/mhpch.h"
#include "Log.h"

#include "SharedLibrary.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mahakam
{
	Ref<spdlog::logger> Log::s_EngineLogger;
	Ref<spdlog::logger> Log::s_GameLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_EngineLogger = spdlog::stdout_color_mt("Mahakam");
		s_EngineLogger->set_level(spdlog::level::trace);

		s_GameLogger = spdlog::stdout_color_mt("Game");
		s_GameLogger->set_level(spdlog::level::trace);
	}

	void Log::Shutdown()
	{
		s_EngineLogger = nullptr;
		s_GameLogger = nullptr;
	}

	//Ref<spdlog::logger>& Log::GetEngineLogger()
	MH_DEFINE_FUNC(Log::GetEngineLogger, Ref<spdlog::logger>&)
	{
		return s_EngineLogger;
	};

	//Ref<spdlog::logger>& Log::GetGameLogger()
	MH_DEFINE_FUNC(Log::GetGameLogger, Ref<spdlog::logger>&)
	{
		return s_GameLogger;
	};
}