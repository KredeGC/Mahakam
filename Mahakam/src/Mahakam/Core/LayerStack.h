#pragma once
#include "Core.h"
#include "Layer.h"

#include <vector>

namespace Mahakam
{
	class LayerStack
	{
	private:
		std::vector<Layer*> layers;
		unsigned int layerInsertIndex;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return layers.begin(); }
		std::vector<Layer*>::iterator end() { return layers.end(); }
	};
}