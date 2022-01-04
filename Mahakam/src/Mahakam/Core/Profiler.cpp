#include "mhpch.h"
#include "Profiler.h"

namespace Mahakam
{
	std::vector<Profiler::ProfileResult> Profiler::results;

	void Profiler::stop()
	{
		auto endPoint = std::chrono::high_resolution_clock::now();

		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(startPoint).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

		stopped = true;

		float duration = (end - start) * 0.001f;

		results.push_back({ name, duration });
	}
}