#include "Mahakam/mhpch.h"
#include "ComponentRegistry.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/SkinComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"

#include "Entity.h"
#include "SceneSerializer.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Mahakam/Renderer/Animation.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#define MH_DESERIALIZE_NODE(func, type, name) if (node.has_child(name)) { \
	type value; \
	node[name] >> value; \
	func; }

namespace Mahakam
{
	ComponentRegistry::ComponentMap ComponentRegistry::s_ComponentInterfaces;

	//void ComponentRegistry::RegisterComponent(const std::string& name, const ComponentInterface& componentInterface)
	MH_DEFINE_FUNC(ComponentRegistry::RegisterComponent, void, const std::string& name, const ComponentInterface& componentInterface)
	{
		s_ComponentInterfaces.insert({ name, componentInterface });
	};

	//void ComponentRegistry::DeregisterComponent(const std::string& name)
	MH_DEFINE_FUNC(ComponentRegistry::DeregisterComponent, void, const std::string& name)
	{
		s_ComponentInterfaces.erase(name);
	};

	//const ComponentMap& ComponentRegistry::GetComponents()
	MH_DEFINE_FUNC(ComponentRegistry::GetComponents, const ComponentRegistry::ComponentMap&)
	{
		return s_ComponentInterfaces;
	};

	//void ComponentRegistry::RegisterDefaultComponents()
	MH_DEFINE_FUNC(ComponentRegistry::RegisterDefaultComponents, void)
	{
		ComponentInterface componentInterface;

#pragma region Transform
		// vector icon
		componentInterface.SetEditor(u8"\uf020", [](Entity entity)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			bool noMatrix = transform.HasNoMatrix();
			if (ImGui::Checkbox("Skip matrix update", &noMatrix))
				transform.SetNoMatrix(noMatrix);

			glm::vec3 pos = transform.GetPosition();
			if (GUI::DrawVec3Control("Position", pos, 0.0f))
				transform.SetPosition(pos);

			glm::vec3 eulerAngles = glm::degrees(transform.GetEulerAngles());
			if (GUI::DrawVec3Control("Rotation", eulerAngles, 0.0f))
				transform.SetEulerangles(glm::radians(eulerAngles));

			glm::vec3 scale = transform.GetScale();
			if (GUI::DrawVec3Control("Scale", scale, 1.0f))
				transform.SetScale(scale);
		});
		componentInterface.SetComponent<TransformComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			node["Translation"] << transform.GetPosition();
			node["Rotation"] << transform.GetRotation();
			node["Scale"] << transform.GetScale();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			TransformComponent& transform = entity.AddComponent<TransformComponent>();

			MH_DESERIALIZE_NODE(transform.SetPosition(value), glm::vec3, "Translation");
			MH_DESERIALIZE_NODE(transform.SetRotation(value), glm::quat, "Rotation");
			MH_DESERIALIZE_NODE(transform.SetScale(value), glm::vec3, "Scale");

			return true;
		};

		RegisterComponent("Transform", componentInterface);
#pragma endregion

#pragma region Animator
		// video-clapper icon
		componentInterface.SetEditor(u8"\uecb4", [](Entity entity)
		{
			Animator& animator = entity.GetComponent<AnimatorComponent>();

			Asset<Animation> animation = animator.GetAnimation();

			std::filesystem::path importPath = animation.GetImportPath();
			if (GUI::DrawDragDropField("Animation", ".anim", importPath))
			{
				animation = Asset<Animation>(importPath);
				animator.SetAnimation(Asset<Sound>(importPath));
			}

			if (animation)
			{
				float duration = animation->GetDuration();

				ImGui::TextWrapped("Animation: %s", animation->GetName().c_str());
				ImGui::Text("Duration: %.1fs", duration);

				float progress = animator.GetTime() / animation->GetDuration();
				float realtime = animator.GetTime();

				ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), std::to_string(realtime).c_str());
			}

			bool playOnStart = true;
			ImGui::Checkbox("Play on start", &playOnStart);
		});
		componentInterface.SetComponent<AnimatorComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			Animator& animator = entity.GetComponent<AnimatorComponent>();

			node["Animation"] << animator.GetAnimation().GetID();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			Animator& animator = entity.AddComponent<AnimatorComponent>();

			if (node.has_child("Animation"))
			{
				uint64_t assetID;
				node["Animation"] >> assetID;
				Asset<Animation> animation = Asset<Animation>(assetID);
				if (animation)
				{
					animator.SetAnimation(animation);
				}
			}

			return true;
		};

		RegisterComponent("Animator", componentInterface);
#pragma endregion

#pragma region AudioListener
		// headphone icon
		componentInterface.SetEditor(u8"\uea33");
		componentInterface.SetComponent<AudioListenerComponent>();
		componentInterface.Serialize = nullptr;
		componentInterface.Deserialize = nullptr;

		RegisterComponent("Audio Listener", componentInterface);
#pragma endregion

#pragma region AudioSource
		// audio icon
		componentInterface.SetEditor(u8"\ueea8", [](Entity entity)
		{
			Ref<AudioSource> source = entity.GetComponent<AudioSourceComponent>();
			Asset<Sound> sound = source->GetSound();

			std::filesystem::path importPath = sound.GetImportPath();
			if (GUI::DrawDragDropField("Sound", ".sound", importPath))
			{
				source->SetSound(Asset<Sound>(importPath));
				source->Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
			}

			float spatialBlend = source->GetSpatialBlend();
			if (ImGui::DragFloat("Spatial blend", &spatialBlend, 0.01f, 0.0f, 1.0f))
				source->SetSpatialBlend(spatialBlend);

			bool interpolate = source->GetInterpolation();
			if (ImGui::Checkbox("Interpolate", &interpolate))
				source->SetInterpolation(interpolate);

			if (sound)
			{
				float duration = source->GetDuration();

				ImGui::Text("Duration: %.1fs", duration);

				float progress = source->GetTime() / source->GetDuration();
				float realtime = source->GetTime();

				ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), std::to_string(realtime).c_str());
			}
		});
		componentInterface.SetComponent<AudioSourceComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();

			node["Sound"] << source.GetSound().GetID();
			node["SpatialBlend"] << source.GetSpatialBlend();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			AudioSourceComponent& source = entity.AddComponent<AudioSourceComponent>();

			if (node.has_child("Sound"))
			{
				uint64_t assetID;
				node["Sound"] >> assetID;
				Asset<Sound> sound = Asset<Sound>(assetID);
				if (sound)
				{
					source.SetSound(sound);
					source.Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
				}
			}

			MH_DESERIALIZE_NODE(source.SetSpatialBlend(value), float, "SpatialBlend");

			return true;
		};

		RegisterComponent("Audio Source", componentInterface);
#pragma endregion

#pragma region Camera
		// video icon
		componentInterface.SetEditor(u8"\uecb5", [](Entity entity)
		{
			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = cameraComponent;

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
		componentInterface.SetComponent<CameraComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			node["Projection"] << (int)camera.GetProjectionType();
			node["FOV"] << camera.GetFov();
			node["NearZ"] << camera.GetNearPlane();
			node["FarZ"] << camera.GetFarPlane();
			node["Size"] << camera.GetSize();
			node["Ratio"] << camera.GetRatio();
			node["FixedRatio"] << cameraComponent.HasFixedAspectRatio();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			CameraComponent& cameraComponent = entity.AddComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			MH_DESERIALIZE_NODE(camera.SetProjectionType((Camera::ProjectionType)value), int, "Projection");
			MH_DESERIALIZE_NODE(camera.SetFov(value), float, "FOV");
			MH_DESERIALIZE_NODE(camera.SetNearPlane(value), float, "NearZ");
			MH_DESERIALIZE_NODE(camera.SetFarPlane(value), float, "FarZ");
			MH_DESERIALIZE_NODE(camera.SetSize(value), float, "Size");
			MH_DESERIALIZE_NODE(camera.SetRatio(value), float, "Ratio");
			MH_DESERIALIZE_NODE(cameraComponent.SetFixedAspectRatio(value), float, "FixedRatio");

			return true;
		};

		RegisterComponent("Camera", componentInterface);
#pragma endregion

#pragma region Light
		// light-bulb icon
		componentInterface.SetEditor(u8"\uef6b", [](Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			const char* projectionTypeStrings[] = { "Directional", "Point", "Spot" };
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

			float range = light.GetRange();
			if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, std::numeric_limits<float>::infinity()))
				light.SetRange(range);

			if (light.GetLightType() == Light::LightType::Spot)
			{
				float fov = glm::degrees(light.GetFov());
				if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
					light.SetFov(glm::radians(fov));
			}

			glm::vec3 color = light.GetColor();
			if (GUI::DrawColor3Edit("Color", color, ImGuiColorEditFlags_HDR))
				light.SetColor(color);

			bool hasShadows = light.IsShadowCasting();
			if (ImGui::Checkbox("Shadow casting", &hasShadows))
				light.SetShadowCasting(hasShadows);

			if (hasShadows)
			{
				float bias = light.GetBias();
				if (ImGui::DragFloat("Shadow bias", &bias, 0.001f, 0.0f, 1.0f))
					light.SetBias(bias);
			}

			float scattering = light.GetVolumetricScattering();
			if (ImGui::DragFloat("Volumetric Scattering", &scattering, 0.001f, 0.0f, 1.0f))
				light.SetVolumetricScattering(scattering);

			glm::vec3 scatteringColor = light.GetVolumetricColor();
			if (GUI::DrawColor3Edit("Scattering Color", scatteringColor, ImGuiColorEditFlags_HDR))
				light.SetVolumetricColor(scatteringColor);
		});
		componentInterface.SetComponent<LightComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			node["LightType"] << (int)light.GetLightType();
			node["Range"] << light.GetRange();
			node["FOV"] << light.GetFov();
			node["Color"] << light.GetColor();
			node["ShadowCasting"] << light.IsShadowCasting();
			node["ShadowBias"] << light.GetBias();
			node["VolumetricScattering"] << light.GetVolumetricScattering();
			node["VolumetricColor"] << light.GetVolumetricColor();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			Light& light = entity.AddComponent<LightComponent>();

			MH_DESERIALIZE_NODE(light.SetLightType((Light::LightType)value), int, "LightType");
			MH_DESERIALIZE_NODE(light.SetRange(value), float, "Range");
			MH_DESERIALIZE_NODE(light.SetFov(value), float, "FOV");
			MH_DESERIALIZE_NODE(light.SetColor(value), glm::vec3, "Color");
			MH_DESERIALIZE_NODE(light.SetShadowCasting(value), bool, "ShadowCasting");
			MH_DESERIALIZE_NODE(light.SetBias(value), float, "ShadowBias");
			MH_DESERIALIZE_NODE(light.SetVolumetricScattering(value), float, "VolumetricScattering");
			MH_DESERIALIZE_NODE(light.SetVolumetricColor(value), glm::vec3, "VolumetricColor");

			return true;
		};

		RegisterComponent("Light", componentInterface);
#pragma endregion

#pragma region Mesh
		// cube icon
		componentInterface.SetEditor(u8"\ueef7", [](Entity entity)
		{
			MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();

			// Mesh dragdrop
			std::filesystem::path importPath = meshComponent.GetMesh().GetImportPath();
			if (GUI::DrawDragDropField("Mesh", ".mesh", importPath))
			{
				Asset<Mesh> mesh = Asset<Mesh>(importPath);
				if (mesh)
					meshComponent.SetMesh(mesh);
				else
					meshComponent.SetMesh(nullptr);
			}

			if (!meshComponent.HasMesh()) return;

			auto& meshes = meshComponent.GetSubMeshes();

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
		});
		componentInterface.SetComponent<MeshComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();

			Asset<Mesh> mesh = meshComponent.GetMesh();

			node["Mesh"] << mesh.GetID();

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			if (node.has_child("Mesh"))
			{
				uint64_t meshID;
				node["Mesh"] >> meshID;

				Asset<Mesh> mesh = Asset<Mesh>(meshID);

				MeshComponent& meshComponent = entity.AddComponent<MeshComponent>(mesh);
			}
			else
			{
				MeshComponent& meshComponent = entity.AddComponent<MeshComponent>();
			}

			return true;
		};

		RegisterComponent("Mesh", componentInterface);
#pragma endregion

#pragma region Particle
		// pixels icon
		componentInterface.SetEditor(u8"\uefbe", nullptr);
		componentInterface.SetComponent<ParticleSystemComponent>();
		componentInterface.Serialize = nullptr;
		componentInterface.Deserialize = nullptr;

		RegisterComponent("Particle system", componentInterface);
#pragma endregion

#pragma region Skin
		// male icon
		componentInterface.SetEditor(u8"\uef89", [](Entity entity)
		{
			SkinComponent& skinComponent = entity.GetComponent<SkinComponent>();

			if (MeshComponent* meshComponent = entity.TryGetComponent<MeshComponent>())
			{
				if (meshComponent->HasMesh())
				{
					auto& bones = skinComponent.GetBoneEntities();
					auto& hierarchy = meshComponent->GetNodeHierarchy();

					if (ImGui::Button("Create bone entities"))
					{
						skinComponent.CreateBoneEntities(entity, hierarchy);
					}

					ImGui::SameLine();

					if (ImGui::Button("Clear bone entities"))
					{
						skinComponent.ClearBoneEntities(hierarchy);
					}

					if (bones.size() == hierarchy.size())
					{
						for (size_t i = 0; i < bones.size(); i++)
							GUI::DrawDragDropEntity(hierarchy[i].Name, "Transform", bones[i]);
					}
				}
				else
				{
					ImGui::Text("Skin requires a Mesh");
				}
			}
			else
			{
				ImGui::Text("Skin requires a Mesh Component");
			}
		});
		componentInterface.SetComponent<SkinComponent>();
		componentInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			SkinComponent& skin = entity.GetComponent<SkinComponent>();

			ryml::NodeRef bones = node["Bones"];
			bones |= ryml::SEQ;

			const auto& boneEntities = skin.GetBoneEntities();
			for (auto& boneEntity : boneEntities)
			{
				bones.append_child() << boneEntity.GetHandle();
			}

			return true;
		};
		componentInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			SkinComponent& skin = entity.AddComponent<SkinComponent>();
			auto& boneEntities = skin.GetBoneEntities();

			if (node.has_child("Bones"))
			{
				ryml::NodeRef nodes = node["Bones"];
				boneEntities.reserve(nodes.num_children());
				uint32_t entityID;

				for (auto boneNode : nodes)
				{
					boneNode >> entityID;

					auto entityIter = translation.find(entityID);
					if (entityIter != translation.end())
					{
						Entity translatedEntity{ entityIter->second, entity.GetScene() };
						boneEntities.push_back(translatedEntity);
					}
					else
					{
						boneEntities.push_back({});
					}
				}
			}

			return true;
		};

		RegisterComponent("Skin", componentInterface);
#pragma endregion
	};

	//void ComponentRegistry::DeregisterDefaultComponents()
	MH_DEFINE_FUNC(ComponentRegistry::DeregisterDefaultComponents, void)
	{
		DeregisterComponent("Transform");
		DeregisterComponent("Animator");
		DeregisterComponent("AudioListener");
		DeregisterComponent("AudioSource");
		DeregisterComponent("Camera");
		DeregisterComponent("Light");
		DeregisterComponent("Mesh");
		DeregisterComponent("Skin");
	};
}

#undef MH_DESERIALIZE_NODE