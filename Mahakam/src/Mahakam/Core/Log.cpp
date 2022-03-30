#include "mhpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "SharedLibrary.h"

namespace Mahakam
{
	Ref<spdlog::logger> Log::engineLogger;
	Ref<spdlog::logger> Log::gameLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		engineLogger = spdlog::stdout_color_mt("Mahakam");
		engineLogger->set_level(spdlog::level::trace);

		gameLogger = spdlog::stdout_color_mt("Game");
		gameLogger->set_level(spdlog::level::trace);
	}

	Ref<spdlog::logger>& Log::GetEngineLogger()
	{
		MH_OVERRIDE_FUNC(EngineLogger);

		return engineLogger;
	}

	Ref<spdlog::logger>& Log::GetGameLogger()
	{
		MH_OVERRIDE_FUNC(GameLogger);

		return gameLogger;
	}
}