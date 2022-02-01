#include "mhpch.h"
#include "GL.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	RendererAPI* GL::rendererAPI = new OpenGLRendererAPI();

	Ref<Mesh> GL::staticScreenQuad = nullptr;
	Ref<Mesh> GL::staticPyramid = nullptr;
	Ref<Mesh> GL::staticSphereMesh = nullptr;
	Ref<Mesh> GL::staticCubemapMesh = nullptr;

	void GL::init()
	{
		rendererAPI->init();

		staticScreenQuad = CreateScreenQuad();
		staticPyramid = CreatePyramid();
		staticSphereMesh = Mesh::createCubeSphere(5, true);
		staticCubemapMesh = Mesh::createCube(2, true);

		uint8_t whiteData = 255;

		Texture2D::white = Texture2D::create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::white->setData(&whiteData, 1);

		uint8_t blackData = 0;

		Texture2D::black = Texture2D::create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::black->setData(&blackData, 1);

		uint8_t bumpData[3]
		{
			127,
			127,
			255
		};

		Texture2D::bump = Texture2D::create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::bump->setData(&bumpData, 3);
	}

	void GL::Shutdown()
	{
		//rendererAPI->Shutdown();

		staticScreenQuad = nullptr;
		staticPyramid = nullptr;
		staticSphereMesh = nullptr;
		staticCubemapMesh = nullptr;

		Texture2D::white = nullptr;
		Texture2D::black = nullptr;
		Texture2D::bump = nullptr;
		TextureCube::white = nullptr;
	}

	Ref<Mesh> GL::CreateScreenQuad()
	{
		MH_PROFILE_FUNCTION();

		glm::vec3 positions[] = {
			{ -1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f  },
			{ -1.0f, -1.0f, 0.0f },
			{ 1.0f, -1.0f, 0.0f }
		};

		glm::vec2 uvs[] = {
			{ 0.0f, 1.0f, },
			{ 1.0f, 1.0f, },
			{ 0.0f, 0.0f, },
			{ 1.0f, 0.0f }
		};

		uint32_t indices[] = {
			0, 2, 3,
			0, 3, 1
		};

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" }
		};

		return Mesh::create(4, indices, 6, { positions, uvs });
	}

	Ref<Mesh> GL::CreatePyramid()
	{
		MH_PROFILE_FUNCTION();

		const uint32_t vertexCount = 5;
		const uint32_t indexCount = 18;

		glm::vec3 positions[vertexCount];
		uint32_t indices[indexCount];

		positions[0] = { 0.0f, 0.0f, 0.0f };
		positions[1] = { -1.0f, -1.0f, -1.0f };
		positions[2] = { 1.0f, -1.0f, -1.0f };
		positions[3] = { 1.0f, 1.0f, -1.0f };
		positions[4] = { -1.0f, 1.0f, -1.0f };

		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;

		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		indices[6] = 0;
		indices[7] = 4;
		indices[8] = 3;

		indices[9] = 0;
		indices[10] = 1;
		indices[11] = 4;

		indices[12] = 1;
		indices[13] = 2;
		indices[14] = 3;

		indices[15] = 1;
		indices[16] = 3;
		indices[17] = 4;

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" }
		};

		return Mesh::create(vertexCount, indices, indexCount, { positions });
	}
}