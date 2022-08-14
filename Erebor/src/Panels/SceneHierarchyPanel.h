#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class SceneHierarchyPanel : EditorWindow
	{
	private:
		bool m_Open = true;

	public:
		SceneHierarchyPanel() = default;

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

	private:
		void DrawEntityNode(Entity entity, Ref<Scene> context);

		void DrawInspector(Entity entity);
	};
}