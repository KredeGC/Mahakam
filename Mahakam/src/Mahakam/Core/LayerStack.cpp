#include "mhpch.h"
#include "LayerStack.h"

namespace Mahakam
{
	LayerStack::LayerStack() : layerInsertIndex(0)
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : layers)
			delete layer;
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		layers.emplace(layers.begin() + layerInsertIndex, layer);
		layerInsertIndex++;
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		layers.emplace_back(overlay);
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto iter = std::find(layers.begin(), layers.end(), layer);
		if (iter != layers.end())
		{
			layers.erase(iter);
			layerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay)
	{
		auto iter = std::find(layers.begin(), layers.end(), overlay);
		if (iter != layers.end())
			layers.erase(iter);
	}
}