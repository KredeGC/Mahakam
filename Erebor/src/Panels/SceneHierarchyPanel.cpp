#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>

namespace Mahakam
{
	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>().tag;
		
		ImGuiTreeNodeFlags flags = ((entity == selectedEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			selectedEntity = entity;

		if (open)
		{
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene)
	{
		context = scene;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		context->registry.each([&](auto handle)
		{
			Entity entity(handle, context.get());

			drawEntityNode(entity);
		});

		ImGui::End();
	}
}