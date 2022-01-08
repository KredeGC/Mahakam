#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

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

	void SceneHierarchyPanel::drawInspector(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			std::string& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				TransformComponent& transform = entity.getComponent<TransformComponent>();

				glm::vec3 pos = transform.getPosition();
				if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f))
					transform.setPosition(pos);

				glm::quat rot = transform.getRotation();
				if (ImGui::DragFloat4("Rotation", glm::value_ptr(rot), 0.1f))
					transform.setRotation(glm::normalize(rot));

				glm::vec3 scale = transform.getScale();
				if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f))
					transform.setScale(scale);
			}
		}

		if (entity.hasComponent<CameraComponent>())
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				CameraComponent& cameraComponent = entity.getComponent<CameraComponent>();
				Camera& camera = cameraComponent.getCamera();

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool selected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], selected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.setProjectionType((Camera::ProjectionType)i);
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == Camera::ProjectionType::Perspective)
				{
					float fov = glm::degrees(camera.getFov());
					if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
						camera.setFov(glm::radians(fov));
				}
				else
				{
					float size = camera.getSize();
					if (ImGui::DragFloat("Size", &size, 0.1f, 0.0f))
						camera.setSize(size);
				}

				float nearClip = camera.getNearPlane();
				if (ImGui::DragFloat("Near clip-plane", &nearClip, 0.1f, 0.0f))
					camera.setNearPlane(nearClip);

				float farClip = camera.getFarPlane();
				if (ImGui::DragFloat("Far clip-plane", &farClip, 0.1f, 0.0f))
					camera.setFarPlane(farClip);

				bool fixedAspectRatio = cameraComponent.hasFixedAspectRatio();
				if (ImGui::Checkbox("Fixed aspect ratio", &fixedAspectRatio))
					cameraComponent.setFixedAspectRatio(fixedAspectRatio);
			}
		}

		if (entity.hasComponent<MeshComponent>())
		{
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			{
				MeshComponent& meshComponent = entity.getComponent<MeshComponent>();
				Ref<Mesh>& mesh = meshComponent.getMesh();
				Ref<Material>& material = meshComponent.getMaterial();

				ImGui::Text("Vertex count: %d", mesh->getVertexCount());
				ImGui::Text("Triangle count: %d", mesh->getIndexCount() * 3);

				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// TODO: Temporary
					glm::vec3 color = material->getFloat3("u_Color");
					if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
						material->setFloat3("u_Color", color);
				}
			}
		}
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene)
	{
		context = scene;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Scene Hierarchy", &open);

			context->registry.each([&](auto handle)
			{
				Entity entity(handle, context.get());

				drawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				selectedEntity = {};

			ImGui::End();

			ImGui::Begin("Inspector");

			if (selectedEntity)
				drawInspector(selectedEntity);

			ImGui::End();
		}
	}
}