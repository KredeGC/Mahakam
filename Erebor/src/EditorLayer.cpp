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
		virtual void OnCreate() override
		{
			transform = &entity.GetComponent<TransformComponent>();
		}

		virtual void OnUpdate(Timestep dt) override
		{
			rotation += dt * 10.0f;
			transform->SetRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(rotation), 0.0f }));
		}
	};

	class CameraController : public ScriptableEntity
	{
	public:
		virtual void OnUpdate(Timestep dt) override
		{
			auto& transform = GetComponent<TransformComponent>();

			float speed = 20.0f * dt;
			float rotationSpeed = dt;

			glm::vec3 eulerAngles = transform.GetEulerAngles();

			// Camera rotation
			if (Input::IsKeyPressed(MH_KEY_LEFT))
			{
				eulerAngles.y += rotationSpeed;
				transform.SetEulerangles(eulerAngles);
			}
			else if (Input::IsKeyPressed(MH_KEY_RIGHT))
			{
				eulerAngles.y -= rotationSpeed;
				transform.SetEulerangles(eulerAngles);
			}

			if (Input::IsKeyPressed(MH_KEY_UP))
			{
				eulerAngles.x += rotationSpeed;
				transform.SetEulerangles(eulerAngles);
			}
			else if (Input::IsKeyPressed(MH_KEY_DOWN))
			{
				eulerAngles.x -= rotationSpeed;
				transform.SetEulerangles(eulerAngles);
			}

			// Camera movement
			if (Input::IsKeyPressed(MH_KEY_A))
				transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetRight());
			else if (Input::IsKeyPressed(MH_KEY_D))
				transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetRight());

			if (Input::IsKeyPressed(MH_KEY_W))
				transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetForward());
			else if (Input::IsKeyPressed(MH_KEY_S))
				transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetForward());
		}
	};

	void EditorLayer::OnAttach()
	{
		// Create a new active scene
		activeScene = Scene::CreateScene("assets/textures/pines.hdr");


		// Setup the viewport in editor
		sceneViewPanel.SetContext(activeScene, Renderer::GetFrameBuffer());
		sceneHierarchyPanel.SetContext(activeScene);


		// Setup shaders
		Ref<Shader> skinnedShader = Shader::Create("assets/shaders/Skinned.yaml");
		Ref<Shader> textureShader = Shader::Create("assets/shaders/Albedo.yaml");
		Ref<Shader> colorShader = Shader::Create("assets/shaders/LitColor.yaml");


		// Setup lights
		Entity mainLightEntity = activeScene->CreateEntity("Main Light");
		mainLightEntity.AddComponent<LightComponent>(Light::LightType::Directional, glm::vec3(1.0f, 1.0f, 1.0f));
		mainLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ 0.0f, 1.7f, 0.0f }));

		/*for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{*/
				Entity pointLightEntity = activeScene->CreateEntity("Spot Light");
				pointLightEntity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				pointLightEntity.GetComponent<TransformComponent>().SetPosition({ 1.0f, 1.0f, -6.5f });
				//pointLightEntity.getComponent<TransformComponent>().setPosition({ x, y, 1.0f });
				pointLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ glm::radians(-155.0f), 0.0f, 0.0f }));
		/*	}
		}*/


		// Setup plane
		Ref<Texture> brickAlbedo = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_albedo.png", false, { 128, 128, TextureFormat::SRGB_DXT1, TextureFilter::Point });
		Ref<Texture> brickBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_bump.png", false, { 128, 128, TextureFormat::RG_BC5, TextureFilter::Point });
		Ref<Texture> brickRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_roughness.png", false, { 128, 128, TextureFormat::R_BC4, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Ref<Mesh> planeMesh = Mesh::CreatePlane(2, 2);

		Ref<Material> planeMaterial = Material::Create(textureShader);
		planeMaterial->SetTexture("u_Albedo", 0, brickAlbedo);
		planeMaterial->SetTexture("u_Bump", 0, brickBump);
		planeMaterial->SetTexture("u_Metallic", 0, Texture2D::black);
		planeMaterial->SetTexture("u_Roughness", 0, brickRoughness);

		Entity planeEntity = activeScene->CreateEntity("Plane");
		planeEntity.AddComponent<MeshComponent>(planeMesh, planeMaterial);
		planeEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
		planeEntity.GetComponent<TransformComponent>().SetScale({ 10.0f, 10.0f, 10.0f });


		// Create backpack model
		//SkinnedMesh backpackModel = Mesh::loadModel("assets/models/backpack.obj");

		//// Create backpack textures
		Ref<Texture> backpackDiffuse = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/diffuse.jpg", false, { 4096, 4096, TextureFormat::SRGB_DXT1 });
		Ref<Texture> backpackOcclussion = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/ao.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
		Ref<Texture> backpackBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/normal.png", false, { 4096, 4096, TextureFormat::RG_BC5 });
		Ref<Texture> backpackMetallic = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/specular.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
		Ref<Texture> backpackRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/roughness.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });

		//// Create backpack material
		//Ref<Material> backpackMaterial = Material::Create(textureShader);
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
		/*Ref<Material> skinnedMaterial = Material::Create(skinnedShader);
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
		cameraEntity = activeScene->CreateEntity("Camera");
		cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
		cameraEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });
		cameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();


		// Create mesh & base material
		Ref<Mesh> sphereMesh = Mesh::CreateCubeSphere(8);
		Ref<Material> baseMaterial = Material::Create(colorShader);
		baseMaterial->SetFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

		// Create scene entities
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				// Setup material with texture
				Ref<Material> material = Material::Copy(baseMaterial);
				material->SetFloat("u_Metallic", y / 10.0f);
				material->SetFloat("u_Roughness", x / 10.0f);

				// Create entity
				Entity entity = activeScene->CreateEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
				entity.AddComponent<MeshComponent>(sphereMesh, material);
				entity.GetComponent<TransformComponent>().SetPosition({ x, y, 0.0f });
			}
		}
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_FUNCTION();

		activeScene->OnUpdate(dt);

		statsPanel.OnUpdate(dt);
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		dockSpace.OnImGuiRender();
		profilerPanel.OnImGuiRender();
		sceneViewPanel.OnImGuiRender();
		sceneHierarchyPanel.OnImGuiRender();
		statsPanel.OnImGuiRender();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(EditorLayer::OnWindowResize));
		dispatcher.DispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(EditorLayer::OnMouseScrolled));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == MH_KEY_F5)
		{
			wireframe = !wireframe;
			Renderer::EnableWireframe(wireframe);
		}

		return false;
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent& event)
	{
		/*uint32_t width = event.getWidth();
		uint32_t height = event.getHeight();
		if (width > 0 && height > 0)
			viewportFramebuffer->resize(width, height);*/

		return false;
	}

	bool EditorLayer::OnMouseScrolled(MouseScrolledEvent& event)
	{
		sceneViewPanel.OnMouseScrolled(event);

		return false;
	}
}