#pragma once

#include "Core.h"

#include <chrono>
#include <vector>
#include <thread>

namespace Mahakam
{
	class Profiler
	{
	public:
		struct ProfileResult
		{
			std::string Name;
			uint16_t Count;

			std::chrono::duration<double, std::micro> Start;
			std::chrono::microseconds ElapsedTime;
			std::thread::id ThreadID;
		};

	private:
		const char* name;
		std::chrono::steady_clock::time_point startPoint;
		bool stopped;
		bool flushRenderer;
		static std::vector<ProfileResult> results;

	public:
		Profiler(const char* name, bool flushRenderer)
			: name(name), stopped(false), flushRenderer(flushRenderer)
		{
#ifdef MH_ENABLE_PROFILING
			startPoint = std::chrono::steady_clock::now();
#endif
		}

#ifdef MH_ENABLE_PROFILING
		~Profiler()
		{
			if (!stopped)
				Stop();
		}
#endif

		void Stop();

		static void AddResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime);

		static void ClearResults();
		static const std::vector<ProfileResult>& GetResults();
	};
}