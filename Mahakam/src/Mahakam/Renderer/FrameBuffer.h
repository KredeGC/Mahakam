#pragma once

#include "Mahakam/Core/Core.h"

#include <vector>

namespace Mahakam
{
	struct FrameBufferProps
	{
		uint32_t width;
		uint32_t height;

		bool swapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual const std::vector<uint32_t> getColorAttachments() const = 0;
		virtual uint32_t getDepthAttachment() const = 0;

		virtual const FrameBufferProps& getSpecification() const = 0;

		static Ref<FrameBuffer> create(const FrameBufferProps& prop);
	};
}