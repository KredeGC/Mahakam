#include "mhpch.h"
#include "Profiler.h"

#include "SharedLibrary.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
	std::vector<Profiler::ProfileResult> Profiler::results;

	void Profiler::Stop()
	{
#ifdef MH_ENABLE_PROFILING
		auto endPoint = std::chrono::steady_clock::now();

		AddResult(name, startPoint, endPoint);

		stopped = true;

#ifdef MH_ENABLE_RENDER_PROFILING
		if (flushRenderer)
		{
			GL::FinishRendering();

			auto renderPoint = std::chrono::steady_clock::now();

			AddResult("Waiting on GPU", endPoint, renderPoint);
		}
#endif
#endif
	}

	Profiler Profiler::Create(const char* name, bool flushRenderer)
	{
		MH_OVERRIDE_FUNC(ProfilerCreate, name, flushRenderer);

		return Profiler(name, flushRenderer);
	}

	void Profiler::AddResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime)
	{
		MH_OVERRIDE_FUNC(ProfilerAddResult, name, startTime, endTime);

#ifdef MH_ENABLE_PROFILING
		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

		float duration = (end - start) * 0.001f;

		results.push_back({ name, duration });
#endif
	}

	void Profiler::ClearResults()
	{
		MH_OVERRIDE_FUNC(ProfilerClear);

		results.clear();
	}

	const std::vector<Profiler::ProfileResult>& Profiler::GetResults()
	{
		MH_OVERRIDE_FUNC(ProfilerGetResults);

		return results;
	}
}