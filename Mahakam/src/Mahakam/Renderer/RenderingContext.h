#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void Reload() = 0;
		virtual void SwapBuffers() = 0;

		static RenderingContext* Create(void* window, void* proc);
	};
}