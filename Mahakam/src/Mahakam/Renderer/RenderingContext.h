#pragma once

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