#include "mhpch.h"
#include "ComponentRegistry.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Mahakam/Math/Math.h"

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
		transformInterface.SetComponent<TransformComponent>();
		transformInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			emitter << YAML::Key << "Translation" << YAML::Value << transform.GetPosition();
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.GetRotation();
			emitter << YAML::Key << "Scale" << YAML::Value << transform.GetScale();
			emitter << YAML::Key << "Static" << YAML::Value << transform.IsStatic();

			return true;
		};
		transformInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			TransformComponent& transform = entity.AddComponent<TransformComponent>();

			transform.SetPosition(node["Translation"].as<glm::vec3>());
			transform.SetRotation(node["Rotation"].as<glm::quat>());
			transform.SetScale(node["Scale"].as<glm::vec3>());
			transform.SetStatic(node["Static"].as<bool>());

			return true;
		};

		RegisterComponent("Transform", transformInterface);
#pragma endregion

#pragma region Animator
		ComponentInterface animatorInterface;
		animatorInterface.SetComponent<AnimatorComponent>();

		RegisterComponent("Animator", animatorInterface);
#pragma endregion

#pragma region AudioListener
		ComponentInterface audioListenerInterface;
		audioListenerInterface.SetComponent<AudioListenerComponent>();

		RegisterComponent("Audio Listener", audioListenerInterface);
#pragma endregion

#pragma region AudioSource
		ComponentInterface audioSourceInterface;
		audioSourceInterface.SetComponent<AudioSourceComponent>();
		audioSourceInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();

			emitter << YAML::Key << "Sound" << YAML::Value << source.GetSound().GetID();
			emitter << YAML::Key << "SpatialBlend" << YAML::Value << source.GetSpatialBlend();

			return true;
		};
		audioSourceInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			AudioSourceComponent& source = entity.AddComponent<AudioSourceComponent>();

			YAML::Node soundNode = node["Sound"];
			if (soundNode)
			{
				Asset<Sound> sound = Asset<Sound>(soundNode.as<uint64_t>());

				if (sound)
				{
					source.SetSound(sound);
					source.Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
				}
			}

			YAML::Node spatialNode = node["SpatialBlend"];
			if (spatialNode)
				source.SetSpatialBlend(spatialNode.as<float>());

			return true;
		};

		RegisterComponent("Audio Source", audioSourceInterface);
#pragma endregion

#pragma region Camera
		ComponentInterface cameraInterface;
		cameraInterface.SetComponent<CameraComponent>();
		cameraInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			emitter << YAML::Key << "Projection" << YAML::Value << (int)camera.GetProjectionType();
			emitter << YAML::Key << "FOV" << YAML::Value << camera.GetFov();
			emitter << YAML::Key << "NearZ" << YAML::Value << camera.GetNearPlane();
			emitter << YAML::Key << "FarZ" << YAML::Value << camera.GetFarPlane();
			emitter << YAML::Key << "Size" << YAML::Value << camera.GetSize();
			emitter << YAML::Key << "Ratio" << YAML::Value << camera.GetRatio();
			emitter << YAML::Key << "FixedRatio" << YAML::Value << cameraComponent.HasFixedAspectRatio();

			return true;
		};
		cameraInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			CameraComponent& cameraComponent = entity.AddComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			camera.SetProjectionType((Camera::ProjectionType)node["Projection"].as<int>());
			camera.SetFov(node["FOV"].as<float>());
			camera.SetNearPlane(node["NearZ"].as<float>());
			camera.SetFarPlane(node["FarZ"].as<float>());
			camera.SetSize(node["Size"].as<float>());
			camera.SetRatio(node["Ratio"].as<float>());
			cameraComponent.SetFixedAspectRatio(node["FixedRatio"].as<bool>());

			return true;
		};

		RegisterComponent("Camera", cameraInterface);
#pragma endregion

#pragma region Light
		ComponentInterface lightInterface;
		lightInterface.SetComponent<LightComponent>();
		lightInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			emitter << YAML::Key << "LightType" << YAML::Value << (int)light.GetLightType();
			emitter << YAML::Key << "Range" << YAML::Value << light.GetRange();
			emitter << YAML::Key << "FOV" << YAML::Value << light.GetFov();
			emitter << YAML::Key << "Color" << YAML::Value << light.GetColor();
			emitter << YAML::Key << "ShadowCasting" << YAML::Value << light.IsShadowCasting();
			emitter << YAML::Key << "ShadowBias" << YAML::Value << light.GetBias();

			return true;
		};
		lightInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			Light& light = entity.AddComponent<LightComponent>();

			light.SetLightType((Light::LightType)node["LightType"].as<int>());
			light.SetRange(node["Range"].as<float>());
			light.SetFov(node["FOV"].as<float>());
			light.SetColor(node["Color"].as<glm::vec3>());
			light.SetShadowCasting(node["ShadowCasting"].as<bool>());
			light.SetBias(node["ShadowBias"].as<float>());

			return true;
		};

		RegisterComponent("Light", lightInterface);
#pragma endregion

#pragma region Mesh
		ComponentInterface meshInterface;
		meshInterface.SetComponent<MeshComponent>();

		RegisterComponent("Mesh", meshInterface);
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
	};
}