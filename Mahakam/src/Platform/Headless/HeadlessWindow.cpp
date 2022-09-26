#include "Mahakam/mhpch.h"
#include "HeadlessWindow.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	void HeadlessWindow::Init(const WindowProps& props)
	{
		MH_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		MH_CORE_INFO("Creating Headless window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		m_Context = RenderingContext::Create(this, this);
		m_Context->Init();
		SetVSync(false);

        m_Start = std::chrono::steady_clock::now();
	}

	void HeadlessWindow::Shutdown()
	{
		MH_PROFILE_FUNCTION();
	}

	HeadlessWindow::HeadlessWindow(const WindowProps& props)
	{
		Init(props);
	}

	HeadlessWindow::~HeadlessWindow()
	{
		Shutdown();
	}

	void HeadlessWindow::OnUpdate()
	{
		MH_PROFILE_FUNCTION();

		m_Context->SwapBuffers();
	}

	void HeadlessWindow::SetVSync(bool enabled)
	{
		MH_PROFILE_FUNCTION();

		m_Data.VSync = enabled;
	}

	void HeadlessWindow::SetCursorVisible(bool visible)
	{
		MH_PROFILE_FUNCTION();

		m_Data.CursorVisible = visible;
	}
}