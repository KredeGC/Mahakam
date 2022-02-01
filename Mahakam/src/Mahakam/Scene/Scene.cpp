#include "mhpch.h"
#include "Scene.h"

#include "Components.h"

#include "Mahakam/Core/AssetDatabase.h"
#include "Mahakam/Renderer/Renderer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	static Ref<TextureCube> loadOrCreate(const std::string& cachePath, Ref<TextureCube> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		if (!std::filesystem::exists(cachePath))
		{
			Ref<TextureCube> texture = TextureCube::create(src, prefilter, props);

			uint32_t size = texture->getSize();
			uint32_t totalSize = texture->getTotalSize();

			// Save to cache
			char* pixels = new char[totalSize];
			texture->readPixels(pixels, saveMips);
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
			Ref<TextureCube> texture = TextureCube::create(props);
			texture->setData((void*)ss.str().c_str(), size, saveMips);

			stream.close();

			return texture;
		}
	}




	Scene::Scene(const std::string& filepath)
	{
		skyboxTexture = TextureCube::create(filepath, { 4096, TextureFormat::RGB16F });
		skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RGB16F, false });
		skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RGB16F, true });
		//skyboxIrradiance = loadOrCreate(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RGB16F, false });
		//skyboxSpecular = loadOrCreate(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RGB16F, true });

		Ref<Shader> skyboxShader = Shader::create("assets/shaders/Skybox.glsl");
		skyboxMaterial = Material::create(skyboxShader);
		skyboxMaterial->setTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::Scene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
		: skyboxIrradiance(irradianceMap), skyboxSpecular(specularMap) {}

	Scene::~Scene() {}

	void Scene::onUpdate(Timestep ts)
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
					runtime.script->onCreate();
				}

				runtime.script->onUpdate(ts);
			}
		});


		// Update animators
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - AnimatorComponent");

			registry.view<AnimatorComponent, MeshComponent>().each([=](auto entity, auto& animatorComponent, auto& meshComponent)
			{
				auto& animator = animatorComponent.getAnimator();

				animator.UpdateAnimation(ts);

				auto& materials = meshComponent.getMaterials();
				auto& transforms = animator.GetFinalBoneMatrices();

				for (auto& material : materials)
					for (int j = 0; j < transforms.size(); ++j)
						material->setMat4("finalBonesMatrices[" + std::to_string(j) + "]", transforms[j]);
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

			registry.view<LightComponent, TransformComponent>().each([&](auto entity, auto& lightComponent, auto& transformComponent)
			{
				glm::vec3 pos = transformComponent.getPosition();
				glm::quat rot = transformComponent.getRotation();
				glm::vec3 dir = rot * glm::vec3(0.0f, 0.0f, 1.0f);
				Light& light = lightComponent.getLight();

				switch (light.getLightType())
				{
				case Light::LightType::Directional:
					environment.directionalLights.push_back(Renderer::DirectionalLight{ dir, light.getColor() });
					break;
				case Light::LightType::Point:
					environment.pointLights.push_back(Renderer::PointLight{ pos, light.getRange(), light.getColor() });
					break;
				case Light::LightType::Spot:
					environment.spotLights.push_back(Renderer::SpotLight{ pos, rot, light.getFov(), light.getRange(), light.getColor() });
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
				camera.getCamera().recalculateProjectionMatrix();

				mainCamera = &camera;
				mainTransform = &transform;
			}
		}

		// Render each entity with a mesh
		{
			MH_PROFILE_SCOPE("Scene::onUpdate - Render loop");

			if (mainCamera && mainTransform)
			{
				Renderer::beginScene(*mainCamera, *mainTransform, environment);

				auto meshes = registry.view<TransformComponent, MeshComponent>();
				for (auto& entity : meshes)
				{
					auto [transform, meshComponent] = meshes.get<TransformComponent, MeshComponent>(entity);

					auto& meshes = meshComponent.getMeshes();
					auto& materials = meshComponent.getMaterials();
					int materialCount = (int)materials.size() - 1;
					for (int i = 0; i < meshComponent.getMeshCount(); i++)
						Renderer::submit(transform, meshes[i], materials[i < materialCount ? i : materialCount]);
				}

				Renderer::endScene();
			}
		}
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		viewportRatio = (float)width / (float)height;

		auto cameras = registry.view<CameraComponent>();
		for (auto& entity : cameras)
		{
			CameraComponent& cameraComponent = cameras.get<CameraComponent>(entity);

			if (!cameraComponent.hasFixedAspectRatio())
			{
				cameraComponent.getCamera().setRatio(viewportRatio);
			}
		}
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity(registry.create(), this);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::destroyEntity(Entity entity)
	{
		registry.destroy(entity);
	}

	Ref<Scene> Scene::createScene(const std::string& filepath)
	{
		return std::make_shared<Scene>(filepath);
	}

	Ref<Scene> Scene::createScene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap)
	{
		return std::make_shared<Scene>(irradianceMap, specularMap);
	}
}