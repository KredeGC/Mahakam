#pragma once

#include "Mahakam/Renderer/RenderingContext.h"

namespace Mahakam
{
	class HeadlessContext : public RenderingContext
	{
	private:
		void* m_Window;
		void* m_ProcAddress;

	public:
		HeadlessContext(void* window, void* proc);

		virtual void Init() override;
		virtual void Reload() override;
		virtual void SwapBuffers() override;
	};
}