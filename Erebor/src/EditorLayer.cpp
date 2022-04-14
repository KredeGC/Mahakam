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

			if (Input::IsKeyPressed(MH_KEY_LEFT_SHIFT))
				speed *= 0.01f;

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

			if (Input::IsKeyPressed(MH_KEY_Q))
				transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetUp());
			else if (Input::IsKeyPressed(MH_KEY_E))
				transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetUp());
		}
	};

	static SharedLibrary* lib;

	void EditorLayer::OnAttach()
	{
		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<TonemappingRenderPass>() });

		// Setup render passes for pixel renderer
		/*Renderer::SetRenderPasses({
			CreateRef<TexelGeometryPass>(),
			CreateRef<TexelLightingPass>(),
			CreateRef<PixelationPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<TonemappingRenderPass>() });*/

		// Create a new active scene
		s_ActiveScene = Scene::CreateScene("assets/textures/pines.hdr");


		// Setup the viewport in editor
		gameViewPanel.SetScene(s_ActiveScene);
		sceneHierarchyPanel.SetContext(s_ActiveScene);


		// Test compute shader
		debugComputeTexture = Texture2D::Create({ width, height, TextureFormat::RGBA32F, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
		debugComputeShader = ComputeShader::Create("assets/compute/UV.glsl");


		// Setup shaders
		//Ref<Shader> skinnedShader = Shader::Create("assets/shaders/default/Skinned.yaml");
		//Ref<Shader> textureShader = Shader::Create("assets/shaders/default/Albedo.yaml");
		//Ref<Shader> colorShader = Shader::Create("assets/shaders/default/LitColor.yaml");
		Ref<Shader> skinnedShader = Shader::Create("assets/shaders/external/DitheredSkinned.yaml");
		Ref<Shader> textureShader = Shader::Create("assets/shaders/external/LitTexel.yaml");
		Ref<Shader> colorShader = Shader::Create("assets/shaders/external/DitheredColor.yaml");


		// Setup scene camera
		Entity cameraEntity = s_ActiveScene->CreateEntity("Main Camera");
		//cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
		cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
		cameraEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });
		cameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();


		// Setup lights
		Entity mainLightEntity = s_ActiveScene->CreateEntity("Main Light");
		mainLightEntity.AddComponent<LightComponent>(Light::LightType::Directional, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
		mainLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ -0.7f, -3.0f, 0.0f }));

		/*for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{*/
		Entity pointLightEntity = s_ActiveScene->CreateEntity("Spot Light");
		pointLightEntity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
		pointLightEntity.GetComponent<TransformComponent>().SetPosition({ 1.0f, 2.5f, 4.0f });
		//pointLightEntity.GetComponent<TransformComponent>().SetPosition({ x, y, 1.0f });
		pointLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ glm::radians(-150.0f), glm::radians(180.0f), 0.0f }));
		/*	}
		}*/


		// Setup plane
		Ref<Texture> brickAlbedo = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_albedo.png", false, { 128, 128, TextureFormat::SRGB_DXT1, TextureFilter::Point });
		Ref<Texture> brickBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_bump.png", false, { 128, 128, TextureFormat::RG_BC5, TextureFilter::Point });
		Ref<Texture> brickRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_roughness.png", false, { 128, 128, TextureFormat::R_BC4, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Ref<Mesh> planeMesh = Mesh::CreatePlane(2, 2);

		Ref<Material> planeMaterial = Material::Create(textureShader);
		planeMaterial->SetTexture("u_Albedo", 0, brickAlbedo);
		//planeMaterial->SetTexture("u_Bump", 0, brickBump);
		planeMaterial->SetTexture("u_Bump", 0, Texture2D::bump);
		planeMaterial->SetTexture("u_Metallic", 0, Texture2D::black);
		planeMaterial->SetTexture("u_Roughness", 0, brickRoughness);

		Entity planeEntity = s_ActiveScene->CreateEntity("Plane");
		planeEntity.AddComponent<MeshComponent>(planeMesh, planeMaterial);
		planeEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
		planeEntity.GetComponent<TransformComponent>().SetScale({ 30.0f, 30.0f, 30.0f });
		//planeEntity.AddComponent<AnimatorComponent>(); // TEMP


		// Create particle system
		Entity particleEntity = s_ActiveScene->CreateEntity("Particle System");
		particleEntity.AddComponent<ParticleSystemComponent>();
		particleEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });


		// Create backpack textures
		//Ref<Texture2D> backpackDiffuse = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/diffuse.jpg", false, { 4096, 4096, TextureFormat::SRGB_DXT1 });
		//Ref<Texture> backpackOcclussion = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/ao.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
		//Ref<Texture> backpackBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/normal.png", false, { 4096, 4096, TextureFormat::RG_BC5 });
		//Ref<Texture> backpackMetallic = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/specular.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
		//Ref<Texture> backpackRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/roughness.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });

		//// Create backpack model
		//SkinnedMesh backpackModel = Mesh::LoadModel("assets/models/backpack.obj");

		//// Create backpack material
		//Ref<Material> backpackMaterial = Material::Create(textureShader);
		//backpackMaterial->SetTexture("u_Albedo", 0, backpackDiffuse);
		//backpackMaterial->SetTexture("u_Bump", 0, backpackBump);
		//backpackMaterial->SetTexture("u_Metallic", 0, backpackMetallic);
		//backpackMaterial->SetTexture("u_Roughness", 0, backpackRoughness);
		//backpackMaterial->SetTexture("u_Occlussion", 0, backpackOcclussion);

		//// Create backpack entity
		//Entity backpackEntity = activeScene->CreateEntity("Bacpack");
		//backpackEntity.AddComponent<MeshComponent>(backpackModel, backpackMaterial);
		//backpackEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 4.0f, 5.0f });
		//backpackEntity.AddComponent<NativeScriptComponent>().Bind<RotateScript>();


		// Setup dancing monke
		/*Ref<Material> skinnedMaterial = Material::Create(skinnedShader);
		skinnedMaterial->SetFloat3("u_Color", { 0.68f, 0.44f, 0.22f });
		skinnedMaterial->SetFloat("u_Metallic", 1.0f);
		skinnedMaterial->SetFloat("u_Roughness", 0.4f);

		SkinnedMesh testSkinnedModel = AssetDatabase::CreateOrLoadAsset<SkinnedMesh>("assets/models/Defeated.fbx");

		SkinnedMesh skinnedModel = Mesh::LoadModel("assets/models/Defeated.fbx");
		Ref<Animation> animation = Animation::Load("assets/models/Defeated.fbx", skinnedModel);

		Entity animatedEntity = activeScene->CreateEntity("Animated");
		animatedEntity.AddComponent<MeshComponent>(skinnedModel, skinnedMaterial);
		animatedEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 1.5f, 5.0f });
		animatedEntity.GetComponent<TransformComponent>().SetScale({ 0.02f, 0.02f, 0.02f });
		animatedEntity.AddComponent<AnimatorComponent>(animation);
		animatedEntity.AddComponent<NativeScriptComponent>().Bind<RotateScript>();*/



		lib = new SharedLibrary("runtime/Sandbox.dll");

		auto runPtr = lib->GetFunction<void, Scene*>("Run");

		runPtr(s_ActiveScene.get());
	}

	void EditorLayer::OnDetach()
	{
		// IMPORTANT: Unload the scene before unloading the runtime
		s_ActiveScene = nullptr;

		delete lib;
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		UpdateRuntimeLibrary();

		// Call shared library update
		auto updatePtr = lib->GetFunction<void, Scene*, Timestep>("Update");

		updatePtr(s_ActiveScene.get(), dt);

#if MH_RUNTIME
		// Only used during runtime
		activeScene->OnUpdate(dt);
		gameViewPanel.SetFrameBuffer(Renderer::GetFrameBuffer()->GetColorTexture(0));
#else
		// Only used in editor
		s_ActiveScene->OnUpdate(dt, true); // TEMPORARY OR SOMETHING?
		sceneViewPanel.OnUpdate(dt);
		s_ActiveScene->OnRender(sceneViewPanel.GetCamera(), sceneViewPanel.GetCamera().GetModelMatrix());
		sceneViewPanel.SetFrameBuffer(Renderer::GetFrameBuffer()->GetColorTexture(0));
		gameViewPanel.SetFrameBuffer(Renderer::GetFrameBuffer()->GetColorTexture(0)); // TEMPORARY UNTIL PLAY IS IMPL
#endif

		// Test compute shader
		/*debugComputeShader->Bind();
		debugComputeTexture->BindImage(0, false, true);
		debugComputeShader->Dispatch(std::ceil(width / 8), std::ceil(height / 4), 1);

		sceneViewPanel.SetFrameBuffer(debugComputeTexture);*/

		statsPanel.OnUpdate(dt);
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		dockSpace.Begin();

		gameViewPanel.OnImGuiRender();
		profilerPanel.OnImGuiRender();
		renderPassPanel.OnImGuiRender();
		sceneViewPanel.OnImGuiRender();
		sceneHierarchyPanel.OnImGuiRender();
		statsPanel.OnImGuiRender();

		dockSpace.End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(EditorLayer::OnWindowResize));

		sceneViewPanel.OnEvent(event);
	}

	void EditorLayer::CopyRuntime(std::istream& binaryStream, size_t binaryLength)
	{
		// Copy new runtime
		std::vector<char> binaryBuffer;
		binaryBuffer.reserve(binaryLength);
		std::copy(std::istreambuf_iterator<char>(binaryStream),
			std::istreambuf_iterator<char>(),
			std::back_inserter(binaryBuffer));

		std::ofstream copyOut("runtime/Sandbox-runtime.dll", std::ios_base::binary);
		copyOut.write(binaryBuffer.data(), binaryLength);
		copyOut.close();
	}

	void EditorLayer::UpdateRuntimeLibrary()
	{
		if (std::filesystem::exists("runtime/Sandbox.dll"))
		{
			std::ifstream binaryStream("runtime/Sandbox.dll", std::ios_base::binary);

			binaryStream.seekg(0, std::ios_base::end);
			size_t binaryLength = binaryStream.tellg();
			binaryStream.seekg(0, std::ios_base::beg);

			if (std::filesystem::exists("runtime/Sandbox-runtime.dll"))
			{
				std::ifstream runtimeStream("runtime/Sandbox-runtime.dll", std::ios_base::binary);

				runtimeStream.seekg(0, std::ios_base::end);
				size_t runtimeLength = runtimeStream.tellg();
				runtimeStream.seekg(0, std::ios_base::beg);

				if (binaryLength != runtimeLength)
				{
					MH_CORE_TRACE("Reloading runtime code");

					// Unload old scene and runtime
					s_ActiveScene = nullptr;
					delete lib;

					// Copy new runtime
					CopyRuntime(binaryStream, binaryLength);

					// Create new scene
					s_ActiveScene = Scene::CreateScene("assets/textures/pines.hdr");
					gameViewPanel.SetScene(s_ActiveScene);
					sceneHierarchyPanel.SetContext(s_ActiveScene);

					// Load new runtime
					lib = new SharedLibrary("runtime/Sandbox-runtime.dll");

					auto runPtr = lib->GetFunction<void, Scene*>("Run");

					runPtr(s_ActiveScene.get());
				}
			}
			else
			{
				MH_CORE_TRACE("Copying runtime code");

				// Copy new runtime
				CopyRuntime(binaryStream, binaryLength);
			}
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == MH_KEY_F5)
			Renderer::EnableWireframe(!Renderer::HasWireframeEnabled());

		if (event.GetKeyCode() == MH_KEY_F6)
			Renderer::EnableGBuffer(!Renderer::HasGBufferEnabled());

		if (event.GetKeyCode() == MH_KEY_F7)
			Renderer::EnableBoundingBox(!Renderer::HasBoundingBoxEnabled());

		return false;
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent& event)
	{
		width = event.GetWidth();
		height = event.GetHeight();

		debugComputeTexture->Resize(width, height);

		return false;
	}
}