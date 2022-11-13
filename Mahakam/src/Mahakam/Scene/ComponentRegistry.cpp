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

#include "Mahakam/Core/SharedLibrary.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#define MH_DESERIALIZE_NODE(type, variable, name) type variable; \
	if (node.has_child(name)) node[name] >> variable;

namespace Mahakam
{
	ComponentRegistry::ComponentMap ComponentRegistry::s_ComponentInterfaces;

	//void ComponentRegistry::RegisterComponent(const std::string& name, ComponentInterface componentInterface)
	MH_DEFINE_FUNC(ComponentRegistry::RegisterComponent, void, const std::string& name, ComponentInterface componentInterface)
	{
		s_ComponentInterfaces[name] = componentInterface;
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
#pragma region Transform
		ComponentInterface transformInterface;
		transformInterface.Icon = u8"\uf020"; // vector
		transformInterface.SetComponent<TransformComponent>();
		transformInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			node["Translation"] << transform.GetPosition();
			node["Rotation"] << transform.GetRotation();
			node["Scale"] << transform.GetScale();

			return true;
		};
		transformInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			TransformComponent& transform = entity.AddComponent<TransformComponent>();

			MH_DESERIALIZE_NODE(glm::vec3, pos, "Translation");
			transform.SetPosition(pos);

			MH_DESERIALIZE_NODE(glm::quat, rot, "Rotation");
			transform.SetRotation(rot);

			MH_DESERIALIZE_NODE(glm::vec3, scale, "Scale");
			transform.SetScale(scale);

			return true;
		};

		RegisterComponent("Transform", transformInterface);
#pragma endregion

#pragma region Animator
		ComponentInterface animatorInterface;
		animatorInterface.Icon = u8"\uecb4"; // video-clapper
		animatorInterface.SetComponent<AnimatorComponent>();

		RegisterComponent("Animator", animatorInterface);
#pragma endregion

#pragma region AudioListener
		ComponentInterface audioListenerInterface;
		audioListenerInterface.Icon = u8"\uea33"; // headphone
		audioListenerInterface.SetComponent<AudioListenerComponent>();

		RegisterComponent("Audio Listener", audioListenerInterface);
#pragma endregion

#pragma region AudioSource
		ComponentInterface audioSourceInterface;
		audioSourceInterface.Icon = u8"\ueea8"; // audio
		audioSourceInterface.SetComponent<AudioSourceComponent>();
		audioSourceInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();

			node["Sound"] << source.GetSound().GetID();
			node["SpatialBlend"] << source.GetSpatialBlend();

			return true;
		};
		audioSourceInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			AudioSourceComponent& source = entity.AddComponent<AudioSourceComponent>();

			MH_DESERIALIZE_NODE(uint64_t, assetID, "Sound");
			Asset<Sound> sound = Asset<Sound>(assetID);
			if (sound)
			{
				source.SetSound(sound);
				source.Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
			}

			MH_DESERIALIZE_NODE(float, blend, "SpatialBlend");
			source.SetSpatialBlend(blend);

			return true;
		};

		RegisterComponent("Audio Source", audioSourceInterface);
#pragma endregion

#pragma region Camera
		ComponentInterface cameraInterface;
		cameraInterface.Icon = u8"\uecb5"; // video
		cameraInterface.SetComponent<CameraComponent>();
		cameraInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
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
		cameraInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			CameraComponent& cameraComponent = entity.AddComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			MH_DESERIALIZE_NODE(int, projection, "Projection");
			camera.SetProjectionType((Camera::ProjectionType)projection);

			MH_DESERIALIZE_NODE(float, fov, "FOV");
			camera.SetFov(fov);

			MH_DESERIALIZE_NODE(float, nearZ, "NearZ");
			camera.SetNearPlane(nearZ);

			MH_DESERIALIZE_NODE(float, farZ, "FarZ");
			camera.SetFarPlane(farZ);

			MH_DESERIALIZE_NODE(float, size, "Size");
			camera.SetSize(size);

			MH_DESERIALIZE_NODE(float, ratio, "Ratio");
			camera.SetRatio(ratio);

			MH_DESERIALIZE_NODE(float, fixed, "FixedRatio");
			cameraComponent.SetFixedAspectRatio(ratio);

			return true;
		};

		RegisterComponent("Camera", cameraInterface);
#pragma endregion

#pragma region Light
		ComponentInterface lightInterface;
		lightInterface.Icon = u8"\uef6b"; // light-bulb
		lightInterface.SetComponent<LightComponent>();
		lightInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			node["LightType"] << (int)light.GetLightType();
			node["Range"] << light.GetRange();
			node["FOV"] << light.GetFov();
			node["Color"] << light.GetColor();
			node["ShadowCasting"] << light.IsShadowCasting();
			node["ShadowBias"] << light.GetBias();

			return true;
		};
		lightInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			Light& light = entity.AddComponent<LightComponent>();

			MH_DESERIALIZE_NODE(int, type, "LightType");
			light.SetLightType((Light::LightType)type);

			MH_DESERIALIZE_NODE(float, range, "Range");
			light.SetRange(range);

			MH_DESERIALIZE_NODE(float, fov, "FOV");
			light.SetFov(fov);

			MH_DESERIALIZE_NODE(glm::vec3, color, "Color");
			light.SetColor(color);

			MH_DESERIALIZE_NODE(bool, shadowCasting, "ShadowCasting");
			light.SetShadowCasting(shadowCasting);

			MH_DESERIALIZE_NODE(float, shadowBias, "ShadowBias");
			light.SetBias(shadowBias);

			return true;
		};

		RegisterComponent("Light", lightInterface);
#pragma endregion

#pragma region Mesh
		ComponentInterface meshInterface;
		meshInterface.Icon = u8"\ueef7"; // cube
		meshInterface.SetComponent<MeshComponent>();
		meshInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();

			Asset<Mesh> mesh = meshComponent.GetMesh();

			node["Mesh"] << mesh.GetID();

			return true;
		};
		meshInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
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

		RegisterComponent("Mesh", meshInterface);
#pragma endregion

#pragma region Particle
		ComponentInterface particleInterface;
		particleInterface.Icon = u8"\uefbe"; // pixels
		particleInterface.SetComponent<ParticleSystemComponent>();

		RegisterComponent("Particle system", particleInterface);
#pragma endregion

#pragma region Skin
		ComponentInterface skinInterface;
		skinInterface.Icon = u8"\uef89"; // male
		skinInterface.SetComponent<SkinComponent>();
		skinInterface.Serialize = [](ryml::NodeRef& node, Entity entity)
		{
			SkinComponent& skin = entity.GetComponent<SkinComponent>();

			ryml::NodeRef bones = node["Bones"];
			bones |= ryml::SEQ;

			const std::vector<Entity>& boneEntities = skin.GetBoneEntities();
			for (auto& boneEntity : boneEntities)
			{
				bones.append_child() << uint32_t(boneEntity);
			}

			return true;
		};
		skinInterface.Deserialize = [](ryml::NodeRef& node, SceneSerializer::EntityMap& translation, Entity entity)
		{
			SkinComponent& skin = entity.AddComponent<SkinComponent>();
			std::vector<Entity>& boneEntities = skin.GetBoneEntities();

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
						Entity translatedEntity{ entityIter->second, entity };
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

		RegisterComponent("Skin", skinInterface);
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