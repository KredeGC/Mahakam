#pragma once

#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Events/Event.h"

namespace Mahakam::Editor
{
	class EditorWindow
	{
	public:
		virtual ~EditorWindow() = default;

		virtual bool IsOpen() const = 0;

		virtual void OnUpdate(Timestep dt) {}

		virtual bool OnEvent(Event& ev) { return false; }

		virtual void OnImGuiRender() = 0;
	};
}