#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class SceneViewLayer : public Layer
	{
	private:
		Ref<Scene> activeScene;

		Ref<Mesh> skyboxDome;

		Transform transforms[100];
		Ref<Mesh> spheres[100];

		Ref<PerspectiveCamera> camera;
		Ref<Light> mainLight;

		uint32_t* drawCalls;
		uint32_t* vertexCount;
		uint32_t* triCount;

		Ref<FrameBuffer> brdfFramebuffer;
		Ref<FrameBuffer> viewportFramebuffer;

		glm::vec2 viewportSize = { 0.0f, 0.0f };

		bool open = true;
		bool focused = false;
		bool hovered = false;
		bool wireframe = false;

		float rotation = 0;

	public:
		SceneViewLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
			: Layer("SceneView"), drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount) {}

		virtual void onAttach() override;
		virtual void onUpdate(Timestep dt) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& event) override;

		bool onKeyPressed(KeyPressedEvent& event);
		bool onWindowResize(WindowResizeEvent& event);
		bool onMouseScrolled(MouseScrolledEvent& event);
	};
}