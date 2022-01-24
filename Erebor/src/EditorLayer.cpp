#include "ebpch.h"
#include "EditorLayer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	// Create Rotate script
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

	void EditorLayer::onAttach()
	{
		// Create a new active scene
		activeScene = Scene::createScene("assets/textures/apt.hdr");


		// Setup the viewport in editor
		sceneViewPanel.setContext(activeScene, Renderer::getFrameBuffer());
		sceneHierarchyPanel.setContext(activeScene);


		// Setup shaders
		Ref<Shader> skinnedShader = Shader::create("assets/shaders/Skinned.glsl");
		Ref<Shader> textureShader = Shader::create("assets/shaders/Albedo.glsl");
		Ref<Shader> shader = Shader::create("assets/shaders/LitColor.glsl");


		// Setup dancing monke
		Ref<Material> inanimateMaterial = Material::create(shader);
		inanimateMaterial->setFloat4("u_Color", glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
		inanimateMaterial->setFloat("u_Metallic", 0.0f);
		inanimateMaterial->setFloat("u_Roughness", 0.0f);
		debugMaterial = Material::create(skinnedShader);

		debugModel = Mesh::loadModel("assets/models/Defeated.fbx");
		debugAnimation = std::make_shared<Animation>("assets/models/Defeated.fbx", debugModel);
		debugAnimator = std::make_shared<Animator>(debugAnimation.get());

		Entity animatedEntity = activeScene->createEntity("Animated");
		animatedEntity.addComponent<MeshComponent>(debugModel, debugMaterial);
		animatedEntity.getComponent<TransformComponent>().setPosition({ 2.5f, 1.5f, 5.0f });
		animatedEntity.getComponent<TransformComponent>().setScale({ 0.02f, 0.02f, 0.02f });
		animatedEntity.addComponent<NativeScriptComponent>().bind<RotateScript>();

		Entity inanimateEntity = activeScene->createEntity("Inanimate");
		inanimateEntity.addComponent<MeshComponent>(debugModel, inanimateMaterial);
		inanimateEntity.getComponent<TransformComponent>().setPosition({ 6.5f, 1.5f, 5.0f });
		inanimateEntity.getComponent<TransformComponent>().setScale({ 0.02f, 0.02f, 0.02f });
		inanimateEntity.addComponent<NativeScriptComponent>().bind<RotateScript>();


		// Setup scene camera
		cameraEntity = activeScene->createEntity("Camera");
		cameraEntity.addComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
		cameraEntity.getComponent<TransformComponent>().setPosition({ 4.5f, 4.5f, 12.5f });

		class CameraController : public ScriptableEntity
		{
		public:
			virtual void onUpdate(Timestep dt) override
			{
				auto& transform = getComponent<TransformComponent>();

				float speed = 20.0f * dt;
				float rotationSpeed = dt;

				// Camera movement
				if (Input::isKeyPressed(MH_KEY_A))
					transform.setPosition(transform.getPosition() - glm::vec3(speed) * transform.getRight());
				else if (Input::isKeyPressed(MH_KEY_D))
					transform.setPosition(transform.getPosition() + glm::vec3(speed) * transform.getRight());

				if (Input::isKeyPressed(MH_KEY_W))
					transform.setPosition(transform.getPosition() - glm::vec3(speed) * transform.getForward());
				else if (Input::isKeyPressed(MH_KEY_S))
					transform.setPosition(transform.getPosition() + glm::vec3(speed) * transform.getForward());

				// Camera rotation
				if (Input::isKeyPressed(MH_KEY_LEFT))
					transform.setRotation(transform.getRotation() * glm::quat({ 0.0f, rotationSpeed, 0.0f }));
				else if (Input::isKeyPressed(MH_KEY_RIGHT))
					transform.setRotation(transform.getRotation() * glm::quat({ 0.0f, -rotationSpeed, 0.0f }));

				if (Input::isKeyPressed(MH_KEY_UP))
					transform.setRotation(transform.getRotation() * glm::quat({ rotationSpeed, 0.0f, 0.0f }));
				else if (Input::isKeyPressed(MH_KEY_DOWN))
					transform.setRotation(transform.getRotation() * glm::quat({ -rotationSpeed, 0.0f, 0.0f }));
			}
		};

		cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();


		// Create backpack model
		SkinnedMesh backpackModel = Mesh::loadModel("assets/models/backpack.obj");

		// Create backpack material
		Ref<Texture> backpackDiffuse = Texture2D::create("assets/textures/backpack/diffuse.jpg");
		Ref<Texture> backpackMetallic = Texture2D::create("assets/textures/backpack/specular.jpg", { TextureFormat::R8 });
		Ref<Texture> backpackRoughness = Texture2D::create("assets/textures/backpack/roughness.jpg", { TextureFormat::R8 });

		Ref<Material> backpackMaterial = Material::create(textureShader);
		backpackMaterial->setTexture("u_Albedo", 3, backpackDiffuse);
		backpackMaterial->setTexture("u_Metallic", 4, backpackMetallic);
		backpackMaterial->setTexture("u_Roughness", 5, backpackRoughness);

		// Create backpack entity
		Entity backpackEntity = activeScene->createEntity("Bacpack");
		backpackEntity.addComponent<MeshComponent>(backpackModel, backpackMaterial);
		backpackEntity.getComponent<TransformComponent>().setPosition({ 4.5f, 4.0f, 5.0f });
		backpackEntity.addComponent<NativeScriptComponent>().bind<RotateScript>();


		// Create mesh & base material
		Ref<Mesh> sphereMesh = Mesh::createCubeSphere(8);
		Ref<Material> baseMaterial = Material::create(shader);
		baseMaterial->setFloat3("u_Color", { 0.5f, 0.0f, 0.0f });


		// Create scene entities
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				// Setup material with texture
				Ref<Material> material = Material::copy(baseMaterial);
				material->setFloat("u_Metallic", y / 9.0f);
				material->setFloat("u_Roughness", x / 9.0f);

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

		debugAnimator->UpdateAnimation(dt);

		auto transforms = debugAnimator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			debugMaterial->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

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
			GL::setFillMode(wireframe);
			wireframe = !wireframe;
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