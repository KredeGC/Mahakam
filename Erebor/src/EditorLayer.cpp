#include "ebpch.h"
#include "EditorLayer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam
{
	static Ref<Texture> loadOrCreateBRDF(const std::string& cachePath, uint32_t width, uint32_t height)
	{
		if (!std::filesystem::exists(cachePath))
		{
			// Setup BRDF LUT for lighting
			FrameBufferProps brdfProps;
			brdfProps.width = width;
			brdfProps.height = height;
			brdfProps.colorAttachments = { TextureFormat::RG16F };
			brdfProps.dontUseDepth = true;
			Ref<FrameBuffer> brdfFramebuffer = FrameBuffer::create(brdfProps);

			Ref<Shader> brdfShader = Shader::create("assets/shaders/internal/BRDF.glsl");

			Ref<Mesh> quadMesh = Mesh::createScreenQuad();

			brdfFramebuffer->bind();

			brdfShader->bind();
			quadMesh->bind();

			GL::clear();
			GL::drawIndexed(quadMesh->getIndexCount());

			brdfFramebuffer->unbind();

			Ref<Texture> brdfLut = std::static_pointer_cast<Texture>(brdfFramebuffer->getColorAttachments()[0]);

			// Save to cache
			uint32_t size = width * height * 4;
			char* pixels = new char[size];
			brdfLut->readPixels(pixels);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write(pixels, size);

			delete[] pixels;

			return brdfLut;
		}
		else
		{
			// Load from cache
			std::ifstream inStream(cachePath, std::ios::binary);
			std::stringstream ss;
			ss << inStream.rdbuf();
			Ref<Texture> brdfLut = Texture2D::create({ width, height, TextureFormat::RG16F, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
			brdfLut->setData((void*)ss.str().c_str(), ss.str().size());

			return brdfLut;
		}
	}

	static Ref<Texture> loadOrCreate(const std::string& cachePath, const std::string& src, bool saveMips, const CubeTextureProps& props)
	{
		if (!std::filesystem::exists(cachePath))
		{
			Ref<Texture> texture = TextureCube::create(src, props);

			uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution)));
			uint32_t maxMipLevels = saveMips ? mipLevels : 1;

			uint32_t size = 0;
			for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
			{
				uint32_t mipResolution = (uint32_t)(props.resolution * std::pow(0.5, mip));
				size += 6 * 6 * mipResolution * mipResolution;
			}

			// Save to cache
			char* pixels = new char[size];
			texture->readPixels(pixels, saveMips);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write(pixels, size);

			delete[] pixels;

			return texture;
		}
		else
		{
			// Load from cache
			std::ifstream stream(cachePath, std::ios::binary);
			std::stringstream ss;
			ss << stream.rdbuf();
			Ref<Texture> texture = TextureCube::create(props);
			texture->setData((void*)ss.str().c_str(), saveMips);

			stream.close();

			return texture;
		}
	}

	void EditorLayer::onAttach()
	{
		// Viewport framebuffer
		FrameBufferProps prop;
		prop.width = Application::getInstance().getWindow().getWidth();
		prop.height = Application::getInstance().getWindow().getHeight();
		prop.colorAttachments = { TextureFormat::RGB8 };
		viewportFramebuffer = FrameBuffer::create(prop);


		// Setup BRDF LUT for lighting
		Ref<Texture> brdfLut = loadOrCreateBRDF("assets/textures/brdf.dat", 512, 512);


		// Create a new active scene
		activeScene = Scene::createScene();


		// Setup the viewport in editor
		sceneViewPanel.setContext(activeScene, viewportFramebuffer);
		sceneHierarchyPanel.setContext(activeScene);


		// Setup shaders
		Ref<Shader> textureShader = Shader::create("assets/shaders/Albedo.glsl");
		Ref<Shader> shader = Shader::create("assets/shaders/LitColor.glsl");
		Ref<Shader> skyboxShader = Shader::create("assets/shaders/Skybox.glsl");


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


		// Setup scene cubemaps
		Ref<Texture> skyboxTexture = TextureCube::create("assets/textures/apt.hdr", { 2048, TextureFormat::RGB16F });
		Ref<Texture> skyboxIrradiance = loadOrCreate("assets/textures/apt.hdr.irradiance", "assets/textures/apt.hdr", false, { 32, TextureFormat::RGB16F, true, TextureCubePrefilter::Convolute });
		Ref<Texture> skyboxSpecular = loadOrCreate("assets/textures/apt.hdr.specular", "assets/textures/apt.hdr", true, { 256, TextureFormat::RGB16F, true, TextureCubePrefilter::Prefilter });


		// Create backpack model
		Ref<Model> backpackModel = Model::load("assets/models/backpack.obj", {
			{ ShaderSemantic::Position, "i_Pos" },
			{ ShaderSemantic::TexCoord0, "i_UV" },
			{ ShaderSemantic::Normal, "i_Normal" } });

		// Create backpack material
		Ref<Texture> backpackDiffuse = Texture2D::create("assets/textures/backpack/diffuse.jpg");
		Ref<Texture> backpackMetallic = Texture2D::create("assets/textures/backpack/specular.jpg", { TextureFormat::R8 });
		Ref<Texture> backpackRoughness = Texture2D::create("assets/textures/backpack/roughness.jpg", { TextureFormat::R8 });

		Ref<Material> backpackMaterial = Material::create(textureShader);
		backpackMaterial->setTexture("u_IrradianceMap", 0, skyboxIrradiance);
		backpackMaterial->setTexture("u_SpecularMap", 1, skyboxSpecular);
		backpackMaterial->setTexture("u_BRDFLUT", 2, brdfLut);
		backpackMaterial->setTexture("u_Albedo", 3, backpackDiffuse);
		backpackMaterial->setTexture("u_Metallic", 4, backpackMetallic);
		backpackMaterial->setTexture("u_Roughness", 5, backpackRoughness);

		// Create backpack script
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

		// Create backpack entities
		const auto& meshes = backpackModel->getMeshes();
		for (auto& mesh : meshes)
		{
			// Create backpack entity
			Entity entity = activeScene->createEntity("Bacpack");
			entity.addComponent<MeshComponent>(mesh, backpackMaterial);
			entity.getComponent<TransformComponent>().setPosition({ 4.5f, 4.0f, 5.0f });
			entity.addComponent<NativeScriptComponent>().bind<RotateScript>();
		}


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
				Entity entity = activeScene->createEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
				entity.addComponent<MeshComponent>(sphereMesh, material);
				entity.getComponent<TransformComponent>().setPosition({ x, y, 0.0f });
			}
		}


		// Setup scene skybox
		Ref<Mesh> skyboxMesh = Mesh::createScreenQuad();
		Ref<Material> skyboxMaterial = Material::create(skyboxShader);
		skyboxMaterial->setTexture("u_Environment", 0, skyboxTexture);

		Entity skybox = activeScene->createEntity("Skybox");
		skybox.addComponent<MeshComponent>(skyboxMesh, skyboxMaterial);
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