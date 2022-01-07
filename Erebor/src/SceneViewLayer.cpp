#include "SceneViewLayer.h"

namespace Mahakam
{
	void SceneViewLayer::onAttach()
	{
		activeScene = Scene::createScene();

		Entity entity = activeScene->createEntity();
		entity.hasComponent<MeshComponent>();
		entity.addComponent<MeshComponent>();



		// Setup BRDF LUT
		FrameBufferProps brdfProps;
		brdfProps.width = 512;
		brdfProps.height = 512;
		brdfProps.colorAttachments = { { TextureFormat::RG16F } };
		brdfProps.depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true };
		Ref<FrameBuffer> brdfFramebuffer = FrameBuffer::create(brdfProps);

		Ref<Shader> brdfShader = Shader::create("assets/shaders/internal/BRDF.glsl");

		Ref<Mesh> quadMesh = Mesh::createQuad();

		brdfFramebuffer->bind();

		brdfShader->bind();
		quadMesh->bind();

		GL::clear();
		GL::drawIndexed(quadMesh->getIndexCount());

		brdfFramebuffer->unbind();

		Ref<Texture> brdfLut = std::static_pointer_cast<Texture>(brdfFramebuffer->getColorAttachments()[0]);

		// Viewport framebuffer
		FrameBufferProps prop;
		prop.width = Application::getInstance().getWindow().getWidth();
		prop.height = Application::getInstance().getWindow().getHeight();
		prop.colorAttachments = { { TextureFormat::RGB8 } };
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
		Ref<Texture> fern = Texture2D::create("assets/textures/fern.png", { TextureFormat::RGB8 });
		Ref<Texture> skybox = TextureCube::create("assets/textures/apt.hdr", { 2048, TextureFormat::RGB8 });
		Ref<Texture> skyboxIrradiance = TextureCube::create("assets/textures/apt.hdr", { 32, TextureFormat::RGB8, true, TextureCubePrefilter::Convolute });
		Ref<Texture> skyboxSpecular = TextureCube::create("assets/textures/apt.hdr", { 1024, TextureFormat::RGB8, true, TextureCubePrefilter::Prefilter });

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
				material->setTexture("u_IrradianceMap", 0, skyboxIrradiance);
				material->setTexture("u_SpecularMap", 1, skyboxSpecular);
				material->setTexture("u_BRDFLUT", 2, brdfLut);
				material->setFloat3("u_Color", { 0.5f, 0.0f, 0.0f });
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

		activeScene->onUpdate(dt);

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
		ImGui::Begin("Viewport", &open);
		focused = ImGui::IsWindowFocused();
		hovered = ImGui::IsWindowHovered();
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != viewportSize.x || size.y != viewportSize.y)
		{
			viewportSize.x = size.x;
			viewportSize.y = size.y;

			camera->setRatio(size.x / size.y);
		}
		ImGui::Image((void*)viewportFramebuffer->getColorAttachments()[0]->getRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
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
		dispatcher.dispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(SceneViewLayer::onMouseScrolled));
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
		uint32_t width = event.getWidth();
		uint32_t height = event.getHeight();
		if (width > 0 && height > 0)
			viewportFramebuffer->resize(width, height);

		return false;
	}

	bool SceneViewLayer::onMouseScrolled(MouseScrolledEvent& event)
	{
		if (focused && hovered)
		{
			MH_CORE_TRACE("Scrolled!");
			return true;
		}

		return false;
	}
}