#include "mhpch.h"
#include "GL.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	GL* GL::s_Instance = nullptr;

	GL::GL()
	{
		MH_PROFILE_FUNCTION();

		rendererAPI = new OpenGLRendererAPI;
		rendererAPI->Init();

		staticScreenQuad = CreateScreenQuad();
		staticPyramid = CreatePyramid();
		staticSphereMesh = Mesh::CreateCubeSphere(5, true);
		staticCubemapMesh = Mesh::CreateCube(2, true);
		staticCube = Mesh::CreateCube(2);

		uint8_t whiteData = 255;

		Texture2D::white = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::white->SetData(&whiteData, 1);

		uint8_t blackData = 0;

		Texture2D::black = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::black->SetData(&blackData, 1);

		uint8_t bumpData[3]
		{
			127,
			127,
			255
		};

		Texture2D::bump = Texture2D::Create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::bump->SetData(&bumpData, 3);
	}

	GL::~GL()
	{
		//rendererAPI->Shutdown();

		staticScreenQuad = nullptr;
		staticPyramid = nullptr;
		staticSphereMesh = nullptr;
		staticCubemapMesh = nullptr;
		staticCube = nullptr;

		Texture2D::white = nullptr;
		Texture2D::black = nullptr;
		Texture2D::bump = nullptr;
		TextureCube::white = nullptr;
	}

	GL* GL::GetInstance()
	{
		MH_OVERRIDE_FUNC(GLGetInstance);

		return s_Instance;
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

		// Interleave vertices
		Mesh::InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;

		return Mesh::Create(4, 6, interleavedVertices, indices);
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

		// Interleave vertices
		Mesh::InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;

		return Mesh::Create(vertexCount, indexCount, interleavedVertices, indices);
	}
}