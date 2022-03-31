#include "mhpch.h"
#include "Profiler.h"

#include "SharedLibrary.h"

#include "Mahakam/Renderer/GL.h"

#include "Instrumentor.h"

namespace Mahakam
{
	std::vector<Profiler::ProfileResult> Profiler::results;

	void Profiler::Stop()
	{
#ifdef MH_ENABLE_PROFILING
		stopped = true;

		auto endPoint = std::chrono::steady_clock::now();

#ifdef MH_ENABLE_RENDER_PROFILING
		if (flushRenderer)
		{
			GL::FinishRendering();

			auto renderPoint = std::chrono::steady_clock::now();

			AddResult(name, startPoint, renderPoint);
			AddResult("Waiting on GPU", endPoint, renderPoint);
		}
		else
		{
			AddResult(name, startPoint, endPoint);
		}
#else
		AddResult(name, startPoint, endPoint);
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
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch();

		auto duration = end - start;

		results.push_back({ name, start, duration, std::this_thread::get_id() });

		Instrumentor::Get().WriteProfile({ name, start, duration, std::this_thread::get_id() });
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