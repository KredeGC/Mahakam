#pragma once

#include "Application.h"
#include "FileUtility.h"

#include "Mahakam/Renderer/RendererAPI.h"

#if defined(MH_PLATFORM_WINDOWS) || defined(MH_PLATFORM_LINUX)

extern Mahakam::Application* Mahakam::CreateApplication();

int main(int argc, char** argv)
{
#if defined(MH_PLATFORM_WINDOWS) && defined(MH_RELEASE)
	FreeConsole();
#endif

	if (argc > 0)
	{
		if (argv[0] == "--headless")
		{
			Mahakam::RendererAPI::SetAPI(Mahakam::RendererAPI::API::None);
		}
	}

	Mahakam::Log::Init();
	MH_CORE_INFO("Logging initialized");
	MH_INFO("Logging initialized");

#ifndef MH_STANDALONE
	Mahakam::FileUtility::CreateDirectories("profiling/");
#endif

	MH_PROFILE_BEGIN_SESSION("startup", "profiling/Startup.json");
	auto app = Mahakam::CreateApplication();
	MH_PROFILE_END_SESSION();

	app->Run();

	MH_PROFILE_BEGIN_SESSION("shutdown", "profiling/Shutdown.json");
	delete app;
	MH_PROFILE_END_SESSION();

	Mahakam::Log::Shutdown();
}

#endif