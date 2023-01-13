#include "Mahakam/mhpch.h"
#include "GL.h"

#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Mesh.h"
#include "Texture.h"

namespace Mahakam
{
	Scope<RendererAPI> GL::s_RendererAPI;

	Ref<SubMesh> GL::s_StaticScreenQuad;
	Ref<SubMesh> GL::s_StaticSphereMesh;
	Ref<SubMesh> GL::s_StaticInvertedPyramid;
	Ref<SubMesh> GL::s_StaticInvertedSphereMesh;
	Ref<SubMesh> GL::s_StaticInvertedCubemapMesh;
	Ref<SubMesh> GL::s_StaticCube;
	
	Asset<Texture2D> GL::s_Texture2DRed;
	Asset<Texture2D> GL::s_Texture2DWhite;
	Asset<Texture2D> GL::s_Texture2DBlack;
	Asset<Texture2D> GL::s_Texture2DBump;
	Asset<TextureCube> GL::s_TextureCubeWhite;
	Asset<TextureCube> GL::s_TextureCubeGrey;

	void GL::Init()
	{
		MH_PROFILE_FUNCTION();

		s_RendererAPI = RendererAPI::Create();
		s_RendererAPI->Init();

		s_StaticScreenQuad = CreateScreenQuad();
		s_StaticSphereMesh = SubMesh::CreateUVSphere(10, 10);
		s_StaticInvertedPyramid = CreatePyramid();
		s_StaticInvertedSphereMesh = SubMesh::CreateCubeSphere(5, true);
		s_StaticInvertedCubemapMesh = SubMesh::CreateCube(2, true);
		s_StaticCube = SubMesh::CreateCube(2);

		uint8_t redData = 255;

		uint8_t whiteData[3]
		{
			255,
			255,
			255
		};

		uint8_t blackData = 0;

		uint8_t bumpData[3]
		{
			127,
			127,
			255
		};

		uint8_t greyData[3]
		{
			127,
			127,
			127
		};

		s_Texture2DRed = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		s_Texture2DRed->SetData(&redData, 0);

		s_Texture2DWhite = Texture2D::Create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		s_Texture2DWhite->SetData(whiteData, 0);

		s_Texture2DBlack = Texture2D::Create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		s_Texture2DBlack->SetData(&blackData, 0);

		s_Texture2DBump = Texture2D::Create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		s_Texture2DBump->SetData(&bumpData, 0);

		s_TextureCubeWhite = TextureCube::Create({ 1, TextureFormat::RGB8, TextureFilter::Point, TextureCubePrefilter::None, false });
		s_TextureCubeWhite->SetData(whiteData, 0);

		s_TextureCubeGrey = TextureCube::Create({ 1, TextureFormat::RGB8, TextureFilter::Point, TextureCubePrefilter::None, false });
		s_TextureCubeGrey->SetData(greyData, 0);
	}

	void GL::Shutdown()
	{
		s_StaticScreenQuad = nullptr;
		s_StaticSphereMesh = nullptr;
		s_StaticInvertedPyramid = nullptr;
		s_StaticInvertedSphereMesh = nullptr;
		s_StaticInvertedCubemapMesh = nullptr;
		s_StaticCube = nullptr;

		s_Texture2DRed = nullptr;
		s_Texture2DWhite = nullptr;
		s_Texture2DBlack = nullptr;
		s_Texture2DBump = nullptr;
		s_TextureCubeWhite = nullptr;
		s_TextureCubeGrey = nullptr;
	}

#pragma region Mesh
	//Ref<Mesh> GL::GetScreenQuad()
	MH_DEFINE_FUNC(GL::GetScreenQuad, Ref<SubMesh>)
	{
		return s_StaticScreenQuad;
	};

	//Ref<Mesh> GL::GetSphere()
	MH_DEFINE_FUNC(GL::GetSphere, Ref<SubMesh>)
	{
		return s_StaticSphereMesh;
	};

	//Ref<Mesh> GL::GetInvertedPyramid()
	MH_DEFINE_FUNC(GL::GetInvertedPyramid, Ref<SubMesh>)
	{
		return s_StaticInvertedPyramid;
	};

	//Ref<Mesh> GL::GetInvertedSphere()
	MH_DEFINE_FUNC(GL::GetInvertedSphere, Ref<SubMesh>)
	{
		return s_StaticInvertedSphereMesh;
	};

	//Ref<Mesh> GL::GetInvertedCube()
	MH_DEFINE_FUNC(GL::GetInvertedCube, Ref<SubMesh>)
	{
		return s_StaticInvertedCubemapMesh;
	};

	//Ref<Mesh> GL::GetCube()
	MH_DEFINE_FUNC(GL::GetCube, Ref<SubMesh>)
	{
		return s_StaticCube;
	};
#pragma endregion

#pragma region Texture
	//Ref<Texture2D> GL::GetTexture2DRed()
	MH_DEFINE_FUNC(GL::GetTexture2DRed, Asset<Texture2D>)
	{
		return s_Texture2DRed;
	};

	//Ref<Texture2D> GL::GetTexture2DWhite()
	MH_DEFINE_FUNC(GL::GetTexture2DWhite, Asset<Texture2D>)
	{
		return s_Texture2DWhite;
	};

	//Ref<Texture2D> GL::GetTexture2DBlack()
	MH_DEFINE_FUNC(GL::GetTexture2DBlack, Asset<Texture2D>)
	{
		return s_Texture2DBlack;
	};

	//Ref<Texture2D> GL::GetTexture2DBump()
	MH_DEFINE_FUNC(GL::GetTexture2DBump, Asset<Texture2D>)
	{
		return s_Texture2DBump;
	};

	//Ref<TextureCube> GL::GetTextureCubeWhite()
	MH_DEFINE_FUNC(GL::GetTextureCubeWhite, Asset<TextureCube>)
	{
		return s_TextureCubeWhite;
	};

	//Ref<TextureCube> GL::GetTextureCubeGrey()
	MH_DEFINE_FUNC(GL::GetTextureCubeGrey, Asset<TextureCube>)
	{
		return s_TextureCubeGrey;
	};
#pragma endregion

	//const char* GL::GetGraphicsVendor()
	MH_DEFINE_FUNC(GL::GetGraphicsVendor, const char*)
	{
		return s_RendererAPI->GetGraphicsVendor();
	};

	//void GL::SetViewportImpl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor)
	MH_DEFINE_FUNC(GL::SetViewportImpl, void, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor)
	{
		s_RendererAPI->SetViewport(x, y, w, h, scissor);
	};

	//void GL::FinishRendering()
	MH_DEFINE_FUNC(GL::FinishRendering, void)
	{
		s_RendererAPI->FinishRendering();
	};

	//void GL::SetClearColor(const glm::vec4& color)
	MH_DEFINE_FUNC(GL::SetClearColor, void, const glm::vec4& color)
	{
		s_RendererAPI->SetClearColor(color);
	};

	//void GL::ClearImpl(bool color, bool depth)
	MH_DEFINE_FUNC(GL::ClearImpl, void, bool color, bool depth)
	{
		s_RendererAPI->Clear(color, depth);
	};

	//void GL::EnableCullingImpl(bool enable, bool cullFront)
	MH_DEFINE_FUNC(GL::EnableCullingImpl, void, bool enable, bool cullFront)
	{
		s_RendererAPI->EnableCulling(enable, cullFront);
	};

	//void GL::EnableZWriting(bool enable)
	MH_DEFINE_FUNC(GL::EnableZWriting, void, bool enable)
	{
		s_RendererAPI->EnableZWriting(enable);
	};

	//void GL::SetZTesting(RendererAPI::DepthMode mode)
	MH_DEFINE_FUNC(GL::SetZTesting, void, RendererAPI::DepthMode mode)
	{
		s_RendererAPI->SetZTesting(mode);
	};

	//void GL::SetFillMode(bool enable)
	MH_DEFINE_FUNC(GL::SetFillMode, void, bool enable)
	{
		s_RendererAPI->SetFillMode(enable);
	};

	//void GL::SetBlendMode(RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable)
	MH_DEFINE_FUNC(GL::SetBlendMode, void, RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable)
	{
		s_RendererAPI->SetBlendMode(src, dst, enable);
	};

#pragma region Draw
	//void GL::DrawScreenQuad()
	MH_DEFINE_FUNC(GL::DrawScreenQuad, void)
	{
		s_StaticScreenQuad->Bind();
		s_RendererAPI->DrawIndexed(s_StaticScreenQuad->GetIndexCount());
	};

	//void GL::DrawIndexed(uint32_t indexCount)
	MH_DEFINE_FUNC(GL::DrawIndexed, void, uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(indexCount);
	};

	//void GL::DrawInstanced(uint32_t indexCount, uint32_t count)
	MH_DEFINE_FUNC(GL::DrawInstanced, void, uint32_t indexCount, uint32_t count)
	{
		s_RendererAPI->DrawInstanced(indexCount, count);
	};
#pragma endregion

	Ref<SubMesh> GL::CreateScreenQuad()
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
		SubMesh::InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;

		return SubMesh::Create(4, 6, interleavedVertices, indices);
	}

	Ref<SubMesh> GL::CreatePyramid()
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
		SubMesh::InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;

		return SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);
	}
}