#include <Mahakam.h>

#include <imgui.h>

// TEMPORARY
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Mahakam/Renderer/Mesh.h>

using namespace Mahakam;

class BasicLayer : public Layer
{
private:
	ShaderLibrary shaderLibrary;
	Ref<Mesh> mesh;

	OrthographicCamera camera;

public:
	BasicLayer() : Layer("Basic Quad"), camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		// Setup vertex position array
		float positions[3 * 4]
		{
			-0.5f, 0.0f, -0.5f,
			 0.5f, 0.0f, -0.5f,
			 0.5f, 0.0f,  0.5f,
			-0.5f, 0.0f,  0.5f
		};

		// Setup UV array
		float uvs[2 * 4]
		{
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		// Setup indices
		uint32_t indices[6]{ 0, 1, 2, 2, 3, 0 };

		// Setup vertex attribute layout
		BufferLayout layout = {
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" }
		};

		// Initialize mesh
		mesh = Mesh::create();
		mesh->setVertexCount(4);
		mesh->setLayout(layout);
		mesh->addVertices("i_Pos", (const char*)positions);
		mesh->addVertices("i_UV", (const char*)uvs);
		mesh->setIndices(indices, 6);
		mesh->init();
		

		// Setup texture
		Ref<Texture> albedo = Texture2D::create("assets/textures/fern.png");

		// Setup shader
		Ref<Shader> shader = shaderLibrary.load("assets/shaders/Albedo.glsl");

		// Setup material with texture
		Ref<Material> material = Material::create(shader);
		material->setTexture("u_Albedo", 0, albedo);


		// Set material in mesh
		mesh->setMaterial(material);
	}

	void onUpdate(Timestep dt) override
	{
		// Memory leak test
		/*float positions[3 * 4]
		{
			-0.5f, 0.0f, -0.5f,
			 0.5f, 0.0f, -0.5f,
			 0.5f, 0.0f,  0.5f,
			-0.5f, 0.0f,  0.5f
		};

		float uvs[2 * 4]
		{
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		BufferLayout layout = {
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" }
		};

		uint32_t indices[6]{ 0, 1, 2, 2, 3, 0 };

		mesh = Mesh::create();
		mesh->setVertexCount(4);
		mesh->setLayout(layout);
		mesh->setVertices("i_Pos", 0, (const char*)positions);
		mesh->setVertices("i_UV", 1, (const char*)uvs);
		mesh->setIndices(indices, 6);
		mesh->init();

		Ref<Texture> albedo = Texture2D::create("assets/textures/fern.png");

		Ref<Shader> shader = shaderLibrary.get("Albedo");

		Ref<Material> material = Material::create(shader);

		material->setTexture("u_Albedo", 0, albedo);

		mesh->setMaterial(material);*/





		// Camera movement
		if (Input::isKeyPressed(MH_KEY_LEFT))
			camera.setPosition(camera.getPosition() - glm::vec3({ dt, 0.0f, 0.0f }));
		else if (Input::isKeyPressed(MH_KEY_RIGHT))
			camera.setPosition(camera.getPosition() + glm::vec3({ dt, 0.0f, 0.0f }));

		if (Input::isKeyPressed(MH_KEY_UP))
			camera.setPosition(camera.getPosition() + glm::vec3({ 0.0f, 0.0f, dt }));
		else if (Input::isKeyPressed(MH_KEY_DOWN))
			camera.setPosition(camera.getPosition() - glm::vec3({ 0.0f, 0.0f, dt }));

		// Camera rotation
		glm::quat rot({ glm::radians(45.0f), 0.0f, 0.0f });

		camera.setRotation(rot);

		// Clearing screen
		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		// Scene setup
		Renderer::beginScene(camera);

		// Mesh transform setup
		glm::quat rotation({ 0.0f, glm::radians(-45.0f), 0.0f });

		glm::mat4 modelMatrix = glm::mat4(rotation)
			* glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

		// Submitting to render queue
		Renderer::submit(modelMatrix, mesh);

		Renderer::endScene();
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
	unsigned int drawCalls = 0, vertexCount = 0, indexCount = 0;
	bool open = true;

public:
	StatsLayer() : Layer("Stats") {}

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
			ImGui::Text("Drawcalls: %d", drawCalls);
			ImGui::Text("Vertex count: %d", vertexCount);
			ImGui::Text("Frametime: %d fps (%.4g ms)", (int)(1.0f / frametime), frametime.getMilliSeconds());
			ImGui::End();
		}
	}
};

class Sandbox : public Application
{
public:
	Sandbox()
	{
		pushLayer(new BasicLayer());
		pushOverlay(new DockingLayer());
		pushOverlay(new StatsLayer());

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