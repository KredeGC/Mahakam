#include "Mahakam/mhpch.h"
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

	void Profiler::AddResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime)
	{
#ifdef MH_ENABLE_PROFILING
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch();

		auto duration = end - start;

		ProfileResult result = { name, 1, start, duration, std::this_thread::get_id() };

		Instrumentor::Get().WriteProfile(result);

		auto iter = results.rbegin();
		if (iter != results.rend() && iter->Name == name)
		{
			iter->Count++;
			iter->ElapsedTime += duration;
		}
		else
		{
			results.push_back(result);
		}
#endif
	}

	void Profiler::ClearResults()
	{
		results.clear();
	}

	const std::vector<Profiler::ProfileResult>& Profiler::GetResults()
	{
		return results;
	}
}