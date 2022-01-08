#pragma once

#include <Mahakam.h>

namespace Mahakam
{
	class SceneHierarchyPanel : public Layer
	{
	private:
		Ref<Scene> context;
		Entity selectedEntity;

		void drawEntityNode(Entity entity);

	public:
		SceneHierarchyPanel() = default;

		void setContext(const Ref<Scene>& scene);

		virtual void onImGuiRender() override;
	};
}