#pragma once

#include <Mahakam.h>

namespace Mahakam
{
	class SceneHierarchyPanel : public Layer
	{
	private:
		bool open = true;

		Ref<Scene> context;
		Entity selectedEntity;

		void drawEntityNode(Entity entity);

		void drawInspector(Entity entity);

	public:
		SceneHierarchyPanel() = default;

		void setContext(Ref<Scene> scene);

		virtual void onImGuiRender() override;
	};
}