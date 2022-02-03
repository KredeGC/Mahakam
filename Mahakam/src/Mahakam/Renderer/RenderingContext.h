#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}