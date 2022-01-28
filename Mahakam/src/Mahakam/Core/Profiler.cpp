#include "mhpch.h"
#include "Profiler.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
	std::vector<Profiler::ProfileResult> Profiler::results;

	void Profiler::addResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime)
	{
#ifdef MH_DEBUG
		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

		float duration = (end - start) * 0.001f;

		results.push_back({ name, duration });
#endif
	}

	void Profiler::stop()
	{
#ifdef MH_DEBUG
		auto endPoint = std::chrono::high_resolution_clock::now();

		addResult(name, startPoint, endPoint);

		stopped = true;

		if (flushRenderer)
		{
			GL::finishRendering();

			auto renderPoint = std::chrono::high_resolution_clock::now();

			addResult("Waiting on GPU", endPoint, renderPoint);
		}
#endif
	}
}