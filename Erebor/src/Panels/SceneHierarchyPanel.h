#pragma once

#include <Mahakam.h>

namespace Mahakam
{
	class SceneHierarchyPanel : public Layer
	{
	private:
		bool m_Open = true;

	public:
		SceneHierarchyPanel() = default;

		virtual void OnImGuiRender() override;

	private:
		void DrawEntityNode(Entity entity, Ref<Scene> context);

		void DrawInspector(Entity entity);
	};
}