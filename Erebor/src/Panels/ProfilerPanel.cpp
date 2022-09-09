#include "ebpch.h"
#include "ProfilerPanel.h"

namespace Mahakam::Editor
{
	void ProfilerPanel::OnUpdate(Timestep dt)
	{
		if (m_UpdateHigh)
		{
			m_SmoothDelta = 0.99f * m_SmoothDelta + 0.01f * dt.GetMilliSeconds();

			if (m_SmoothDelta > m_Time)
			{
				m_Time = m_SmoothDelta;

				UpdateProfilerResults();
			}
		}
		else
		{
			m_Time += dt;

			if (m_Time >= 1.0f)
			{
				m_Time -= 1.0f;

				UpdateProfilerResults();
			}
		}
	}

	void ProfilerPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			if (ImGui::Begin("Profiler", &m_Open))
			{
				if (!m_Recording)
				{
					if (ImGui::Button("Record"))
					{
						m_Recording = true;
						uint64_t time = std::chrono::steady_clock::now().time_since_epoch().count();
						MH_PROFILE_BEGIN_SESSION("record", std::string("profiling/Record-") + std::to_string(time) + ".json");
					}
				}
				else
				{
					if (ImGui::Button("Stop"))
					{
						m_Recording = false;
						MH_PROFILE_END_SESSION();
					}
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Update lowend", &m_UpdateHigh))
					m_Time = 0.0f;

				ImGui::PushFont(Application::GetInstance()->GetImGuiLayer()->GetMonoFont());

				const char* format = "%.3fms %3d %s";
				for (auto& result : m_AverageResults)
				{
					float milliseconds = result.ElapsedTime.count() * 0.001f;

					ImGui::Text(format, milliseconds, result.Count, result.Name);
				}

				ImGui::PopFont();
			}

			ImGui::End();
		}
	}

	void ProfilerPanel::UpdateProfilerResults()
	{
		auto& results = Profiler::GetResults();

		m_AverageResults.resize(results.size());
		m_AverageResults = results;

		std::sort(m_AverageResults.begin(), m_AverageResults.end(), [](Profiler::ProfileResult& a, Profiler::ProfileResult& b)
		{
			return (a.Start <= b.Start && (a.Start + a.ElapsedTime) > (b.Start + b.ElapsedTime)) || (a.Start < b.Start);
		});
	}
}