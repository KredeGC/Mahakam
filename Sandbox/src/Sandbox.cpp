#include <Mahakam.h>

#include <imgui.h>

// TEMPORARY
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Mahakam/Renderer/Model.h>

using namespace Mahakam;

class BasicLayer : public Layer
{
private:
	ShaderLibrary shaderLibrary;
	Ref<Model> model;

	uint32_t* drawCalls;
	uint32_t* vertexCount;
	uint32_t* triCount;

	PerspectiveCamera camera;

	bool wireframe = false;

public:
	BasicLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
		: Layer("Basic Quad"), camera(1.6f / 0.9f, 0.01f, 100.0f), drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount)
	{
		//mesh = Mesh::createCube(2);


		MeshLayout layout
		{
			{ ShaderSemantic::Position, "i_Pos" },
			{ ShaderSemantic::TexCoord0, "i_UV" },
			{ ShaderSemantic::Normal, "i_Normal" }
		};

		model = Model::load("assets/models/backpack.obj", layout);


		// Setup texture
		Ref<Texture> albedo = Texture2D::create("assets/textures/backpack/diffuse.jpg");

		// Setup shader
		Ref<Shader> shader = shaderLibrary.load("assets/shaders/Albedo.glsl");

		// Setup material with texture
		Ref<Material> material = Material::create(shader);
		material->setTexture("u_Albedo", 0, albedo);


		// Set material in mesh
		for (auto& mesh : model->getMeshes())
			mesh->setMaterial(material);
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
		if (Input::isKeyPressed(MH_KEY_LEFT))
			camera.setPosition(camera.getPosition() - glm::vec3({ dt, 0.0f, 0.0f }));
		else if (Input::isKeyPressed(MH_KEY_RIGHT))
			camera.setPosition(camera.getPosition() + glm::vec3({ dt, 0.0f, 0.0f }));

		if (Input::isKeyPressed(MH_KEY_UP))
			camera.setPosition(camera.getPosition() - glm::vec3({ 0.0f, 0.0f, dt }));
		else if (Input::isKeyPressed(MH_KEY_DOWN))
			camera.setPosition(camera.getPosition() + glm::vec3({ 0.0f, 0.0f, dt }));

		// Camera rotation
		glm::quat rot({ glm::radians(-45.0f), 0.0f, 0.0f });

		camera.setRotation(rot);

		// Clearing screen
		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		// Scene setup
		Renderer::beginScene(camera);

		// Mesh transform setup
		glm::quat rotation({ 0.0f, glm::radians(-45.0f), 0.0f });

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), { 0.0f, -0.5f, 0.0f })
			* glm::mat4(rotation)
			* glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });

		// Submitting to render queue
		for (auto& mesh : model->getMeshes())
			Renderer::submit(modelMatrix, mesh);

		Renderer::endScene(drawCalls, vertexCount, triCount);
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

	void onImGuiRender()
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
		pushLayer(new BasicLayer(&drawCalls, &vertexCount, &triCount));
		pushOverlay(new DockingLayer());
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