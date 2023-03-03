#include "Mahakam/mhpch.h"
#include "Instrumentor.h"

#include "Log.h"

#include <spdlog/spdlog.h>

namespace Mahakam
{
	void Instrumentor::BeginSession(const std::string& name, const std::string& filepath)
	{
		std::lock_guard lock(m_Mutex);
		if (m_CurrentSession)
		{
			// If there is already a current session, then close it before beginning new one.
			// Subsequent profiling output meant for the original session will end up in the
			// newly opened session instead.  That's better than having badly formatted
			// profiling output.
			if (Log::GetLogger()) // Edge case: BeginSession() might be before Log::Init()
				MH_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
			InternalEndSession();
		}
		m_OutputStream.open(filepath);

		if (m_OutputStream.is_open())
		{
			m_CurrentSession = new InstrumentationSession({ name });
			WriteHeader();
		}
		else
		{
			if (Log::GetLogger()) // Edge case: BeginSession() might be before Log::Init()
				MH_ERROR("Instrumentor could not open results file '{0}'.", filepath);
		}
	}

	void Instrumentor::EndSession()
	{
		std::lock_guard lock(m_Mutex);
		InternalEndSession();
	}
}