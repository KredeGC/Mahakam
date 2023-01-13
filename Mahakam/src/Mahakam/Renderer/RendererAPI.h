#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Types.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float4.hpp>

namespace Mahakam
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		enum class BlendMode
        {
			Zero = 0,
			One,
			SrcColor,
			SrcAlpha,
			OneMinusSrcColor,
			OneMinusSrcAlpha,
			DstColor,
			DstAlpha
		};

		enum class DepthMode
		{
			Never,
			Less,
			LEqual,
			Equal,
			NotEqual,
			GEqual,
			Greater,
			Always
		};

	private:
		static API s_API;

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual const char* GetGraphicsVendor() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor) = 0;

		virtual void FinishRendering() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear(bool color, bool depth) = 0;

		virtual void EnableCulling(bool enable, bool cullFront) = 0;
		virtual void EnableZWriting(bool enable) = 0;

		virtual void SetZTesting(DepthMode mode) = 0;
		virtual void SetFillMode(bool fill) = 0;
		virtual void SetBlendMode(BlendMode src, BlendMode dst, bool enable) = 0;

		virtual void DrawIndexed(uint32_t count) = 0;
		virtual void DrawInstanced(uint32_t indexCount, uint32_t count) = 0;

		inline static void SetAPI(API api) { s_API = api; }
		inline static API GetAPI() { return s_API; }

		MH_DECLARE_FUNC(Create, Scope<RendererAPI>);
	};
}