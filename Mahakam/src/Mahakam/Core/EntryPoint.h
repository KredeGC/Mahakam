#pragma once

#include "Application.h"
#include "FileUtility.h"
#include "Instrumentor.h"
#include "Profiler.h"

#include "Mahakam/Renderer/RendererAPI.h"

#include <signal.h>

#if defined(MH_PLATFORM_WINDOWS) || defined(MH_PLATFORM_LINUX)

extern Mahakam::Application* Mahakam::CreateApplication();

static Mahakam::Application* g_App;

void intercept_interrupt(int signal)
{
	g_App->Close();
}

#if defined(MH_PLATFORM_WINDOWS) && defined(MH_STANDALONE)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char** argv)
{
	signal(SIGINT, intercept_interrupt);

	if (argc > 1)
	{
		if (strcmp(argv[1], "--headless") == 0)
			Mahakam::RendererAPI::SetAPI(Mahakam::RendererAPI::API::None);
	}

#ifdef MH_ENABLE_PROFILING
	Mahakam::FileUtility::CreateDirectories("profiling/");

	Mahakam::Profiler::Init();
#endif

	{
		MH_PROFILE_BEGIN_SESSION("startup", "profiling/Startup.json");
		g_App = Mahakam::CreateApplication();
		MH_PROFILE_END_SESSION();

		g_App->Run();

		MH_PROFILE_BEGIN_SESSION("shutdown", "profiling/Shutdown.json");
		delete g_App;
		MH_PROFILE_END_SESSION();
	}

#ifdef MH_ENABLE_PROFILING
	Mahakam::Profiler::Shutdown();
#endif

	return 0;
}

#endif