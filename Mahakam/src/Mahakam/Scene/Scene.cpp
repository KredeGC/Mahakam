#include "Mahakam/mhpch.h"
#include "Scene.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/FlagComponents.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Core/FileUtility.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

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



	static uint16_t RecursiveDepth(entt::registry& registry, const RelationshipComponent& relation)
	{
		if (relation.Parent)
			return 1 + RecursiveDepth(registry, registry.get<RelationshipComponent>(relation.Parent));
		else
			return 0;
	}

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

		// Update matrix transforms
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - TransformComponent");

			registry.view<RelationshipComponent, TransformComponent>().each([=](RelationshipComponent& relation, TransformComponent& transform)
			{
				if (relation.Parent && relation.Parent.HasComponent<TransformComponent>())
				{
					auto& parentTransform = relation.Parent.GetComponent<TransformComponent>();
					const glm::mat4& parentMatrix = parentTransform.GetModelMatrix();

					transform.UpdateModelMatrix(parentMatrix);
				}
				else
				{
					transform.UpdateModelMatrix(glm::mat4{ 1.0f });
				}
			});
		}

		// Update sound positions
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AudioSourceComponent");

			registry.view<TransformComponent, AudioSourceComponent>().each([=](TransformComponent& transformComponent, AudioSourceComponent& audioSourceComponent)
			{
				auto source = audioSourceComponent.GetAudioSource();
				source->SetPosition(transformComponent.GetPosition());
			});
		}

		// Update animators
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AnimatorComponent");

			registry.view<AnimatorComponent, MeshComponent>().each([=](AnimatorComponent& animatorComponent, MeshComponent& meshComponent)
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
			MH_PROFILE_SCOPE("Scene::OnUpdate - AudioListenerComponent");

			registry.view<TransformComponent, AudioListenerComponent>().each([&](TransformComponent& transform, AudioListenerComponent& audio)
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
			MH_PROFILE_SCOPE("Scene::OnUpdate - CameraComponent");

			registry.view<TransformComponent, CameraComponent>().each([&](TransformComponent& transformComponent, CameraComponent& cameraComponent)
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
		MH_PROFILE_FUNCTION();

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
			MH_PROFILE_SCOPE("Scene::OnUpdate - LightComponent");

			registry.view<TransformComponent, LightComponent>().each([&](TransformComponent& transformComponent, LightComponent& lightComponent)
			{
				const glm::mat4& modelMatrix = transformComponent;

				glm::vec3 pos = modelMatrix[3];
				glm::quat rot;

				Math::DecomposeRotation(modelMatrix, rot);

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
			MH_PROFILE_SCOPE("Scene::OnUpdate - Render loop");
			Renderer::BeginScene(camera, cameraTransform, environment);

			// Render each entity with a mesh
			{
				MH_PROFILE_SCOPE("Scene::OnUpdate - Submit");

				registry.view<TransformComponent, MeshComponent>().each([&](TransformComponent& transformComponent, MeshComponent& meshComponent)
				{
					auto& meshes = meshComponent.GetMeshes();
					auto& materials = meshComponent.GetMaterials();
					int materialCount = (int)materials.size() - 1;

					glm::mat4 modelMatrix = transformComponent.GetModelMatrix();

					for (int i = 0; i < meshComponent.GetMeshCount(); i++)
						Renderer::Submit(modelMatrix, meshes[i], materials[i < materialCount ? i : materialCount]);
				});
			}

			// Render particle systems
			{
				MH_PROFILE_SCOPE("Scene::OnUpdate - SubmitParticles");

				registry.view<TransformComponent, ParticleSystemComponent>().each([=](TransformComponent& transformComponent, ParticleSystemComponent& particleComponent)
				{
					Renderer::SubmitParticles(transformComponent, particleComponent);
				});
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		// In editor this is called by SceneViewPanel / GameViewPanel
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
		std::string tagName = name.empty() ? "Entity" : name;
		Entity entity(registry.create(), this);
		auto& tag = entity.AddComponent<TagComponent>(tagName);
		auto& relation = entity.AddComponent<RelationshipComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		registry.destroy(entity);
	}

	void Scene::Sort()
	{
		registry.sort<RelationshipComponent>([&](const RelationshipComponent& clhs, const RelationshipComponent& crhs)
		{
			uint16_t lhsDepth = RecursiveDepth(registry, clhs);
			uint16_t rhsDepth = RecursiveDepth(registry, crhs);

			// TODO: Store the depth somehow
			// Possibly sort by parents as well

			bool lowerParent = lhsDepth < rhsDepth;
			bool lowerAddress = (clhs.Parent == crhs.Parent && &clhs < &crhs);

			return lowerParent || lowerAddress;
		});

		//registry.sort<RelationshipComponent, TransformComponent>();

		registry.sort<TransformComponent>([&](const entt::entity lhs, const entt::entity rhs)
		{
			auto& clhs = registry.get<RelationshipComponent>(lhs);
			auto& crhs = registry.get<RelationshipComponent>(rhs);

			uint16_t lhsDepth = RecursiveDepth(registry, clhs);
			uint16_t rhsDepth = RecursiveDepth(registry, crhs);

			// TODO: Store the depth somehow
			// Possibly sort by parents as well

			bool lowerParent = lhsDepth < rhsDepth;
			bool lowerAddress = (clhs.Parent == crhs.Parent && &clhs < &crhs);

			return lowerParent || lowerAddress;
		});
	}

	//Ref<Scene> Scene::CreateEmpty()
	MH_DEFINE_FUNC(Scene::CreateEmpty, Ref<Scene>)
	{
		return CreateRef<Scene>();
	};

	//Ref<Scene> Scene::CreateFilepath(const std::string& filepath)
	MH_DEFINE_FUNC(Scene::CreateFilepath, Ref<Scene>, const std::string& filepath)
	{
		return CreateRef<Scene>(filepath);
	};
}