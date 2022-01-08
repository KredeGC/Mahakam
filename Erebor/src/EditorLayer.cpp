#include "ebpch.h"
#include "EditorLayer.h"

namespace Mahakam
{
	void EditorLayer::onAttach()
	{
		// Viewport framebuffer
		FrameBufferProps prop;
		prop.width = Application::getInstance().getWindow().getWidth();
		prop.height = Application::getInstance().getWindow().getHeight();
		prop.colorAttachments = { { TextureFormat::RGB8 } };
		viewportFramebuffer = FrameBuffer::create(prop);


		// Setup BRDF LUT for lighting
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


		// Create a new active scene
		activeScene = Scene::createScene();


		// Setup the viewport in editor
		sceneViewPanel.setContext(activeScene, viewportFramebuffer);
		sceneHierarchyPanel.setContext(activeScene);


		// Setup shaders
		Ref<Shader> shader = Shader::create("assets/shaders/LitColor.glsl");
		Ref<Shader> skyboxShader = Shader::create("assets/shaders/Skybox.glsl");


		// Setup scene camera
		cameraEntity = activeScene->createEntity("Camera");
		cameraEntity.addComponent<CameraComponent>(true, glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
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


		// Setup scene cubemaps
		Ref<Texture> skyboxTexture = TextureCube::create("assets/textures/apt.hdr", { 2048, TextureFormat::RGB8 });
		Ref<Texture> skyboxIrradiance = TextureCube::create("assets/textures/apt.hdr", { 32, TextureFormat::RGB8, true, TextureCubePrefilter::Convolute });
		Ref<Texture> skyboxSpecular = TextureCube::create("assets/textures/apt.hdr", { 256, TextureFormat::RGB8, true, TextureCubePrefilter::Prefilter });


		// Setup scene skybox
		Ref<Mesh> skyboxMesh = Mesh::createUVSphere(10, 10);
		Ref<Material> skyboxMaterial = Material::create(skyboxShader);
		skyboxMaterial->setTexture("u_Environment", 0, skyboxTexture);

		Entity skybox = activeScene->createEntity("Skybox");
		skybox.addComponent<MeshComponent>(skyboxMesh, skyboxMaterial);


		// Create mesh & base material
		Ref<Mesh> sphereMesh = Mesh::createCubeSphere(8);
		Ref<Material> baseMaterial = Material::create(shader);
		baseMaterial->setTexture("u_IrradianceMap", 0, skyboxIrradiance);
		baseMaterial->setTexture("u_SpecularMap", 1, skyboxSpecular);
		baseMaterial->setTexture("u_BRDFLUT", 2, brdfLut);
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
				Entity entity = activeScene->createEntity("Sphere");
				entity.addComponent<MeshComponent>(sphereMesh, material);
				entity.getComponent<TransformComponent>().setPosition({ x, y, 0.0f });
			}
		}
	}

	void EditorLayer::onUpdate(Timestep dt)
	{
		MH_PROFILE_FUNCTION();

		viewportFramebuffer->bind();

		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		activeScene->onUpdate(dt);

		viewportFramebuffer->unbind();

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



		// TEMPORARY
		ImGui::Begin("Camera");
		ImGui::SliderAngle("Rotation", &rotation, -180, 180);
		ImGui::End();

		/*TransformComponent& cameraTransform = cameraEntity.getComponent<TransformComponent>();

		cameraTransform.setRotation(glm::quat({ 0.0f, rotation, 0.0f }));

		glm::vec3 pos = { 4.5f, 4.5f, 0.0f };
		pos += cameraTransform.getForward() * 12.5f;

		cameraTransform.setPosition(pos);*/
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
		uint32_t width = event.getWidth();
		uint32_t height = event.getHeight();
		if (width > 0 && height > 0)
			viewportFramebuffer->resize(width, height);

		return false;
	}

	bool EditorLayer::onMouseScrolled(MouseScrolledEvent& event)
	{
		sceneViewPanel.onMouseScrolled(event);

		return false;
	}
}