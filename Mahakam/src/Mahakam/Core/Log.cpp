#include "Mahakam/mhpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mahakam
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init(const char* name)
	{
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%T %^%n %16!s::%-16!! :%$ %v");

		s_Logger = std::make_shared<spdlog::logger>(name, consoleSink);
		s_Logger->set_level(spdlog::level::trace);
	}

	void Log::Shutdown()
	{
		s_Logger = nullptr;
	}
    
    void Log::ImportSinks(const std::vector<spdlog::sink_ptr>& sinks)
	{
        auto& logSinks = s_Logger->sinks();
		logSinks = sinks;
	}

	std::shared_ptr<spdlog::logger> Log::GetLogger()
	{
		return s_Logger;
	}
}