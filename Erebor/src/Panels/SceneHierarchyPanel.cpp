#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Mahakam
{
	static bool drawVec3Control(const char* label, glm::vec3& value, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		if (ImGui::Button("X", buttonSize))
		{
			changed = true;
			value.x = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, 0.1f))
			changed = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		if (ImGui::Button("Y", buttonSize))
		{
			changed = true;
			value.y = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f))
			changed = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		if (ImGui::Button("Z", buttonSize))
		{
			changed = true;
			value.z = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, 0.1f))
			changed = true;
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((entity == selectedEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			selectedEntity = entity;

		bool markedAsDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				markedAsDeleted = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}

		if (markedAsDeleted)
		{
			if (selectedEntity == entity)
				selectedEntity = {};

			context->destroyEntity(entity);
		}
	}

	template<typename T, typename Fn>
	static void drawComponent(const char* label, Entity entity, Fn func, bool deletable = true)
	{
		if (entity.hasComponent<T>())
		{
			bool markedForDeletion = false;
			if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
					ImGui::OpenPopup(label);

				if (ImGui::BeginPopup(label))
				{
					if (deletable)
					{
						if (ImGui::MenuItem("Delete Component"))
						{
							markedForDeletion = true;
							ImGui::CloseCurrentPopup();
						}
					}

					if (ImGui::MenuItem("Reset"))
					{
						entity.removeComponent<T>();
						entity.addComponent<T>();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				T& component = entity.getComponent<T>();
				func(component);
			}

			if (markedForDeletion)
				entity.removeComponent<T>();
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

		drawComponent<TransformComponent>("Transform", entity, [](TransformComponent& transform)
		{
			glm::vec3 pos = transform.getPosition();
			if (drawVec3Control("Position", pos))
				transform.setPosition(pos);

			// TODO: Fix
			glm::vec3 eulerAngles = glm::degrees(transform.getEulerAngles());
			if (drawVec3Control("Rotation", eulerAngles))
				transform.setEulerangles(glm::radians(eulerAngles));

			glm::vec3 scale = transform.getScale();
			if (drawVec3Control("Scale", scale, 1.0f))
				transform.setScale(scale);
		}, false);

		drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComponent)
		{
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
		});

		drawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& meshComponent)
		{
			Ref<Mesh>& mesh = meshComponent.getMesh();
			Ref<Material>& material = meshComponent.getMaterial();

			if (mesh)
			{
				ImGui::Text("Vertex count: %d", mesh->getVertexCount());
				ImGui::Text("Triangle count: %d", mesh->getIndexCount() * 3);
			}

			if (material)
			{
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// TODO: Temporary
					glm::vec3 color = material->getFloat3("u_Color");
					if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
						material->setFloat3("u_Color", color);
				}
			}
		});
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

			// Blank space menu
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create empty entity"))
					context->createEntity();

				ImGui::EndPopup();
			}

			ImGui::End();


			ImGui::Begin("Inspector");

			if (selectedEntity)
			{
				drawInspector(selectedEntity);

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponent");

				if (ImGui::BeginPopup("AddComponent"))
				{
					if (ImGui::MenuItem("Camera"))
					{
						selectedEntity.addComponent<CameraComponent>();
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Mesh"))
					{
						selectedEntity.addComponent<MeshComponent>();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			ImGui::End();
		}
	}
}