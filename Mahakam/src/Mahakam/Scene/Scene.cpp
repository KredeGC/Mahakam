#include "mhpch.h"
#include "Scene.h"

#include "Components.h"

#include "Mahakam/Core/Utility.h"
#include "Mahakam/Core/AssetDatabase.h"
#include "Mahakam/Renderer/Renderer.h"

#include "Entity.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	static Ref<TextureCube> LoadOrCreate(const std::string& cachePath, Ref<TextureCube> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		if (!FileUtility::Exists(cachePath))
		{
			Ref<TextureCube> texture = TextureCube::Create(src, prefilter, props);

			uint32_t size = texture->GetSize();
			uint32_t totalSize = texture->GetTotalSize();

			// Save to cache
			char* pixels = new char[totalSize];
			texture->ReadPixels(pixels, saveMips);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write((char*)&size, sizeof(uint32_t));
			stream.write(pixels, totalSize);

			delete[] pixels;

			return texture;
		}
		else
		{
			// Load from cache
			std::ifstream stream(cachePath, std::ios::binary);
			std::stringstream ss;
			uint32_t size = 0;
			stream.read((char*)&size, sizeof(uint32_t));
			ss << stream.rdbuf();
			Ref<TextureCube> texture = TextureCube::Create(props);
			texture->SetData((void*)ss.str().c_str(), size, saveMips);

			stream.close();

			return texture;
		}
	}




	Scene::Scene(const std::string& filepath)
	{
		// Idea:: Use RG11B10f instead, should halve memory usage
		skyboxTexture = TextureCube::Create(filepath, { 4096, TextureFormat::RG11B10F });
		skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RG11B10F, false });
		skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RG11B10F, true });

		Ref<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.yaml");
		skyboxMaterial = Material::Create(skyboxShader);
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::Scene(const Ref<TextureCube>& skybox, const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
		: skyboxTexture(skybox), skyboxIrradiance(irradianceMap), skyboxSpecular(specularMap)
	{
		Ref<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.yaml");
		skyboxMaterial = Material::Create(skyboxShader);
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::~Scene() {}

	void Scene::OnUpdate(Timestep ts)
	{
		MH_PROFILE_FUNCTION();

		// Update scripts.... REMOVE
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - NativeScriptComponent");

			registry.view<NativeScriptComponent>().each([=](auto entity, auto& scriptComponent)
			{
				// TODO: onScenePlay
				for (auto& runtime : scriptComponent.scripts)
				{
					if (!runtime.script)
					{
						runtime.script = runtime.instantiateScript();
						runtime.script->entity = Entity{ entity, this };
						runtime.script->OnCreate();
					}

					runtime.script->OnUpdate(ts);
				}
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
				auto& transforms = animator.GetFinalBoneMatrices();

				for (auto& material : materials)
					for (int j = 0; j < transforms.size(); ++j)
						material->SetMat4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);
			});
		}

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		{
			MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - CameraComponent");

			registry.view<TransformComponent, CameraComponent>().each([&](auto entity, TransformComponent& transformComponent, CameraComponent& cameraComponent)
			{
				// Recalculate all projection matrices, if they've changed
				cameraComponent.GetCamera().RecalculateProjectionMatrix();

				mainCamera = &cameraComponent;
				mainTransform = &transformComponent;
			});
		}

		// Only prepare scene if we actually have something to render to
		if (mainCamera && mainTransform)
		{
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
						environment.directionalLights.push_back({ mainTransform->GetPosition(), rot, light });
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

			// Render each entity with a mesh
			{
				MH_PROFILE_SCOPE("Mahakam::Scene::OnUpdate - Render loop");
				Renderer::BeginScene(*mainCamera, *mainTransform, environment);

				registry.view<TransformComponent, MeshComponent>().each([&](auto entity, TransformComponent& transformComponent, MeshComponent& meshComponent)
				{
					auto& meshes = meshComponent.GetMeshes();
					auto& materials = meshComponent.GetMaterials();
					int materialCount = (int)materials.size() - 1;
					for (int i = 0; i < meshComponent.GetMeshCount(); i++)
						Renderer::Submit(transformComponent, meshes[i], materials[i < materialCount ? i : materialCount]);
				});

				Renderer::EndScene();
			}
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
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

	Ref<Scene> Scene::CreateScene(const std::string& filepath)
	{
		return std::make_shared<Scene>(filepath);
	}

	Ref<Scene> Scene::CreateScene(const Ref<TextureCube>& skybox, const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
	{
		return std::make_shared<Scene>(skybox, irradianceMap, specularMap);
	}
}