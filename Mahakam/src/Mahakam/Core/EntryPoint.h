#pragma once

#include "Application.h"

#if defined(MH_PLATFORM_WINDOWS) || defined(MH_PLATFORM_LINUX)

extern Mahakam::Application* Mahakam::CreateApplication();

int main(int argc, char** argv)
{
	Mahakam::Log::Init();
	MH_CORE_INFO("Logging initialized");
	MH_INFO("Logging initialized");

	auto app = Mahakam::CreateApplication();
	app->Run();
	delete app;
}

#endif