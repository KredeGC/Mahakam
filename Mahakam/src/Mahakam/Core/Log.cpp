#include "Mahakam/mhpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Mahakam
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init(const char* name)
	{
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%T %^%n %16!s:%-20!! :%$ %v");

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

	spdlog::source_loc Log::GetLocation(const char* filename, int line, const char* funcname)
	{
		spdlog::source_loc location{ filename, line, funcname };

		size_t funcLength = strlen(funcname);
		for (size_t i = 0; i < funcLength; i++)
		{
			if (funcname[i] == ':')
			{
				location.funcname = funcname + i + 2;
				break;
			}
		}

		return location;
	}

	std::shared_ptr<spdlog::logger> Log::GetLogger()
	{
		return s_Logger;
	}
}