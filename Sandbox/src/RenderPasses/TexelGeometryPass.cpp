#include "sbpch.h"
#include "TexelGeometryPass.h"

namespace Mahakam
{
	bool TexelGeometryPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		// Create gbuffer
		FrameBufferProps gProps;
		gProps.Width = width;
		gProps.Height = height;
		gProps.ColorAttachments = {
			TextureFormat::RGBA8, // RGB - Albedo, A - Occlussion
			TextureFormat::RGBA8, // RG - Unused, B - Metallic, A - Roughness
			TextureFormat::RG11B10F, // RGB - Emission (not affected by light)
			TextureFormat::RGB10A2, // RGB - World normal, A - Unused
			TextureFormat::RGB10A2 }; // RGB - World position offset, A - Unused
		gProps.DepthAttachment = TextureFormat::Depth24; // Mutable

		gBuffer = FrameBuffer::Create(gProps);

		return true;
	}
}