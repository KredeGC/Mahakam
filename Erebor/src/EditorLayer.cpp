#include "ebpch.h"
#include "EditorLayer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	// Create scripts
	class RotateScript : public ScriptableEntity
	{
	private:
		float rotation = 0.0f;
		TransformComponent* transform = nullptr;

	public:
		virtual void onCreate() override
		{
			transform = &entity.getComponent<TransformComponent>();
		}

		virtual void onUpdate(Timestep dt) override
		{
			rotation += dt * 10.0f;
			transform->setRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(rotation), 0.0f }));
		}
	};

	class CameraController : public ScriptableEntity
	{
	public:
		virtual void onUpdate(Timestep dt) override
		{
			auto& transform = getComponent<TransformComponent>();

			float speed = 20.0f * dt;
			float rotationSpeed = dt;

			glm::vec3 eulerAngles = transform.getEulerAngles();

			// Camera rotation
			if (Input::isKeyPressed(MH_KEY_LEFT))
			{
				eulerAngles.y += rotationSpeed;
				transform.setEulerangles(eulerAngles);
			}
			else if (Input::isKeyPressed(MH_KEY_RIGHT))
			{
				eulerAngles.y -= rotationSpeed;
				transform.setEulerangles(eulerAngles);
			}

			if (Input::isKeyPressed(MH_KEY_UP))
			{
				eulerAngles.x += rotationSpeed;
				transform.setEulerangles(eulerAngles);
			}
			else if (Input::isKeyPressed(MH_KEY_DOWN))
			{
				eulerAngles.x -= rotationSpeed;
				transform.setEulerangles(eulerAngles);
			}

			// Camera movement
			if (Input::isKeyPressed(MH_KEY_A))
				transform.setPosition(transform.getPosition() - glm::vec3(speed) * transform.getRight());
			else if (Input::isKeyPressed(MH_KEY_D))
				transform.setPosition(transform.getPosition() + glm::vec3(speed) * transform.getRight());

			if (Input::isKeyPressed(MH_KEY_W))
				transform.setPosition(transform.getPosition() - glm::vec3(speed) * transform.getForward());
			else if (Input::isKeyPressed(MH_KEY_S))
				transform.setPosition(transform.getPosition() + glm::vec3(speed) * transform.getForward());
		}
	};

	static Ref<Texture> loadOrCreateTexture(const std::string& cachePath, const std::string& src, bool saveMips, const TextureProps& props)
	{
		if (!std::filesystem::exists(cachePath))
		{
			Ref<Texture2D> texture = Texture2D::create(src, props);

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
			std::ifstream inStream(cachePath, std::ios::binary);
			std::stringstream ss;
			uint32_t size = 0;
			inStream.read((char*)&size, sizeof(uint32_t));
			ss << inStream.rdbuf();
			Ref<Texture> lut = Texture2D::create(props);
			lut->setData((void*)ss.str().c_str(), size, saveMips);

			return lut;
		}
	}

	void EditorLayer::onAttach()
	{
		// Create a new active scene
		activeScene = Scene::createScene("assets/textures/pines.hdr");


		// Setup the viewport in editor
		sceneViewPanel.setContext(activeScene, Renderer::getFrameBuffer());
		sceneHierarchyPanel.setContext(activeScene);


		// Setup shaders
		Ref<Shader> skinnedShader = Shader::create("assets/shaders/Skinned.glsl");
		Ref<Shader> textureShader = Shader::create("assets/shaders/Albedo.glsl");
		Ref<Shader> shader = Shader::create("assets/shaders/LitColor.glsl");


		// Setup lights
		Entity mainLightEntity = activeScene->createEntity("Main Light");
		mainLightEntity.addComponent<LightComponent>(Light::LightType::Directional, glm::vec3(1.0f, 1.0f, 1.0f));
		mainLightEntity.getComponent<TransformComponent>().setRotation(glm::quat({ 0.0f, 1.7f, 0.0f }));

		/*for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{*/
				Entity pointLightEntity = activeScene->createEntity("Point Light");
				pointLightEntity.addComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				pointLightEntity.getComponent<TransformComponent>().setPosition({ 1.0f, 1.0f, -6.5f });
				//pointLightEntity.getComponent<TransformComponent>().setPosition({ x, y, 1.0f });
				pointLightEntity.getComponent<TransformComponent>().setRotation(glm::quat({ glm::radians(-155.0f), 0.0f, 0.0f }));
		/*	}
		}*/


		// Setup plane
		Ref<Texture> brickAlbedo = loadOrCreateTexture("assets/textures/brick/brick_albedo.dat", "assets/textures/brick/brick_albedo.png", false, { 128, 128, TextureFormat::SRGBDXT1, TextureFilter::Point });
		//Ref<Texture> brickAlbedo = Texture2D::create("assets/textures/brick/brick_albedo.png", { TextureFormat::SRGBDXT1, TextureFilter::Point });
		Ref<Texture> brickBump = Texture2D::create("assets/textures/brick/brick_bump.png", { TextureFormat::RGB8, TextureFilter::Point });
		Ref<Texture> brickRoughness = Texture2D::create("assets/textures/brick/brick_albedo.png", { TextureFormat::R8, TextureFilter::Point });
		Ref<Mesh> planeMesh = Mesh::createPlane(2, 2);

		Ref<Material> planeMaterial = Material::create(textureShader);
		planeMaterial->setTexture("u_Albedo", 0, brickAlbedo);
		planeMaterial->setTexture("u_Bump", 0, brickBump);
		planeMaterial->setTexture("u_Metallic", 0, Texture2D::black);
		planeMaterial->setTexture("u_Roughness", 0, brickRoughness);

		Entity planeEntity = activeScene->createEntity("Plane");
		planeEntity.addComponent<MeshComponent>(planeMesh, planeMaterial);
		planeEntity.getComponent<TransformComponent>().setPosition({ 0.0f, -1.0f, 0.0f });
		planeEntity.getComponent<TransformComponent>().setScale({ 10.0f, 10.0f, 10.0f });


		// Create backpack model
		SkinnedMesh backpackModel = Mesh::loadModel("assets/models/backpack.obj");

		// Create backpack textures
		//Ref<Texture> backpackDiffuse = loadOrCreateTexture("assets/textures/backpack/diffuse.dat", "assets/textures/backpack/diffuse.jpg", true, TextureFormat::SRGBDXT1);
		//Ref<Texture> backpackDiffuse = Texture2D::create("assets/textures/backpack/diffuse.jpg", TextureFormat::SRGBDXT1);
		//Ref<Texture> backpackOcclussion = Texture2D::create("assets/textures/backpack/ao.jpg", TextureFormat::R8);
		//Ref<Texture> backpackBump = Texture2D::create("assets/textures/backpack/normal.png");
		//Ref<Texture> backpackMetallic = Texture2D::create("assets/textures/backpack/specular.jpg", TextureFormat::R8);
		//Ref<Texture> backpackRoughness = Texture2D::create("assets/textures/backpack/roughness.jpg", TextureFormat::R8);

		//// Create backpack material
		//Ref<Material> backpackMaterial = Material::create(textureShader);
		//backpackMaterial->setTexture("u_Albedo", 0, backpackDiffuse);
		//backpackMaterial->setTexture("u_Bump", 0, backpackBump);
		//backpackMaterial->setTexture("u_Metallic", 0, backpackMetallic);
		//backpackMaterial->setTexture("u_Roughness", 0, backpackRoughness);
		//backpackMaterial->setTexture("u_Occlussion", 0, backpackOcclussion);

		//// Create backpack entity
		//Entity backpackEntity = activeScene->createEntity("Bacpack");
		//backpackEntity.addComponent<MeshComponent>(backpackModel, backpackMaterial);
		//backpackEntity.getComponent<TransformComponent>().setPosition({ 4.5f, 4.0f, 5.0f });
		//backpackEntity.addComponent<NativeScriptComponent>().bind<RotateScript>();


		// Setup dancing monke
		/*Ref<Material> skinnedMaterial = Material::create(skinnedShader);
		skinnedMaterial->setTexture("u_Albedo", 0, backpackDiffuse);
		skinnedMaterial->setTexture("u_Bump", 0, backpackBump);
		skinnedMaterial->setTexture("u_Metallic", 0, backpackMetallic);
		skinnedMaterial->setTexture("u_Roughness", 0, backpackRoughness);

		SkinnedMesh skinnedModel = Mesh::loadModel("assets/models/Defeated.fbx");
		Ref<Animation> animation = Animation::load("assets/models/Defeated.fbx", skinnedModel);

		Entity animatedEntity = activeScene->createEntity("Animated");
		animatedEntity.addComponent<MeshComponent>(skinnedModel, skinnedMaterial);
		animatedEntity.getComponent<TransformComponent>().setPosition({ 4.5f, 1.5f, 5.0f });
		animatedEntity.getComponent<TransformComponent>().setScale({ 0.02f, 0.02f, 0.02f });
		animatedEntity.addComponent<AnimatorComponent>(animation);
		animatedEntity.addComponent<NativeScriptComponent>().bind<RotateScript>();*/


		// Setup scene camera
		cameraEntity = activeScene->createEntity("Camera");
		cameraEntity.addComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
		cameraEntity.getComponent<TransformComponent>().setPosition({ 4.5f, 4.5f, 12.5f });
		cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();


		// Create mesh & base material
		Ref<Mesh> sphereMesh = Mesh::createCubeSphere(8);
		Ref<Material> baseMaterial = Material::create(shader);
		baseMaterial->setFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

		// Create scene entities
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				// Setup material with texture
				Ref<Material> material = Material::copy(baseMaterial);
				material->setFloat("u_Metallic", y / 10.0f);
				material->setFloat("u_Roughness", x / 10.0f);

				// Create entity
				Entity entity = activeScene->createEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
				entity.addComponent<MeshComponent>(sphereMesh, material);
				entity.getComponent<TransformComponent>().setPosition({ x, y, 0.0f });
			}
		}
	}

	void EditorLayer::onUpdate(Timestep dt)
	{
		MH_PROFILE_FUNCTION();

		activeScene->onUpdate(dt);

		statsPanel.onUpdate(dt);
	}

	void EditorLayer::onImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		dockSpace.onImGuiRender();
		profilerPanel.onImGuiRender();
		sceneViewPanel.onImGuiRender();
		sceneHierarchyPanel.onImGuiRender();
		statsPanel.onImGuiRender();
	}

	void EditorLayer::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.dispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::onKeyPressed));
		dispatcher.dispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(EditorLayer::onWindowResize));
		dispatcher.dispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(EditorLayer::onMouseScrolled));
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& event)
	{
		if (event.getKeyCode() == MH_KEY_F5)
		{
			wireframe = !wireframe;
			Renderer::enableWireframe(wireframe);
		}

		return false;
	}

	bool EditorLayer::onWindowResize(WindowResizeEvent& event)
	{
		/*uint32_t width = event.getWidth();
		uint32_t height = event.getHeight();
		if (width > 0 && height > 0)
			viewportFramebuffer->resize(width, height);*/

		return false;
	}

	bool EditorLayer::onMouseScrolled(MouseScrolledEvent& event)
	{
		sceneViewPanel.onMouseScrolled(event);

		return false;
	}
}