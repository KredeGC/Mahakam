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

		void DrawEntityNode(Entity entity);

		void DrawInspector(Entity entity);

	public:
		SceneHierarchyPanel() = default;

		void SetContext(Ref<Scene> scene);

		virtual void OnImGuiRender() override;
	};
}