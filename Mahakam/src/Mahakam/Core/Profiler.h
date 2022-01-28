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
		std::chrono::time_point<std::chrono::steady_clock> startPoint;
		bool stopped;
		bool flushRenderer;
		static std::vector<ProfileResult> results;

	public:
		Profiler(const char* name, bool flushRenderer)
			: name(name), stopped(false), flushRenderer(flushRenderer)
		{
#ifdef MH_DEBUG
			startPoint = std::chrono::high_resolution_clock::now();
#endif
		}

#ifdef MH_DEBUG
		~Profiler()
		{
			if (!stopped)
				stop();
		}
#endif

		void stop();

		static void addResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime);

		static void clearResults() { results.clear(); }
		static const std::vector<ProfileResult>& getResults() { return results; }
	};
}