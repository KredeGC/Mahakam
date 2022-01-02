#include <Mahakam.h>

#include <imgui.h>

// TEMPORARY
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Mahakam;

class BasicLayer : public Layer
{
private:
	ShaderLibrary shaderLibrary;
	Ref<Model> model;
	Ref<Mesh> planeMesh;
	Ref<Material> backpackMat;

	uint32_t* drawCalls;
	uint32_t* vertexCount;
	uint32_t* triCount;

	float metallic = 0.0f;
	float roughness = 0.5f;

	Ref<PerspectiveCamera> camera;
	Ref<Light> mainLight;

	bool wireframe = false;

public:
	BasicLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
		: Layer("Basic Renderer"), drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount)
	{
		camera = std::make_shared<PerspectiveCamera>(glm::radians(45.0f), 1.6f / 0.9f, 0.01f, 100.0f);
		mainLight = std::make_shared<Light>(glm::vec3(1.0f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

		// Create base plane
		//planeMesh = Mesh::createPlane(10, 10);
		planeMesh = Mesh::createUVSphere(100, 100);
		//planeMesh = Mesh::createCube(2);

		// Setup shader
		Ref<Shader> shader = shaderLibrary.load("assets/shaders/Albedo.glsl");

		// Setup texture
		Ref<Texture> planeTex = Texture2D::create("assets/textures/fern.png");

		// Setup material with texture
		Ref<Material> planeMaterial = Material::create(shader);
		planeMaterial->setTexture("u_Albedo", 0, planeTex);
		planeMaterial->setFloat("u_Metallic", 0.0f);
		planeMaterial->setFloat("u_Roughness", 1.0f);

		planeMesh->setMaterial(planeMaterial);



		// Load model
		MeshLayout layout
		{
			{ ShaderSemantic::Position, "i_Pos" },
			{ ShaderSemantic::TexCoord0, "i_UV" },
			{ ShaderSemantic::Normal, "i_Normal" }
		};

		model = Model::load("assets/models/backpack.obj", layout);


		// Setup texture
		Ref<Texture> backpackTex = Texture2D::create("assets/textures/backpack/diffuse.jpg");

		// Setup material with texture
		backpackMat = Material::create(shader);
		backpackMat->setTexture("u_Albedo", 0, backpackTex);
		//material->setFloat3("u_LightPosition", mainLight->getPosition());
		//material->setFloat3("u_LightColor", mainLight->getColor());


		// Set material in mesh
		for (auto& mesh : model->getMeshes())
			mesh->setMaterial(backpackMat);
	}

	void onEvent(Event& event) override
	{
		if (event.getEventType() == EventType::KeyPressed)
		{
			KeyPressedEvent& keyPressed = (KeyPressedEvent&)event;

			if (keyPressed.getKeyCode() == MH_KEY_F5)
			{
				GL::setFillMode(wireframe);
				wireframe = !wireframe;
			}
		}
	}

	void onUpdate(Timestep dt) override
	{
		// Camera movement
		if (Input::isKeyPressed(MH_KEY_A))
			camera->setPosition(camera->getPosition() - glm::vec3(dt) * camera->getRight());
		else if (Input::isKeyPressed(MH_KEY_D))
			camera->setPosition(camera->getPosition() + glm::vec3(dt) * camera->getRight());

		if (Input::isKeyPressed(MH_KEY_W))
			camera->setPosition(camera->getPosition() - glm::vec3(dt) * camera->getForward());
		else if (Input::isKeyPressed(MH_KEY_S))
			camera->setPosition(camera->getPosition() + glm::vec3(dt) * camera->getForward());

		// Camera rotation
		if (Input::isKeyPressed(MH_KEY_LEFT))
			camera->setRotation(camera->getRotation() * glm::quat({ 0.0f, dt, 0.0f }));
		else if (Input::isKeyPressed(MH_KEY_RIGHT))
			camera->setRotation(camera->getRotation() * glm::quat({ 0.0f, -dt, 0.0f }));

		if (Input::isKeyPressed(MH_KEY_UP))
			camera->setRotation(camera->getRotation() * glm::quat({ dt, 0.0f, 0.0f }));
		else if (Input::isKeyPressed(MH_KEY_DOWN))
			camera->setRotation(camera->getRotation() * glm::quat({ -dt, 0.0f, 0.0f }));

		// Clearing screen
		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		// Scene setup
		Renderer::beginScene(camera, mainLight);

		// Render base plane
		Renderer::submit(glm::translate(glm::mat4(1.0f), { 0.0f, -0.5f, 0.0f })
			* glm::scale(glm::mat4(1.0f), { 5.0f, 5.0, 5.0f }), planeMesh);

		// Render many objects
		for (int i = 0; i < 50; i++)
		{
			// Mesh transform setup
			glm::quat rotation({ 0.0f, glm::radians(-45.0f), 0.0f });

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), { (float)(i * 0.05f), -0.5f, 0.0f })
				* glm::mat4(rotation)
				* glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });

			// Submitting to render queue
			for (auto& mesh : model->getMeshes())
				Renderer::submit(modelMatrix, mesh);
		}

		Renderer::endScene(drawCalls, vertexCount, triCount);
	}

	void onImGuiRender() override
	{
		ImGui::Begin("Material");
		ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
		ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
		ImGui::End();

		backpackMat->setFloat("u_Metallic", metallic);
		backpackMat->setFloat("u_Roughness", roughness);
	}
};

class TestLightingLayer : public Layer
{
private:
	Ref<Mesh> skyboxDome;

	Transform transforms[100];
	Ref<Mesh> spheres[100];

	Ref<PerspectiveCamera> camera;
	Ref<Light> mainLight;

	uint32_t* drawCalls;
	uint32_t* vertexCount;
	uint32_t* triCount;

	bool wireframe = false;

	float rotation = 0;

public:
	TestLightingLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
		: Layer("Lighting Test"), drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount)
	{
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
				Ref<Mesh> sphereMesh = Mesh::createCubeSphere(8.0f);
				sphereMesh->setMaterial(material);

				transforms[y + x * 10].setPosition({ x, y, 0.0f });
				spheres[y + x * 10] = sphereMesh;
			}
		}
	}

	void onEvent(Event& event) override
	{
		if (event.getEventType() == EventType::KeyPressed)
		{
			KeyPressedEvent& keyPressed = (KeyPressedEvent&)event;

			if (keyPressed.getKeyCode() == MH_KEY_F5)
			{
				GL::setFillMode(wireframe);
				wireframe = !wireframe;
			}
		}
	}

	void onUpdate(Timestep dt) override
	{
		// Clearing screen
		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		// Scene setup
		Renderer::beginScene(camera, mainLight);

		Renderer::submit(glm::scale(glm::mat4(1.0f), { 50.0f, 50.0f, 50.0f }), skyboxDome);

		for (int i = 0; i < 100; i++)
			Renderer::submit(transforms[i].getModelMatrix(), spheres[i]);

		Renderer::endScene(drawCalls, vertexCount, triCount);
	}

	void onImGuiRender()
	{
		ImGui::Begin("Camera");
		ImGui::SliderAngle("Rotation", &rotation, -180, 180);
		ImGui::End();

		camera->setRotation(glm::quat({ 0.0f, rotation, 0.0f }));
		
		glm::vec3 pos = { 4.5f, 4.5f, 0.0f };
		pos += camera->getForward() * 12.5f;

		camera->setPosition(pos);
	}
};

class DockingLayer : public Layer
{
private:
	bool open = true;
	bool opt_fullscreen = true;
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

public:
	DockingLayer() : Layer("Dock") {}

	void onImGuiRender() override
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", &open, window_flags);

		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGui::End();
	}
};

class StatsLayer : public Layer
{
private:
	Timestep frametime = 0.0f;
	uint32_t* drawCalls;
	uint32_t* vertexCount;
	uint32_t* triCount;
	bool open = true;

public:
	StatsLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
		: drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount), Layer("Stats") {}

	void onUpdate(Timestep dt)
	{
		frametime = dt;
	}

	void onImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Stats", &open);
			ImGui::Text("Graphics unit: %s", GL::getGraphicsVendor());
			ImGui::Text("Drawcalls: %d", *drawCalls);
			ImGui::Text("Vertex count: %d", *vertexCount);
			ImGui::Text("Tri count: %d", *triCount);
			ImGui::Text("Frametime: %d fps (%.4g ms)", (int)(1.0f / frametime), frametime.getMilliSeconds());
			ImGui::End();
		}
	}
};

class Sandbox : public Application
{
private:
	uint32_t drawCalls = 0;
	uint32_t vertexCount = 0;
	uint32_t triCount = 0;

public:
	Sandbox()
	{
		pushLayer(new DockingLayer());
		pushLayer(new TestLightingLayer(&drawCalls, &vertexCount, &triCount));
		//pushLayer(new BasicLayer(&drawCalls, &vertexCount, &triCount));
		pushOverlay(new StatsLayer(&drawCalls, &vertexCount, &triCount));

		//getWindow().setVSync(true);
	}

	~Sandbox()
	{

	}
};

extern Application* Mahakam::createApplication()
{
	return new Sandbox();
}