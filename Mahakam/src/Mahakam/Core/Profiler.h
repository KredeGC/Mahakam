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
			std::string name;

			std::chrono::duration<double, std::micro> start;
			std::chrono::microseconds elapsedTime;
			std::thread::id threadID;
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