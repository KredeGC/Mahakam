#pragma once

#include "RendererAPI.h"

#include "Mesh.h"

namespace Mahakam
{
	class GL
	{
	private:
		static GL* s_Instance;

		RendererAPI* rendererAPI;

		Ref<Mesh> staticScreenQuad;
		Ref<Mesh> staticPyramid;
		Ref<Mesh> staticSphereMesh;
		Ref<Mesh> staticCubemapMesh;
		Ref<Mesh> staticCube;

	public:
		GL();
		~GL();

		static GL* GetInstance();

		inline static void Init() { s_Instance = new GL; }
		inline static void Shutdown() { delete s_Instance; }

		inline static Ref<Mesh> GetScreenQuad() { return GetInstance()->staticScreenQuad; }
		inline static Ref<Mesh> GetInvertedPyramid() { return GetInstance()->staticPyramid; }
		inline static Ref<Mesh> GetInvertedSphere() { return GetInstance()->staticSphereMesh; }
		inline static Ref<Mesh> GetInvertedCube() { return GetInstance()->staticCubemapMesh; }
		inline static Ref<Mesh> GetCube() { return GetInstance()->staticCube; }

		inline static const char* GetGraphicsVendor() { return GetInstance()->rendererAPI->GetGraphicsVendor(); }

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor = false) { GetInstance()->rendererAPI->SetViewport(x, y, w, h, scissor); }

		inline static void FinishRendering() { GetInstance()->rendererAPI->FinishRendering(); }

		inline static void SetClearColor(const glm::vec4& color) { GetInstance()->rendererAPI->SetClearColor(color); }
		inline static void Clear(bool color = true, bool depth = true) { GetInstance()->rendererAPI->Clear(color, depth); }

		inline static void EnableCulling(bool enable, bool cullFront = false) { GetInstance()->rendererAPI->EnableCulling(enable, cullFront); }

		inline static void EnableZWriting(bool enable) { GetInstance()->rendererAPI->EnableZWriting(enable); }
		inline static void EnableZTesting(bool enable) { GetInstance()->rendererAPI->EnableZTesting(enable); }

		inline static void SetFillMode(bool fill) { GetInstance()->rendererAPI->SetFillMode(fill); }

		inline static void SetBlendMode(RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable) { GetInstance()->rendererAPI->SetBlendMode(src, dst, enable); }

		inline static void DrawScreenQuad() { GetInstance()->staticScreenQuad->Bind(); DrawIndexed(GetInstance()->staticScreenQuad->GetIndexCount()); }

		inline static void DrawIndexed(uint32_t indexCount) { GetInstance()->rendererAPI->DrawIndexed(indexCount); }

		inline static void DrawInstanced(uint32_t indexCount, uint32_t count) { GetInstance()->rendererAPI->DrawInstanced(indexCount, count); }

	private:
		static Ref<Mesh> CreateScreenQuad();
		static Ref<Mesh> CreatePyramid();
	};
}