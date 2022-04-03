#pragma once

#include <Mahakam.h>

namespace Mahakam
{
	class SceneHierarchyPanel : public Layer
	{
	private:
		bool open = true;

		WeakRef<Scene> scene;

	public:
		SceneHierarchyPanel() = default;

		void SetContext(WeakRef<Scene> scene);

		virtual void OnImGuiRender() override;

	private:
		void DrawEntityNode(Entity entity, Ref<Scene> context);

		void DrawInspector(Entity entity);
	};
}