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

	public:
		static void Init();

		static void Shutdown();

		inline static Ref<Mesh> GetScreenQuad() { return staticScreenQuad; }
		inline static Ref<Mesh> GetInvertedPyramid() { return staticPyramid; }
		inline static Ref<Mesh> GetInvertedSphere() { return staticSphereMesh; }
		inline static Ref<Mesh> GetInvertedCube() { return staticCubemapMesh; }

		inline static const char* GetGraphicsVendor() { return rendererAPI->GetGraphicsVendor(); }

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { rendererAPI->SetViewport(x, y, w, h); }

		inline static void FinishRendering() { rendererAPI->FinishRendering(); }

		inline static void SetClearColor(const glm::vec4& color) { rendererAPI->SetClearColor(color); }
		inline static void Clear(bool color = true, bool depth = true) { rendererAPI->Clear(color, depth); }

		inline static void EnableCulling(bool enable, bool cullFront = false) { rendererAPI->EnableCulling(enable, cullFront); }

		inline static void EnableZWriting(bool enable) { rendererAPI->EnableZWriting(enable); }

		inline static void EnableZTesting(bool enable) { rendererAPI->EnableZTesting(enable); }

		inline static void SetFillMode(bool fill) { rendererAPI->SetFillMode(fill); }

		inline static void SetBlendMode(RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable) { rendererAPI->SetBlendMode(src, dst, enable); }

		inline static void DrawScreenQuad() { staticScreenQuad->Bind(); DrawIndexed(staticScreenQuad->GetIndexCount()); }

		inline static void DrawIndexed(uint32_t indexCount) { rendererAPI->DrawIndexed(indexCount); }

		inline static void DrawInstanced(uint32_t indexCount, uint32_t count) { rendererAPI->DrawInstanced(indexCount, count); }

	private:
		static Ref<Mesh> CreateScreenQuad();
		static Ref<Mesh> CreatePyramid();
	};
}