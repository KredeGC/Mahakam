#include "mhpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Mahakam
{
	std::shared_ptr<spdlog::logger> Log::MahakamLogger;
	std::shared_ptr<spdlog::logger> Log::gameLogger;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		MahakamLogger = spdlog::stdout_color_mt("Mahakam");
		MahakamLogger->set_level(spdlog::level::trace);

		gameLogger = spdlog::stdout_color_mt("Game");
		gameLogger->set_level(spdlog::level::trace);
	}
}