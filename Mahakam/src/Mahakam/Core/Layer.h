#pragma once

#include "Core.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Events/Event.h"

namespace Mahakam
{
	class Layer
	{
	protected:
		std::string debugName;

	public:
		Layer(const std::string& name = "Layer") : debugName(name) {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep dt) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return debugName; }
	};
}