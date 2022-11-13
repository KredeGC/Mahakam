#include "Mahakam/mhpch.h"

#include "YAMLGuard.h"

namespace Mahakam
{
	YAMLGuard::ErrorCallback YAMLGuard::s_Callback;

	void YAMLGuard::Init()
	{
		ryml::set_callbacks(s_Callback.callbacks());
	}

	void YAMLGuard::Shutdown()
	{

	}
}