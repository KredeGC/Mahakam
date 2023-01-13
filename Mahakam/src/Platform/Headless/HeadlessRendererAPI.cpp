#include "Mahakam/mhpch.h"
#include "HeadlessRendererAPI.h"

namespace Mahakam
{
	void HeadlessRendererAPI::Init() {}

	const char* HeadlessRendererAPI::GetGraphicsVendor()
	{
		return "HEADLESS";
	}

	void HeadlessRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor) {}

	void HeadlessRendererAPI::FinishRendering() {}

	void HeadlessRendererAPI::SetClearColor(const glm::vec4& color) {}

	void HeadlessRendererAPI::Clear(bool color, bool depth) {}

	void HeadlessRendererAPI::EnableCulling(bool enable, bool cullFront) {}

	void HeadlessRendererAPI::EnableZWriting(bool enable) {}

	void HeadlessRendererAPI::SetZTesting(RendererAPI::DepthMode mode) {}

	void HeadlessRendererAPI::SetFillMode(bool fill) {}

	void HeadlessRendererAPI::SetBlendMode(BlendMode src, BlendMode dst, bool enable) {}

	void HeadlessRendererAPI::DrawIndexed(uint32_t count) {}

	void HeadlessRendererAPI::DrawInstanced(uint32_t indexCount, uint32_t count) {}
}