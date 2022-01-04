#include "SceneViewLayer.h"

// TEMPORARY
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	void SceneViewLayer::onAttach()
	{
		// Debug stuff
		FrameBufferProps prop;
		prop.width = Application::getInstance().getWindow().getWidth();
		prop.height = Application::getInstance().getWindow().getHeight();
		viewportFramebuffer = FrameBuffer::create(prop);



		// Setup camera
		camera = std::make_shared<PerspectiveCamera>(glm::radians(45.0f), 1.6f / 0.9f, 0.01f, 100.0f);
		camera->setPosition({ 4.5f, 4.5f, 12.5f });

		// Setup lights
		mainLight = std::make_shared<Light>(glm::vec3(1.0f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

		// Setup shader
		Ref<Shader> shader = Shader::create("assets/shaders/LitColor.glsl");
		Ref<Shader> skyboxShader = Shader::create("assets/shaders/Skybox.glsl");

		// Setup texture
		Ref<Texture> skybox = TextureCube::create("assets/textures/studio.hdr");

		// Setup skybox
		skyboxDome = Mesh::createUVSphere(20, 20);
		Ref<Material> skyboxMaterial = Material::create(skyboxShader);
		skyboxMaterial->setTexture("u_Environment", 0, skybox);
		skyboxDome->setMaterial(skyboxMaterial);

		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				// Setup material with texture
				Ref<Material> material = Material::create(shader);
				//material->setTexture("u_Albedo", 0, tex);
				material->setTexture("u_IrradianceMap", 0, skybox);
				material->setFloat3("u_Color", { 1.0f, 0.7f, 0.0f });
				material->setFloat("u_Metallic", y / 9.0f);
				material->setFloat("u_Roughness", x / 9.0f);

				// Create sphere
				Ref<Mesh> sphereMesh = Mesh::createCubeSphere(8);
				sphereMesh->setMaterial(material);

				transforms[y + x * 10].setPosition({ x, y, 0.0f });
				spheres[y + x * 10] = sphereMesh;
			}
		}
	}

	void SceneViewLayer::onUpdate(Timestep dt)
	{
		MH_PROFILE_FUNCTION();

		viewportFramebuffer->bind();
		// Clearing screen
		{
			MH_PROFILE_SCOPE("Renderer Clear");
			GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
			GL::clear();
		}

		// Scene draw
		{
			MH_PROFILE_SCOPE("Renderer Draw");
			Renderer::beginScene(camera, mainLight);

			Renderer::submit(glm::scale(glm::mat4(1.0f), { 50.0f, 50.0f, 50.0f }), skyboxDome);

			for (int i = 0; i < 100; i++)
				Renderer::submit(transforms[i].getModelMatrix(), spheres[i]);

			Renderer::endScene(drawCalls, vertexCount, triCount);
		}
		viewportFramebuffer->unbind();
	}

	void SceneViewLayer::onImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != viewportSize.x || size.y != viewportSize.y)
		{
			viewportSize.x = size.x;
			viewportSize.y = size.y;

			camera->setRatio(size.x / size.y);
		}
		ImGui::Image((void*)viewportFramebuffer->getColorAttachments()[0], size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		ImGui::PopStyleVar();



		// TEMPORARY
		ImGui::Begin("Camera");
		ImGui::SliderAngle("Rotation", &rotation, -180, 180);
		ImGui::End();

		camera->setRotation(glm::quat({ 0.0f, rotation, 0.0f }));

		glm::vec3 pos = { 4.5f, 4.5f, 0.0f };
		pos += camera->getForward() * 12.5f;

		camera->setPosition(pos);
	}

	void SceneViewLayer::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.dispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(SceneViewLayer::onKeyPressed));
		dispatcher.dispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(SceneViewLayer::onWindowResize));
	}

	bool SceneViewLayer::onKeyPressed(KeyPressedEvent& event)
	{
		if (event.getKeyCode() == MH_KEY_F5)
		{
			GL::setFillMode(wireframe);
			wireframe = !wireframe;
		}

		return false;
	}

	bool SceneViewLayer::onWindowResize(WindowResizeEvent& event)
	{
		viewportFramebuffer->resize(event.getWidth(), event.getHeight());

		return false;
	}
}