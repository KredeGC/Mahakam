#include "sbpch.h"
#include "EditorLayer.h"

#include "ConsoleLogSink.h"

#include "Panels/ConsolePanel.h"

#include "RenderPasses/BoundingBoxRenderPass.h"

#include <Mahakam/Editor/Windows/AssetManagerPanel.h>
#include <Mahakam/Editor/Windows/ContentBrowserPanel.h>
#include <Mahakam/Editor/Windows/EnvironmentPanel.h>
#include <Mahakam/Editor/Windows/ImportWizardPanel.h>
#include <Mahakam/Editor/Windows/ProfilerPanel.h>
#include <Mahakam/Editor/Windows/RenderPassPanel.h>
#include <Mahakam/Editor/Windows/SceneHierarchyPanel.h>
#include <Mahakam/Editor/Windows/SceneViewPanel.h>
#include <Mahakam/Editor/Windows/StatsPanel.h>

#include "Mahakam/Asset/AssetLookup.h"

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	void EditorLayer::OnAttach()
	{
		// Add the console panel to the logger
		auto sink = CreateRef<ConsoleLogSinkMt>();

		Log::GetLogger()->sinks().push_back(sink);

		ComponentRegistry::RegisterDefaultComponents();

		AssetDatabase::RegisterDefaultAssetImporters();

		AssetDatabase::RefreshAssetPaths();

		ResourceRegistry::RegisterDefaultAssetImporters();

		ResourceRegistry::RefreshImportPaths();

#pragma region Windows
		// AssetManagerPanel
		EditorWindowRegistry::RegisterWindowClass<AssetManagerPanel>("Asset Manager");
		//EditorWindowRegistry::OpenWindow("Asset Manager");

		// ConsolePanel
		EditorWindowRegistry::RegisterWindowClass<ConsolePanel>("Console");
		EditorWindowRegistry::OpenWindow("Console");

		// ContentBrowserPanel
		EditorWindowRegistry::RegisterWindowClass<ContentBrowserPanel>("Content Browser");
		EditorWindowRegistry::OpenWindow("Content Browser");

		// EnvironmentPanel
		EditorWindowRegistry::RegisterWindowClass<EnvironmentPanel>("Scene Environment");
		//EditorWindowRegistry::OpenWindow("Scene Environment");

		// ImportWizardPanel
		EditorWindowRegistry::EditorWindowProps panelProps;
		panelProps.Name = "Import Wizard";
		panelProps.Viewable = false;
		panelProps.SetWindow<ImportWizardPanel>();

		EditorWindowRegistry::RegisterWindow(panelProps);

		// ProfilerPanel
		EditorWindowRegistry::RegisterWindowClass<ProfilerPanel>("Profiler");
		//EditorWindowRegistry::OpenWindow("Profiler");

		// RenderPassPanel
		EditorWindowRegistry::RegisterWindowClass<RenderPassPanel>("Renderpass");
		//EditorWindowRegistry::OpenWindow("Renderpass");

		// SceneHierarchyPanel
		EditorWindowRegistry::RegisterWindowClass<SceneHierarchyPanel>("Scene Hierarchy");
		EditorWindowRegistry::OpenWindow("Scene Hierarchy");

		// SceneViewPanel
		EditorWindowRegistry::RegisterWindowClass<SceneViewPanel>("Scene View");
		EditorWindowRegistry::OpenWindow("Scene View");

		// StatsPanel
		EditorWindowRegistry::RegisterWindowClass<StatsPanel>("Stats");
		EditorWindowRegistry::OpenWindow("Stats");
#pragma endregion

		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<BoundingBoxRenderPass>(),
			CreateRef<TonemappingRenderPass>() });

		// Use this once scenes are setup correctly
		Asset<Material> skyboxMaterial = Asset<Material>("import/materials/Skybox.material.import");
		Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>(5839156319952352249ull);
		Asset<TextureCube> skyboxSpecular = Asset<TextureCube>(14413137597893043784ull);

		Ref<Scene> scene = Scene::Create();
		scene->SetSkyboxMaterial(skyboxMaterial);
		scene->SetSkyboxIrradiance(skyboxIrradiance);
		scene->SetSkyboxSpecular(skyboxSpecular);

		SceneManager::SetActiveScene(scene);

		// Scene camera
		Entity cameraEntity = scene->CreateEntity("Main Camera");
		cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
		cameraEntity.AddComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });
		cameraEntity.AddComponent<AudioListenerComponent>();


		// Directional light
		Entity mainLightEntity = scene->CreateEntity("Main Light");
		mainLightEntity.AddComponent<LightComponent>(Light::LightType::Directional, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
		mainLightEntity.AddComponent<TransformComponent>().SetRotation(glm::quat({ -0.7f, -3.0f, 0.0f }));


		// Spot light
		Entity pointLightEntity = scene->CreateEntity("Spot Light");
		pointLightEntity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
		pointLightEntity.AddComponent<TransformComponent>().SetPosition({ 1.0f, 2.5f, 4.0f });
		pointLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ glm::radians(-150.0f), glm::radians(180.0f), 0.0f }));


		// Setup plane
		Asset<Mesh> planeMesh = Asset<Mesh>(556425745901094492ull);

		/*Ref<SubMesh> planeMesh = SubMesh::CreatePlane(2, 2);

		Asset<Material> planeMaterial = Asset<Material>("import/assets/materials/BrickPlane.material.import");*/

		Entity planeEntity = scene->CreateEntity("Plane");
		planeEntity.AddComponent<MeshComponent>(planeMesh);
		planeEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
		planeEntity.GetComponent<TransformComponent>().SetScale({ 30.0f, 30.0f, 30.0f });


		// Create particle system
		Entity particleEntity = scene->CreateEntity("Particle System");
		particleEntity.AddComponent<ParticleSystemComponent>();
		particleEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });


#if 0
		// Create backpack model
		Asset<Model> backpackModel = Asset<Model>(13155307824895996659ull);

		// Create backpack entity
		Entity backpackEntity = scene->CreateEntity("Bacpack");
		backpackEntity.AddComponent<MeshComponent>(backpackModel);
		backpackEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
#endif


#if 1
		Entity animatedArchive = scene->CreateEntity("Animated Skeletons");

		// Create glTF skinned model
		Asset<Model> skinnedModel = Asset<Model>(11945816250811211738ull);
		//Asset<Mesh> skinnedModel = Mesh::LoadMesh("resource/models/mannequin_clap.glb");
		Asset<Animation> skinnedAnimation = Asset<Animation>(1494460746093480384ull);
		//Asset<Animation> skinnedAnimation = Animation::Load("resource/models/mannequin_clap.glb");

		//Asset<Material> skinnedMaterial = Asset<Material>("assets/materials/Skinned.material.asset");

		// Create backpack entity
		Entity skinnedEntity = scene->CreateEntity("Skinned glTF");
		skinnedEntity.SetParent(animatedArchive);
		skinnedEntity.AddComponent<MeshComponent>(skinnedModel);
		skinnedEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
		skinnedEntity.AddComponent<AnimatorComponent>().GetAnimator().SetAnimation(skinnedAnimation);
		SkeletonComponent& skin = skinnedEntity.AddComponent<SkeletonComponent>();

		skin.CreateBoneEntities(skinnedEntity, skinnedModel->NodeHierarchy);
#endif


#if 0
		// Create piano sound entity
		Asset<Sound> pianoSound = Asset<Sound>("assets/sounds/piano.wav.asset");

		Entity pianoEntity = scene->CreateEntity("Piano");
		pianoEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
		AudioSourceComponent& pianoSource = pianoEntity.AddComponent<AudioSourceComponent>();
		pianoSource.SetSpatialBlend(1.0f);
		pianoSource.SetSound(pianoSound);
		pianoSource.Play();


		// Create vespa sound entity
		Asset<Sound> vespaSound = Asset<Sound>("assets/sounds/vespa.wav.asset");

		Entity vespaEntity = scene->CreateEntity("Vespa Sound");
		vespaEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, 1.0f, 2.0f });
		AudioSourceComponent& vespaSource = vespaEntity.AddComponent<AudioSourceComponent>();
		vespaSource.SetSound(vespaSound);
		vespaSource.Play();
#endif


#if 0
		// Create base collection entity to store in
		Asset<Mesh> sphereMesh = Asset<Mesh>(14539941857077216621ull);

		// Create base collection entity to store in
		Entity sphereCollection = scene->CreateEntity("Spheres");
		sphereCollection.AddComponent<TransformComponent>().SetEulerangles({ 0.0f, 90.0f, 0.0f });

		// Create scene entities
		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				// Setup material with texture
				Asset<Material> material = Material::Copy(sphereMesh->GetProps().Materials[0]);
				material->SetFloat("Uniforms.MetallicMul", y / 10.0f);
				material->SetFloat("Uniforms.RoughnessMul", x / 10.0f);

				// Create entity
				Entity entity = scene->CreateEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
				entity.SetParent(sphereCollection);
				entity.AddComponent<MeshComponent>(Mesh::Copy(sphereMesh), material);
				entity.AddComponent<TransformComponent>().SetPosition({ x, y, 0.0f });

				if (x == 0 && y == 0)
					entity.GetComponent<TransformComponent>().SetScale({ 2.0f, 1.0f, 1.0f });

				//sphereCollection = entity; // TEMPORARY: Just to see the depth
			}
		}
#endif

		Entity entity1 = scene->CreateEntity("ID 1");
		Entity entity2 = scene->CreateEntity("ID 2");
		Entity entity3 = scene->CreateEntity("ID 3");
		Entity entity4 = scene->CreateEntity("ID 4");
		Entity entity5 = scene->CreateEntity("ID 5");
		Entity entity6 = scene->CreateEntity("ID 6");
		Entity entity7 = scene->CreateEntity("ID 7");
		Entity entity8 = scene->CreateEntity("ID 8");

		entity1.SetParent(entity6);
		entity2.SetParent(entity1);
		entity3.SetParent(entity4);
		entity4.SetParent(entity1);
		entity5.SetParent(entity2);
		entity7.SetParent(entity5);
		entity8.SetParent(entity4);
	}

	void EditorLayer::OnDetach()
	{
		SceneManager::SetActiveScene(nullptr);

		ComponentRegistry::DeregisterDefaultComponents();

		AssetDatabase::DeregisterDefaultAssetImporters();

		ResourceRegistry::DeregisterDefaultAssetImporters();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		AssetDatabase::ProcessAssets();

		static const bool m_PlayMode = false;
		if (m_PlayMode)
			SceneManager::GetActiveScene()->OnUpdate(dt);
		else
			SceneManager::GetActiveScene()->OnUpdate(dt, true); // TEMPORARY until play-mode is implemented

		// Test compute shader
		/*debugComputeShader->Bind();
		debugComputeTexture->BindImage(0, false, true);
		debugComputeShader->Dispatch(std::ceil(width / 8), std::ceil(height / 4), 1);

		m_SceneViewPanel.SetFrameBuffer(debugComputeTexture);*/

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnUpdate(dt);
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		m_DockSpace.Begin();

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnImGuiRender();

		auto iter = windows.begin();
		while (iter != windows.end())
		{
			if (!(*iter)->IsOpen())
				iter = EditorWindowRegistry::CloseWindow(*iter);
			else
				iter++;
		}

		m_DockSpace.End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));

		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_DockSpace.OnKeyPressed));

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnEvent(event); // TODO: Fix to be blocking if true
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::F5)
			Renderer::EnableWireframe(!Renderer::HasWireframeEnabled());

		if (event.GetKeyCode() == Key::F6)
			Renderer::EnableGBuffer(!Renderer::HasGBufferEnabled());

		if (event.GetKeyCode() == Key::F7)
			Renderer::EnableBoundingBox(!Renderer::HasBoundingBoxEnabled());

		return false;
	}

	bool EditorLayer::OnWindowResized(WindowResizeEvent& event)
	{
		SceneManager::GetActiveScene()->OnViewportResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}