#pragma once

//#include "Platform/OpenGL/OpenGLFrameBuffer.h"

#include <vector>

namespace Mahakam
{
	class FrameBuffer
	{
	private:
		uint32_t rendererID;
		std::vector<uint32_t> colorAttachments;
		uint32_t depthAttachment = 0;

	public:
		virtual ~FrameBuffer() {}

		virtual void bind() = 0;
		virtual void unbind() = 0;



		//static Ref<FrameBuffer> create(uint32_t size);
	};
}