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
		static std::vector<ProfileResult> results;

	public:
		Profiler(const char* name)
			: name(name), stopped(false)
		{
			startPoint = std::chrono::high_resolution_clock::now();
		}

		~Profiler()
		{
			if (!stopped)
				stop();
		}

		void stop();

		static void clearResults() { results.clear(); }
		static const std::vector<ProfileResult>& getResults() { return results; }
	};
}