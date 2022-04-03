#pragma once
#include <Mahakam.h>

#include "EditorLayer.h"

namespace Mahakam
{
	class Editor : public Application
	{
	private:
		EditorLayer* m_EditorLayer;

	public:
		Editor();
		~Editor();
	};
}