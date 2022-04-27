#pragma once

#include "RendererAPI.h"

#include "Mesh.h"

namespace Mahakam
{
	class GL
	{
	private:
		static RendererAPI* rendererAPI;
		
		static Ref<Mesh> staticScreenQuad;
		static Ref<Mesh> staticPyramid;
		static Ref<Mesh> staticSphereMesh;
		static Ref<Mesh> staticCubemapMesh;
		static Ref<Mesh> staticCube;
		
		static Ref<Texture2D> texture2DRed;
		static Ref<Texture2D> texture2DWhite;
		static Ref<Texture2D> texture2DBlack;
		static Ref<Texture2D> texture2DBump;
		static Ref<TextureCube> textureCubeWhite;

	public:
		static void Init();
		static void Shutdown();

		MH_DECLARE_FUNC(GetScreenQuad, Ref<Mesh>);
		MH_DECLARE_FUNC(GetInvertedPyramid, Ref<Mesh>);
		MH_DECLARE_FUNC(GetInvertedSphere, Ref<Mesh>);
		MH_DECLARE_FUNC(GetInvertedCube, Ref<Mesh>);
		MH_DECLARE_FUNC(GetCube, Ref<Mesh>);

		MH_DECLARE_FUNC(GetTexture2DRed, Ref<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DWhite, Ref<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DBlack, Ref<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DBump, Ref<Texture2D>);
		MH_DECLARE_FUNC(GetTextureCubeWhite, Ref<TextureCube>);

		MH_DECLARE_FUNC(GetGraphicsVendor, const char*);

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor = false) { SetViewportImpl(x, y, w, h, scissor); }

		MH_DECLARE_FUNC(FinishRendering, void);

		MH_DECLARE_FUNC(SetClearColor, void, const glm::vec4& color);
		inline static void Clear(bool color = true, bool depth = true) { ClearImpl(color, depth); }

		inline static void EnableCulling(bool enable, bool cullFront = false) { EnableCullingImpl(enable, cullFront); }

		MH_DECLARE_FUNC(EnableZWriting, void, bool enable);
		MH_DECLARE_FUNC(EnableZTesting, void, bool enable);

		MH_DECLARE_FUNC(SetFillMode, void, bool fill);
		MH_DECLARE_FUNC(SetBlendMode, void, RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable);

		MH_DECLARE_FUNC(DrawScreenQuad, void);
		MH_DECLARE_FUNC(DrawIndexed, void, uint32_t indexCount);
		MH_DECLARE_FUNC(DrawInstanced, void, uint32_t indexCount, uint32_t count);

	private:
		static Ref<Mesh> CreateScreenQuad();
		static Ref<Mesh> CreatePyramid();

		MH_DECLARE_FUNC(SetViewportImpl, void, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor);

		MH_DECLARE_FUNC(ClearImpl, void, bool color, bool depth);

		MH_DECLARE_FUNC(EnableCullingImpl, void, bool enable, bool cullFront);
	};
}