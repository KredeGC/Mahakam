#include "mhpch.h"
#include "Scene.h"

#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Components/NativeScriptComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"

#include "Mahakam/Renderer/Renderer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	static Ref<Texture> loadOrCreate(const std::string& cachePath, const std::string& src, bool saveMips, const CubeTextureProps& props)
	{
		if (!std::filesystem::exists(cachePath))
		{
			Ref<Texture> texture = TextureCube::create(src, props);

			uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution)));
			uint32_t maxMipLevels = saveMips ? mipLevels : 1;

			uint32_t size = 0;
			for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
			{
				uint32_t mipResolution = (uint32_t)(props.resolution * std::pow(0.5, mip));
				size += 6 * 6 * mipResolution * mipResolution;
			}

			// Save to cache
			char* pixels = new char[size];
			texture->readPixels(pixels, saveMips);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write(pixels, size);

			delete[] pixels;

			return texture;
		}
		else
		{
			// Load from cache
			std::ifstream stream(cachePath, std::ios::binary);
			std::stringstream ss;
			ss << stream.rdbuf();
			Ref<Texture> texture = TextureCube::create(props);
			texture->setData((void*)ss.str().c_str(), saveMips);

			stream.close();

			return texture;
		}
	}




	Scene::Scene(const std::string& filepath)
	{
		skyboxTexture = TextureCube::create(filepath, { 2048, TextureFormat::RGB16F });
		skyboxIrradiance = loadOrCreate(filepath + ".irradiance", filepath, false, {32, TextureFormat::RGB16F, true, TextureCubePrefilter::Convolute});
		skyboxSpecular = loadOrCreate(filepath + ".specular", filepath, true, {256, TextureFormat::RGB16F, true, TextureCubePrefilter::Prefilter});

		Ref<Shader> skyboxShader = Shader::create("assets/shaders/Skybox.glsl");
		skyboxMaterial = Material::create(skyboxShader);
		skyboxMaterial->setTexture("u_Environment", 3, skyboxTexture);
	}

	Scene::Scene(const Ref<Texture>& irradianceMap, const Ref<Texture>& specularMap)
		: skyboxIrradiance(irradianceMap), skyboxSpecular(specularMap) {}

	Scene::~Scene() {}

	void Scene::onUpdate(Timestep ts)
	{
		// Update scripts
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




		Ref<Light> mainLight = std::make_shared<Light>(glm::vec3(1.0f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

		Renderer::EnvironmentData environment
		{
			{ mainLight },
			skyboxIrradiance,
			skyboxSpecular
		};

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		auto cameras = registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : cameras)
		{
			auto [transform, camera] = cameras.get<TransformComponent, CameraComponent>(entity);

			// Recalculate all projection matrices, if they've changed
			camera.getCamera().recalculateProjectionMatrix();

			mainCamera = &camera;
			mainTransform = &transform;
		}

		// Render each entity with a mesh
		if (mainCamera && mainTransform)
		{
			Renderer::beginScene(*mainCamera, *mainTransform, environment);

			auto meshes = registry.view<TransformComponent, MeshComponent>();
			for (auto& entity : meshes)
			{
				auto [transform, meshComponent] = meshes.get<TransformComponent, MeshComponent>(entity);

				auto& meshes = meshComponent.getMeshes();
				auto& materials = meshComponent.getMaterials();
				int materialCount = materials.size() - 1;
				for (int i = 0; i < meshComponent.getMeshCount(); i++)
					Renderer::submit(transform, meshes[i], materials[i < materialCount ? i : materialCount]);
			}

			Renderer::endScene();

			// Render skybox
			// TODO: Render after opaque and before transparent queue
			Renderer::getFrameBuffer()->bind();

			skyboxMaterial->getShader()->bind();
			skyboxMaterial->bind();
			GL::drawScreenQuad();

			Renderer::getFrameBuffer()->unbind();
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

	Ref<Scene> Scene::createScene(const Ref<Texture>& irradianceMap, const Ref<Texture>& specularMap)
	{
		return std::make_shared<Scene>(irradianceMap, specularMap);
	}
}