#pragma once

#include <Mahakam/Mahakam.h>
#include "Mahakam/Core/EntryPoint.h"

#if !defined(MH_STANDALONE_TITLE)
#define MH_STANDALONE_TITLE "" // Just so VS stops nagging me about this
#error MH_STANDALONE_TITLE not defined
#endif

#if !defined(MH_STANDALONE_ICON)
#define MH_STANDALONE_ICON ""
#error MH_STANDALONE_ICON not defined
#endif

void Load(ImGuiContext* context, void*** funcPtrs);
void Run(Mahakam::Scene* scene);
void Update(Mahakam::Scene* scene, Mahakam::Timestep dt);
void Stop(Mahakam::Scene* scene);
void Unload();

namespace Mahakam
{
	class StandaloneLayer : public Layer
	{
	private:
		Asset<FrameBuffer> m_ViewportBuffer;

	public:
		StandaloneLayer() : Layer("Standalone") {}

		virtual void OnAttach() override
		{
			// Load default component serializers
			ComponentRegistry::RegisterDefaultComponents();

			// Load default asset importers
			AssetDatabase::LoadDefaultAssetImporters();

			// Refresh asset imports
			AssetDatabase::ReloadAssetImports();

			// Create a swapchain target for blitting
			FrameBufferProps mProps;
			mProps.Width = 1600;
			mProps.Height = 900;
			mProps.SwapChainTarget = true;

			m_ViewportBuffer = FrameBuffer::Create(mProps);

			// Setup render passes for the default renderer
			Renderer::SetRenderPasses({
				CreateRef<GeometryRenderPass>(),
				CreateRef<LightingRenderPass>(),
				CreateRef<ParticleRenderPass>(),
				CreateRef<TonemappingRenderPass>() });

			// Create scene?
			// TODO: Let the editor decide which scene to load via a settings panel
			Asset<Material> skyboxMaterial = Asset<Material>("import/assets/materials/Skybox.material.import");
			Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>("import/assets/textures/pines.irradiance.import");
			Asset<TextureCube> skyboxSpecular = Asset<TextureCube>("import/assets/textures/pines.specular.import");

			Ref<Scene> activeScene = Scene::Create();
			activeScene->SetSkyboxMaterial(skyboxMaterial);
			activeScene->SetSkyboxIrradiance(skyboxIrradiance);
			activeScene->SetSkyboxSpecular(skyboxSpecular);

			SceneManager::SetActiveScene(activeScene);

			// Run the library functions, as if it was a shared library
			ImGuiContext* context = ImGui::GetCurrentContext(); // TODO: This seems wrong. This heavily relies on ImGui

			// Load the runtime
			Load(context, nullptr);

			// Run the game
			Run(activeScene.get());
		}

		virtual void OnDetach() override
		{
			Stop(SceneManager::GetActiveScene().get());

			// IMPORTANT: Unload the scene before unloading the runtime
			SceneManager::SetActiveScene(nullptr);

			// Unload the runtime
			Unload();

			ComponentRegistry::DeregisterDefaultComponents();
			
			AssetDatabase::UnloadDefaultAssetImporters();
		}

		virtual void OnUpdate(Timestep dt) override
		{
			Update(SceneManager::GetActiveScene().get(), dt);

			SceneManager::GetActiveScene()->OnUpdate(dt);

			Renderer::GetFrameBuffer()->Blit(m_ViewportBuffer, true, false);

			// Blit the framebuffer to the window
			/*m_BlitShader->Bind("POSTPROCESSING");
			m_BlitShader->SetTexture("u_Albedo", Renderer::GetFrameBuffer()->GetColorTexture(0));
			m_BlitShader->SetUniformInt("u_Depth", 0);

			GL::EnableZTesting(false);
			GL::EnableZWriting(false);
			Renderer::DrawScreenQuad();
			GL::EnableZWriting(true);
			GL::EnableZTesting(true);*/
		}

		virtual void OnImGuiRender() override
		{

		}

		virtual void OnEvent(Event& event) override
		{
			EventDispatcher dispatcher(event);
			dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(StandaloneLayer::OnWindowResized));
		}

		bool OnWindowResized(WindowResizeEvent& event)
		{
			SceneManager::GetActiveScene()->OnViewportResize(event.GetWidth(), event.GetHeight());

			m_ViewportBuffer->Resize(event.GetWidth(), event.GetHeight());

			return false;
		}
	};

	class StandaloneApplication : public Application
	{
	private:
		StandaloneLayer* m_Layer;

	public:
		StandaloneApplication()
			: Application({ MH_STANDALONE_TITLE, MH_STANDALONE_ICON })
		{
			// Create standalone layer
			m_Layer = new StandaloneLayer();

			PushOverlay(m_Layer);
		}
	};
}

extern Mahakam::Application* Mahakam::CreateApplication()
{
	return new Mahakam::StandaloneApplication();
}