#include "mhpch.h"
#include "Scene.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Core/Utility.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Texture.h"

#include "Entity.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	//static Asset<TextureCube> LoadOrCreate(const std::string& cachePath, Ref<TextureCube> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	//{
	//	if (!FileUtility::Exists(cachePath))
	//	{
	//		Asset<TextureCube> texture = TextureCube::Create(src, prefilter, props);

	//		uint32_t size = texture->GetSize();
	//		uint32_t totalSize = texture->GetTotalSize();

	//		// Save to cache
	//		char* pixels = new char[totalSize];
	//		texture->ReadPixels(pixels, saveMips);
	//		std::ofstream stream(cachePath, std::ios::binary);
	//		stream.write((char*)&size, sizeof(uint32_t));
	//		stream.write(pixels, totalSize);

	//		delete[] pixels;

	//		return texture;
	//	}
	//	else
	//	{
	//		// Load from cache
	//		std::ifstream stream(cachePath, std::ios::binary);
	//		std::stringstream ss;
	//		uint32_t size = 0;
	//		stream.read((char*)&size, sizeof(uint32_t));
	//		ss << stream.rdbuf();
	//		Asset<TextureCube> texture = TextureCube::Create(props);
	//		texture->SetData((void*)ss.str().c_str(), size, saveMips);

	//		stream.close();

	//		return texture;
	//	}
	//}




	Scene::Scene()
	{
		MH_PROFILE_FUNCTION();

		Asset<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.shader");
		skyboxMaterial = Material::Create(skyboxShader);
		skyboxMaterial->SetTexture("u_Environment", 0, GL::GetTextureCubeWhite());
	}

	Scene::Scene(const std::string& filepath)
	{
		MH_PROFILE_FUNCTION();

		// Idea:: Use RG11B10f instead, should halve memory usage
		skyboxTexture = TextureCube::Create(filepath, { 4096, TextureFormat::RG11B10F });
		skyboxIrradiance = TextureCube::Create(filepath, { 64, TextureFormat::RG11B10F, TextureCubePrefilter::Convolute, false });
		skyboxSpecular = TextureCube::Create(filepath, { 512, TextureFormat::RG11B10F, TextureCubePrefilter::Prefilter, true });

		//skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RG11B10F, false });
		//skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RG11B10F, true });

		Asset<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.shader");
		skyboxMaterial = Material::Create(skyboxShader);
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::~Scene() {}

	void Scene::OnUpdate(Timestep ts, bool editor)
	{
		MH_PROFILE_FUNCTION();

		// Update sound positions
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - AudioSourceComponent");

			registry.view<TransformComponent, AudioSourceComponent>().each([=](auto entity, TransformComponent& transformComponent, AudioSourceComponent& audioSourceComponent)
			{
				auto source = audioSourceComponent.GetAudioSource();
				source->SetPosition(transformComponent.GetPosition());
			});
		}

		// Update animators
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - AnimatorComponent");

			registry.view<AnimatorComponent, MeshComponent>().each([=](auto entity, AnimatorComponent& animatorComponent, MeshComponent& meshComponent)
			{
				auto& animator = animatorComponent.GetAnimator();

				animator.UpdateAnimation(ts);

				auto& materials = meshComponent.GetMaterials();
				auto transforms = animator.GetFinalBoneMatrices();

				for (auto& material : materials)
					for (int j = 0; j < Animator::MAX_BONES; ++j)
						material->SetMat4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);
			});
		}

		// Get the listening source
		TransformComponent* listener = nullptr;
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - AudioListenerComponent");

			registry.view<TransformComponent, AudioListenerComponent>().each([&](auto entity, TransformComponent& transform, AudioListenerComponent& audio)
			{
				listener = &transform;
			});
		}

		// Update sound buffers
		if (!editor && listener)
			AudioEngine::UpdateSounds(listener->GetModelMatrix());

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - CameraComponent");

			registry.view<TransformComponent, CameraComponent>().each([&](auto entity, TransformComponent& transformComponent, CameraComponent& cameraComponent)
			{
				mainCamera = &cameraComponent;
				mainTransform = &transformComponent;
			});
		}

		// Only prepare scene if we actually have something to render to
		if (!editor && mainCamera && mainTransform)
			OnRender(*mainCamera, *mainTransform);
	}

	void Scene::OnRender(Camera& camera, const glm::mat4& cameraTransform)
	{
		// Recalculate all projection matrices, if they've changed
		camera.RecalculateProjectionMatrix();

		EnvironmentData environment
		{
			skyboxMaterial,
			skyboxIrradiance,
			skyboxSpecular
		};

		// Setup scene lights
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - LightComponent");

			registry.view<TransformComponent, LightComponent>().each([&](auto entity, TransformComponent& transformComponent, LightComponent& lightComponent)
			{
				glm::vec3 pos = transformComponent.GetPosition();
				glm::quat rot = transformComponent.GetRotation();
				glm::vec3 dir = rot * glm::vec3(0.0f, 0.0f, 1.0f);
				Light& light = lightComponent.GetLight();

				switch (light.GetLightType())
				{
				case Light::LightType::Directional:
					environment.directionalLights.push_back({ glm::vec3{ cameraTransform[3] }, rot, light });
					break;
				case Light::LightType::Point:
					environment.pointLights.push_back({ pos, light });
					break;
				case Light::LightType::Spot:
					environment.spotLights.push_back({ pos, rot, light });
					break;
				}
			});
		}

		// Update particle systems
		/*auto particleView = registry.view<TransformComponent, ParticleSystemComponent>();
		{
			MH_PROFILE_RENDERING_SCOPE("Mahakam::Scene::OnUpdate - ParticleSystemComponent");

			particleView.each([=](auto entity, TransformComponent& transformComponent, ParticleSystemComponent& particleComponent)
			{
				auto& particles = particleComponent.GetParticleSystem();

				particles.Simulate(ts, transform, Renderer);
			});
		}*/

		// Begin the render loop
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - Render loop");
			Renderer::BeginScene(camera, cameraTransform, environment);

			// Render each entity with a mesh
			{
				MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - Submit");

				registry.view<TransformComponent, MeshComponent>().each([&](auto entity, TransformComponent& transformComponent, MeshComponent& meshComponent)
				{
					auto& meshes = meshComponent.GetMeshes();
					auto& materials = meshComponent.GetMaterials();
					int materialCount = (int)materials.size() - 1;
					for (int i = 0; i < meshComponent.GetMeshCount(); i++)
						Renderer::Submit(transformComponent, meshes[i], materials[i < materialCount ? i : materialCount]);
				});
			}

			// Render particle systems
			{
				MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - SubmitParticles");

				registry.view<TransformComponent, ParticleSystemComponent>().each([=](auto entity, TransformComponent& transformComponent, ParticleSystemComponent& particleComponent)
				{
					Renderer::SubmitParticles(transformComponent, particleComponent);
				});
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		// In editor this is caleld by SceneViewPanel / GameViewPanel
		// In release this is called by EditorLayer.OnWindowResize
		viewportRatio = (float)width / (float)height;

		auto cameras = registry.view<CameraComponent>();
		for (auto& entity : cameras)
		{
			CameraComponent& cameraComponent = cameras.get<CameraComponent>(entity);

			if (!cameraComponent.HasFixedAspectRatio())
			{
				cameraComponent.GetCamera().SetRatio(viewportRatio);
			}
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(registry.create(), this);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		registry.destroy(entity);
	}

	//Ref<Scene> Scene::CreateEmpty()
	MH_DEFINE_FUNC(Scene::CreateEmpty, Ref<Scene>)
	{
		return std::make_shared<Scene>();
	};

	//Ref<Scene> Scene::CreateFilepath(const std::string& filepath)
	MH_DEFINE_FUNC(Scene::CreateFilepath, Ref<Scene>, const std::string& filepath)
	{
		return std::make_shared<Scene>(filepath);
	};
}