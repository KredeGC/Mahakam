#pragma once
#include <Mahakam/Mahakam.h>

#include "ImporterLayer.h"

namespace Mahakam::Editor
{
	class EreborApplication : public Application
	{
	private:
		ImporterLayer* m_ImporterLayer;

	public:
		EreborApplication();
		~EreborApplication();
	};
}