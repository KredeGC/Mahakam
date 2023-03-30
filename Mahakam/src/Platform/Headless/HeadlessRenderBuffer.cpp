#include "Mahakam/mhpch.h"
#include "HeadlessRenderBuffer.h"

namespace Mahakam
{
	HeadlessRenderBuffer::HeadlessRenderBuffer(uint32_t width, uint32_t height, TextureFormat format)
		: m_Width(width), m_Height(height), m_Size(width * height), m_Format(format)
	{
		
	}

	void HeadlessRenderBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
	}
}