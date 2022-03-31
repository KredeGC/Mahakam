#pragma once

#include "Application.h"

#if defined(MH_PLATFORM_WINDOWS) || defined(MH_PLATFORM_LINUX)

extern Mahakam::Application* Mahakam::CreateApplication();

int main(int argc, char** argv)
{
	Mahakam::Log::Init();
	MH_CORE_INFO("Logging initialized");
	MH_INFO("Logging initialized");

	MH_PROFILE_BEGIN_SESSION("startup", "Profiling-Startup.json");
	auto app = Mahakam::CreateApplication();
	MH_PROFILE_END_SESSION();

	app->Run();

	MH_PROFILE_BEGIN_SESSION("shutdown", "Profiling-Shutdown.json");
	delete app;
	MH_PROFILE_END_SESSION();
}

#endif