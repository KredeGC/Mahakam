#include "mhpch.h"
#include "Scene.h"

#include "Components.h"

#include "Mahakam/Core/AssetDatabase.h"
#include "Mahakam/Renderer/Renderer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	static Ref<TextureCube> LoadOrCreate(const std::string& cachePath, Ref<TextureCube> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		if (!std::filesystem::exists(cachePath))
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
		skyboxTexture = TextureCube::Create(filepath, { 4096, TextureFormat::RGB16F });
		skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RGB16F, false });
		skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RGB16F, true });
		//skyboxIrradiance = loadOrCreate(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RGB16F, false });
		//skyboxSpecular = loadOrCreate(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RGB16F, true });

		Ref<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.yaml");
		skyboxMaterial = Material::Create(skyboxShader);
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::Scene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
		: skyboxIrradiance(irradianceMap), skyboxSpecular(specularMap) {}

	Scene::~Scene() {}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update scripts.... REMOVE
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


		// Update animators
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - AnimatorComponent");

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

		Renderer::EnvironmentData environment
		{
			skyboxMaterial,
			skyboxIrradiance,
			skyboxSpecular
		};

		// Setup scene lights
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - LightComponent");

			registry.view<LightComponent, TransformComponent>().each([&](auto entity, LightComponent& lightComponent, TransformComponent& transformComponent)
			{
				glm::vec3 pos = transformComponent.GetPosition();
				glm::quat rot = transformComponent.GetRotation();
				glm::vec3 dir = rot * glm::vec3(0.0f, 0.0f, 1.0f);
				Light& light = lightComponent.GetLight();

				switch (light.GetLightType())
				{
				case Light::LightType::Directional:
					environment.directionalLights.push_back({ dir, light });
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

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - CameraComponent");

			auto cameras = registry.view<TransformComponent, CameraComponent>();
			for (auto& entity : cameras)
			{
				auto [transform, camera] = cameras.get<TransformComponent, CameraComponent>(entity);

				// Recalculate all projection matrices, if they've changed
				camera.GetCamera().RecalculateProjectionMatrix();

				mainCamera = &camera;
				mainTransform = &transform;
			}
		}

		// Render each entity with a mesh
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - Render loop");

			if (mainCamera && mainTransform)
			{
				Renderer::BeginScene(*mainCamera, *mainTransform, environment);

				auto meshes = registry.view<TransformComponent, MeshComponent>();
				for (auto& entity : meshes)
				{
					auto [transform, meshComponent] = meshes.get<TransformComponent, MeshComponent>(entity);

					auto& meshes = meshComponent.GetMeshes();
					auto& materials = meshComponent.GetMaterials();
					int materialCount = (int)materials.size() - 1;
					for (int i = 0; i < meshComponent.GetMeshCount(); i++)
						Renderer::Submit(transform, meshes[i], materials[i < materialCount ? i : materialCount]);
				}

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

	Ref<Scene> Scene::CreateScene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
	{
		return std::make_shared<Scene>(irradianceMap, specularMap);
	}
}