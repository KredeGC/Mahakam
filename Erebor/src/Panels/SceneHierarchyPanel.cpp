#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Mahakam
{
	static bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue = 0.0f, float columnWidth = 100.0f)
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

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((entity == selectedEntity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

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

			context->DestroyEntity(entity);
		}
	}

	template<typename T, typename Fn>
	static void DrawComponent(const char* label, Entity entity, Fn func, bool deletable = true)
	{
		if (entity.HasComponent<T>())
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
						entity.RemoveComponent<T>();
						entity.AddComponent<T>();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				T& component = entity.GetComponent<T>();
				func(component);
			}

			if (markedForDeletion)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawInspector(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				selectedEntity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Mesh"))
			{
				selectedEntity.AddComponent<MeshComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& transform)
		{
			glm::vec3 pos = transform.GetPosition();
			if (DrawVec3Control("Position", pos))
				transform.SetPosition(pos);

			// TODO: Fix
			glm::vec3 eulerAngles = glm::degrees(transform.GetEulerAngles());
			if (DrawVec3Control("Rotation", eulerAngles))
				transform.SetEulerangles(glm::radians(eulerAngles));

			glm::vec3 scale = transform.GetScale();
			if (DrawVec3Control("Scale", scale, 1.0f))
				transform.SetScale(scale);
		}, false);

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cameraComponent)
		{
			Camera& camera = cameraComponent.GetCamera();

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool selected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], selected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((Camera::ProjectionType)i);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
			{
				float fov = glm::degrees(camera.GetFov());
				if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
					camera.SetFov(glm::radians(fov));
			}
			else
			{
				float size = camera.GetSize();
				if (ImGui::DragFloat("Size", &size, 0.1f, 0.0f))
					camera.SetSize(size);
			}

			float nearClip = camera.GetNearPlane();
			if (ImGui::DragFloat("Near clip-plane", &nearClip, 0.1f, 0.0f))
				camera.SetNearPlane(nearClip);

			float farClip = camera.GetFarPlane();
			if (ImGui::DragFloat("Far clip-plane", &farClip, 0.1f, 0.0f))
				camera.SetFarPlane(farClip);

			bool fixedAspectRatio = cameraComponent.HasFixedAspectRatio();
			if (ImGui::Checkbox("Fixed aspect ratio", &fixedAspectRatio))
				cameraComponent.SetFixedAspectRatio(fixedAspectRatio);
		});

		DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& meshComponent)
		{
			auto& meshes = meshComponent.GetMeshes();
			Ref<Material> material = meshComponent.GetMaterial();

			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			for (auto& mesh : meshes)
			{
				if (mesh)
				{
					vertexCount += mesh->GetVertexCount();
					indexCount += mesh->GetIndexCount();
				}
			}

			if (vertexCount > 0 && indexCount > 0)
			{
				ImGui::Text("Vertex count: %d", vertexCount);
				ImGui::Text("Triangle count: %d", indexCount / 3);
			}

			if (material)
			{
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// TODO: Temporary
					glm::vec3 color = material->GetFloat3("u_Color");
					if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
						material->SetFloat3("u_Color", color);
				}
			}
		});

		DrawComponent<LightComponent>("Light", entity, [](LightComponent& lightComponent)
		{
			Light& light = lightComponent.GetLight();

			const char* projectionTypeStrings[] = { "Directional", "Point", "Spot"};
			const char* currentProjectionTypeString = projectionTypeStrings[(int)light.GetLightType()];

			if (ImGui::BeginCombo("Light Type", currentProjectionTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool selected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], selected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						light.SetLightType((Light::LightType)i);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (light.GetLightType() == Light::LightType::Point || light.GetLightType() == Light::LightType::Spot)
			{
				float range = light.GetRange();
				if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 180.0f))
					light.SetRange(range);
			}

			if (light.GetLightType() == Light::LightType::Spot)
			{
				float fov = glm::degrees(light.GetFov());
				if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
					light.SetFov(glm::radians(fov));
			}

			bool hasShadows = light.IsShadowCasting();
			if (ImGui::Checkbox("Shadow casting", &hasShadows))
				light.SetShadowCasting(hasShadows);
		});
	}

	void SceneHierarchyPanel::SetContext(Ref<Scene> scene)
	{
		context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Scene Hierarchy", &open);

			context->registry.each([&](auto handle)
			{
				Entity entity(handle, context.get());

				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				selectedEntity = {};

			// Blank space menu
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create empty entity"))
					context->CreateEntity();

				ImGui::EndPopup();
			}

			ImGui::End();


			ImGui::Begin("Inspector");

			if (selectedEntity)
			{
				DrawInspector(selectedEntity);
			}

			ImGui::End();
		}
	}
}