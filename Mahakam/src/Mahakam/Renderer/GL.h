#pragma once

#include "Mahakam/Asset/Asset.h"

#include "RendererAPI.h"

namespace Mahakam
{
	class Mesh;
	class Texture2D;
	class TextureCube;

	class GL
	{
	private:
		static RendererAPI* rendererAPI;
		
		static Asset<Mesh> staticScreenQuad;
		static Asset<Mesh> staticSphereMesh;
		static Asset<Mesh> staticInvertedPyramid;
		static Asset<Mesh> staticInvertedSphereMesh;
		static Asset<Mesh> staticInvertedCubemapMesh;
		static Asset<Mesh> staticCube;

		static Asset<Texture2D> texture2DRed;
		static Asset<Texture2D> texture2DWhite;
		static Asset<Texture2D> texture2DBlack;
		static Asset<Texture2D> texture2DBump;
		static Asset<TextureCube> textureCubeWhite;

	public:
		static void Init();
		static void Shutdown();

		MH_DECLARE_FUNC(GetScreenQuad, Asset<Mesh>);
		MH_DECLARE_FUNC(GetSphere, Asset<Mesh>);
		MH_DECLARE_FUNC(GetInvertedPyramid, Asset<Mesh>);
		MH_DECLARE_FUNC(GetInvertedSphere, Asset<Mesh>);
		MH_DECLARE_FUNC(GetInvertedCube, Asset<Mesh>);
		MH_DECLARE_FUNC(GetCube, Asset<Mesh>);

		MH_DECLARE_FUNC(GetTexture2DRed, Asset<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DWhite, Asset<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DBlack, Asset<Texture2D>);
		MH_DECLARE_FUNC(GetTexture2DBump, Asset<Texture2D>);
		MH_DECLARE_FUNC(GetTextureCubeWhite, Asset<TextureCube>);

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
		static Asset<Mesh> CreateScreenQuad();
		static Asset<Mesh> CreatePyramid();

		MH_DECLARE_FUNC(SetViewportImpl, void, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor);

		MH_DECLARE_FUNC(ClearImpl, void, bool color, bool depth);

		MH_DECLARE_FUNC(EnableCullingImpl, void, bool enable, bool cullFront);
	};
}