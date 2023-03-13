#include "Mahakam/mhpch.h"
#include "Profiler.h"

#include "Instrumentor.h"
#include "SharedLibrary.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
#ifdef MH_ENABLE_PROFILING
	Profiler::~Profiler()
	{
		if (!s_ResultsFwd)
			return;

		if (m_Stopped)
			return;

		m_Stopped = true;

		auto endPoint = std::chrono::steady_clock::now();

#ifdef MH_ENABLE_RENDER_PROFILING
		if (m_FlushRenderer)
		{
			GL::FinishRendering();

			auto renderPoint = std::chrono::steady_clock::now();

			AddResult(m_Name, m_StartPoint, renderPoint);
			AddResult("Waiting on GPU", endPoint, renderPoint);
		}
		else
#endif
		{
			AddResult(m_Name, m_StartPoint, endPoint);
		}
	}

	void Profiler::AddResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime)
	{
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch();

		auto duration = end - start;

		ProfileResult result = { name, 1, start, duration, std::this_thread::get_id() };

		Instrumentor::Get().WriteProfile(result);

		auto iter = s_ResultsBck->rbegin();
		if (iter != s_ResultsBck->rend() && strcmp(iter->Name, name) == 0)
		{
			iter->Count++;
			iter->ElapsedTime += duration;
		}
		else
		{
			s_ResultsBck->push_back(result);
		}
	}
#endif

	void Profiler::ClearResults()
	{
#ifdef MH_ENABLE_PROFILING
		if (!s_ResultsFwd)
			return;

		s_ResultsFwd->resize(s_ResultsBck->size());

		*s_ResultsFwd = *s_ResultsBck;

		s_ResultsBck->clear();
#endif
	}

	const Profiler::ProfileVector* Profiler::GetResults()
	{
		return s_ResultsFwd;
	}

	void Profiler::Init()
	{
#ifdef MH_ENABLE_PROFILING
		s_ResultsFwd = Allocator::New<ProfileVector>(Allocator::GetAllocator<ProfileResult>());
		s_ResultsBck = Allocator::New<ProfileVector>(Allocator::GetAllocator<ProfileResult>());
#endif
	}

	void Profiler::Shutdown()
	{
#ifdef MH_ENABLE_PROFILING
		Allocator::Delete(s_ResultsFwd);
		Allocator::Delete(s_ResultsBck);
#endif
	}
}