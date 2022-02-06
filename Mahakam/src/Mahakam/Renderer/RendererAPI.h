#pragma once
#include <glm/glm.hpp>

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

		enum class BlendMode {
			Zero = 0,
			One,
			SrcColor,
			SrcAlpha,
			OneMinusSrcColor,
			OneMinusSrcAlpha,
			DstColor,
			DstAlpha
		};
	private:
		static API api;

	public:
		virtual void Init() = 0;
		virtual const char* GetGraphicsVendor() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void FinishRendering() = 0;

		virtual void SetClearColor(const glm::vec4 color) = 0;
		virtual void Clear(bool color, bool depth) = 0;

		virtual void EnableCulling(bool enable, bool cullFront) = 0;

		virtual void EnableZWriting(bool enable) = 0;

		virtual void EnableZTesting(bool enable) = 0;

		virtual void SetFillMode(bool fill) = 0;

		virtual void SetBlendMode(BlendMode src, BlendMode dst, bool enable) = 0;

		virtual void DrawIndexed(uint32_t count) = 0;

		virtual void DrawInstanced(uint32_t indexCount, uint32_t count) = 0;

		inline static API GetAPI() { return api; }
	};
}