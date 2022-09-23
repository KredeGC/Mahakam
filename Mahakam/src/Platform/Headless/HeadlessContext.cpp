#include "Mahakam/mhpch.h"
#include "HeadlessContext.h"

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	HeadlessContext::HeadlessContext(void* window, void* proc) : m_Window(window), m_ProcAddress(proc)
	{
		MH_CORE_ASSERT(m_Window, "Window is nullptr!");
	}
	
	void HeadlessContext::Init()
	{
		MH_PROFILE_FUNCTION();

		Reload();
	}

	void HeadlessContext::Reload()
	{
		MH_PROFILE_FUNCTION();
	}
	
	void HeadlessContext::SwapBuffers()
	{
		MH_PROFILE_FUNCTION();
	}
}