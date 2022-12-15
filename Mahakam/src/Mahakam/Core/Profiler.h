#pragma once
#include "Core.h"
#include "Allocator.h"
#include "Types.h"

#include <chrono>
#include <thread>

namespace Mahakam
{
	class Profiler
	{
	public:
		struct ProfileResult
		{
			const char* Name;
			uint16_t Count;

			std::chrono::duration<double, std::micro> Start;
			std::chrono::microseconds ElapsedTime;
			std::thread::id ThreadID;
		};

		using ProfileVector = TrivialVector<ProfileResult, Allocator::BaseAllocator<ProfileResult>>;

	private:
		const char* m_Name;
		bool m_FlushRenderer;
		bool m_Stopped;
		std::chrono::steady_clock::time_point m_StartPoint;
		static ProfileVector s_ResultsFwd;
		static ProfileVector s_ResultsBck;

	public:
		Profiler(const char* name, bool flushRenderer)
			: m_Name(name), m_FlushRenderer(flushRenderer), m_Stopped(false)
		{
#ifdef MH_ENABLE_PROFILING
			m_StartPoint = std::chrono::steady_clock::now();
#endif
		}

#ifdef MH_ENABLE_PROFILING
		~Profiler();

		static void AddResult(const char* name, std::chrono::time_point<std::chrono::steady_clock> startTime, std::chrono::time_point<std::chrono::steady_clock> endTime);
#endif

		static void ClearResults();
		static const ProfileVector& GetResults();
	};
}