#pragma once
#include "Core.h"

#include <vector>

namespace Mahakam
{
	class Layer;

	class LayerStack
	{
	private:
		std::vector<Layer*> m_Layers;
		uint32_t m_LayerInsertIndex;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	};
}