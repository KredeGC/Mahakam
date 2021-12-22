#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	class RenderingContext
	{
	public:
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	};
}