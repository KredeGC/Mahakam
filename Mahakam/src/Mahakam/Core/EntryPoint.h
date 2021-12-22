#pragma once

#include "Application.h"

#if defined(MH_PLATFORM_WINDOWS)

extern Mahakam::Application* Mahakam::createApplication();

int main(int argc, char** argv)
{
	Mahakam::Log::init();
	MH_CORE_INFO("Logging initialized");
	MH_INFO("Logging initialized");

	auto app = Mahakam::createApplication();
	app->run();
	delete app;
}

#endif