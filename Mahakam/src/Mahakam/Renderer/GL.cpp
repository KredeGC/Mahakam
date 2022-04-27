#include "mhpch.h"
#include "GL.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	RendererAPI* GL::rendererAPI;

	Ref<Mesh> GL::staticScreenQuad;
	Ref<Mesh> GL::staticPyramid;
	Ref<Mesh> GL::staticSphereMesh;
	Ref<Mesh> GL::staticCubemapMesh;
	Ref<Mesh> GL::staticCube;

	Ref<Texture2D> GL::texture2DRed;
	Ref<Texture2D> GL::texture2DWhite;
	Ref<Texture2D> GL::texture2DBlack;
	Ref<Texture2D> GL::texture2DBump;
	Ref<TextureCube> GL::textureCubeWhite;

	void GL::Init()
	{
		MH_PROFILE_FUNCTION();

		rendererAPI = new OpenGLRendererAPI;
		rendererAPI->Init();

		staticScreenQuad = CreateScreenQuad();
		staticPyramid = CreatePyramid();
		staticSphereMesh = Mesh::CreateCubeSphere(5, true);
		staticCubemapMesh = Mesh::CreateCube(2, true);
		staticCube = Mesh::CreateCube(2);

		uint8_t redData = 255;

		texture2DRed = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		texture2DRed->SetData(&redData, 1);

		uint8_t whiteData[3]
		{
			255,
			255,
			255
		};

		texture2DWhite = Texture2D::Create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		texture2DWhite->SetData(whiteData, 0);

		uint8_t blackData = 0;

		texture2DBlack = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		texture2DBlack->SetData(&blackData, 1);

		uint8_t bumpData[3]
		{
			127,
			127,
			255
		};

		texture2DBump = Texture2D::Create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		texture2DBump->SetData(&bumpData, 3);

		textureCubeWhite = TextureCube::Create({ 1, TextureFormat::RGB8, TextureFilter::Point, false });
		textureCubeWhite->SetData(whiteData, 0);
	}

	void GL::Shutdown()
	{
		//rendererAPI->Shutdown();
		delete rendererAPI;

		staticScreenQuad = nullptr;
		staticPyramid = nullptr;
		staticSphereMesh = nullptr;
		staticCubemapMesh = nullptr;
		staticCube = nullptr;

		texture2DRed = nullptr;
		texture2DWhite = nullptr;
		texture2DBlack = nullptr;
		texture2DBump = nullptr;
		textureCubeWhite = nullptr;
	}

#pragma region Mesh
	//Ref<Mesh> GL::GetScreenQuad()
	MH_DEFINE_FUNC(GL::GetScreenQuad, Ref<Mesh>)
	{
		return staticScreenQuad;
	};

	//Ref<Mesh> GL::GetInvertedPyramid()
	MH_DEFINE_FUNC(GL::GetInvertedPyramid, Ref<Mesh>)
	{
		return staticPyramid;
	};

	//Ref<Mesh> GL::GetInvertedSphere()
	MH_DEFINE_FUNC(GL::GetInvertedSphere, Ref<Mesh>)
	{
		return staticSphereMesh;
	};

	//Ref<Mesh> GL::GetInvertedCube()
	MH_DEFINE_FUNC(GL::GetInvertedCube, Ref<Mesh>)
	{
		return staticCubemapMesh;
	};

	//Ref<Mesh> GL::GetCube()
	MH_DEFINE_FUNC(GL::GetCube, Ref<Mesh>)
	{
		return staticCube;
	};
#pragma endregion

#pragma region Texture
	//Ref<Texture2D> GL::GetTexture2DRed()
	MH_DEFINE_FUNC(GL::GetTexture2DRed, Ref<Texture2D>)
	{
		return texture2DRed;
	};

	//Ref<Texture2D> GL::GetTexture2DWhite()
	MH_DEFINE_FUNC(GL::GetTexture2DWhite, Ref<Texture2D>)
	{
		return texture2DWhite;
	};

	//Ref<Texture2D> GL::GetTexture2DBlack()
	MH_DEFINE_FUNC(GL::GetTexture2DBlack, Ref<Texture2D>)
	{
		return texture2DBlack;
	};

	//Ref<Texture2D> GL::GetTexture2DBump()
	MH_DEFINE_FUNC(GL::GetTexture2DBump, Ref<Texture2D>)
	{
		return texture2DBump;
	};

	//Ref<TextureCube> GL::GetTextureCubeWhite()
	MH_DEFINE_FUNC(GL::GetTextureCubeWhite, Ref<TextureCube>)
	{
		return textureCubeWhite;
	};
#pragma endregion

	//const char* GL::GetGraphicsVendor()
	MH_DEFINE_FUNC(GL::GetGraphicsVendor, const char*)
	{
		return rendererAPI->GetGraphicsVendor();
	};

	//void GL::SetViewportImpl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor)
	MH_DEFINE_FUNC(GL::SetViewportImpl, void, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor)
	{
		rendererAPI->SetViewport(x, y, w, h, scissor);
	};

	//void GL::FinishRendering()
	MH_DEFINE_FUNC(GL::FinishRendering, void)
	{
		rendererAPI->FinishRendering();
	};

	//void GL::SetClearColor(const glm::vec4& color)
	MH_DEFINE_FUNC(GL::SetClearColor, void, const glm::vec4& color)
	{
		rendererAPI->SetClearColor(color);
	};

	//void GL::ClearImpl(bool color, bool depth)
	MH_DEFINE_FUNC(GL::ClearImpl, void, bool color, bool depth)
	{
		rendererAPI->Clear(color, depth);
	};

	//void GL::EnableCullingImpl(bool enable, bool cullFront)
	MH_DEFINE_FUNC(GL::EnableCullingImpl, void, bool enable, bool cullFront)
	{
		rendererAPI->EnableCulling(enable, cullFront);
	};

	//void GL::EnableZWriting(bool enable)
	MH_DEFINE_FUNC(GL::EnableZWriting, void, bool enable)
	{
		rendererAPI->EnableZWriting(enable);
	};

	//void GL::EnableZTesting(bool enable)
	MH_DEFINE_FUNC(GL::EnableZTesting, void, bool enable)
	{
		rendererAPI->EnableZTesting(enable);
	};

	//void GL::SetFillMode(bool enable)
	MH_DEFINE_FUNC(GL::SetFillMode, void, bool enable)
	{
		rendererAPI->SetFillMode(enable);
	};

	//void GL::SetBlendMode(RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable)
	MH_DEFINE_FUNC(GL::SetBlendMode, void, RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable)
	{
		rendererAPI->SetBlendMode(src, dst, enable);
	};

#pragma region Draw
	//void GL::DrawScreenQuad()
	MH_DEFINE_FUNC(GL::DrawScreenQuad, void)
	{
		staticScreenQuad->Bind();
		rendererAPI->DrawIndexed(staticScreenQuad->GetIndexCount());
	};

	//void GL::DrawIndexed(uint32_t indexCount)
	MH_DEFINE_FUNC(GL::DrawIndexed, void, uint32_t indexCount)
	{
		rendererAPI->DrawIndexed(indexCount);
	};

	//void GL::DrawInstanced(uint32_t indexCount, uint32_t count)
	MH_DEFINE_FUNC(GL::DrawInstanced, void, uint32_t indexCount, uint32_t count)
	{
		rendererAPI->DrawInstanced(indexCount, count);
	};
#pragma endregion

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