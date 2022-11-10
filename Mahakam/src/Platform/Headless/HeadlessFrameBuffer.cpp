#include "Mahakam/mhpch.h"
#include "HeadlessFrameBuffer.h"

#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	HeadlessFrameBuffer::HeadlessFrameBuffer(const FrameBufferProps& props)
		: m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		if (!props.SwapChainTarget)
			Invalidate();
	}

	HeadlessFrameBuffer::~HeadlessFrameBuffer() {}

	void HeadlessFrameBuffer::Bind() {}

	void HeadlessFrameBuffer::Unbind() {}

	void HeadlessFrameBuffer::Blit(Asset<FrameBuffer> dest, bool color, bool depth) {}

	void HeadlessFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			MH_CORE_WARN("Attempted to resize framebuffer to an unsupported size: ({0},{1})", width, height);
			return;
		}

		m_Props.Width = width;
		m_Props.Height = height;

		if (!m_Props.SwapChainTarget)
			Invalidate();
	}

	Asset<Texture> HeadlessFrameBuffer::GetColorTexture(int index) const
	{
		return m_ColorAttachments[index];
	}

	Asset<Texture> HeadlessFrameBuffer::GetDepthTexture() const
	{
		return m_DepthAttachment;
	}

	void HeadlessFrameBuffer::ReadColorPixels(void* pixels, int attachmentSlot) const
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(attachmentSlot < m_ColorAttachments.size(), "Index outside range of framebuffer textures!");
	}

	void HeadlessFrameBuffer::Invalidate()
	{
		MH_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			for (int i = 0; i < m_ColorAttachments.size(); i++)
				m_ColorAttachments[i]->Resize(m_Props.Width, m_Props.Height);

			if (!m_Props.DontUseDepth && m_DepthAttachment)
				m_DepthAttachment->Resize(m_Props.Width, m_Props.Height);

			return;
		}

		// Color buffers
		for (int i = 0; i < m_Props.ColorAttachments.size(); i++)
		{
			FrameBufferAttachmentProps& spec = m_Props.ColorAttachments[i];

			Asset<Texture> tex = Asset<Texture2D>(Texture2D::Create({ m_Props.Width, m_Props.Height, spec.Format, spec.FilterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false }));

			m_ColorAttachments.push_back(tex);
		}

		// Depth buffer
		if (!m_Props.DontUseDepth)
		{
			FrameBufferAttachmentProps& spec = m_Props.DepthAttachment;

			if (spec.Immutable)
			{
				m_DepthAttachment = Asset<RenderBuffer>(RenderBuffer::Create(m_Props.Width, m_Props.Height, spec.Format));
			}
			else
			{
				m_DepthAttachment = Asset<Texture2D>(Texture2D::Create({ m_Props.Width, m_Props.Height, spec.Format, spec.FilterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false }));
			}
		}
	}
}