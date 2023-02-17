#pragma once

#include "Application.h"
#include "FileUtility.h"

#include "Mahakam/Renderer/RendererAPI.h"

#include <signal.h>

#if defined(MH_PLATFORM_WINDOWS) || defined(MH_PLATFORM_LINUX)

extern Mahakam::Application* Mahakam::CreateApplication();

static Mahakam::Application* g_App;

void intercept_interrupt(int signal)
{
	g_App->Close();
}

int main(int argc, char** argv)
{
	signal(SIGINT, intercept_interrupt);

	Mahakam::Log::Init();
	MH_CORE_INFO("Logging initialized");
	MH_INFO("Logging initialized");

	if (argc > 1)
	{
		if (strcmp(argv[1], "--headless") == 0)
			Mahakam::RendererAPI::SetAPI(Mahakam::RendererAPI::API::None);
	}

#if defined(MH_PLATFORM_WINDOWS) && defined(MH_RELEASE)
	if (Mahakam::RendererAPI::GetAPI() != Mahakam::RendererAPI::API::None)
		FreeConsole();
#endif

#ifdef MH_ENABLE_PROFILING
	Mahakam::FileUtility::CreateDirectories("profiling/");
#endif

	MH_PROFILE_BEGIN_SESSION("startup", "profiling/Startup.json");
	g_App = Mahakam::CreateApplication();
	MH_PROFILE_END_SESSION();

	g_App->Run();

	MH_PROFILE_BEGIN_SESSION("shutdown", "profiling/Shutdown.json");
	delete g_App;
	MH_PROFILE_END_SESSION();

	MH_CORE_INFO("Logging uninitialized");
	MH_INFO("Logging uninitialized");

	Mahakam::Log::Shutdown();

	return 0;
}

#endif