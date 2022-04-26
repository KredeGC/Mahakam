#pragma once
#include <Mahakam.h>

#include "EditorLayer.h"

namespace Mahakam::Editor
{
	class EditorApplication : public Application
	{
	private:
		EditorLayer* m_EditorLayer;

	public:
		EditorApplication();
		~EditorApplication();
	};
}