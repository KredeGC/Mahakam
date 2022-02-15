#pragma once

#include <chrono>
#include <vector>

namespace Mahakam
{
	class Profiler
	{
	public:
		struct ProfileResult
		{
			const char* name;
			float duration;
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

		static void ClearResults() { results.clear(); }
		static const std::vector<ProfileResult>& GetResults() { return results; }
	};
}