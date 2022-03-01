#include "ebpch.h"
#include "TexelGeometryPass.h"

namespace Mahakam
{
	void TexelGeometryPass::Init(uint32_t width, uint32_t height)
	{
		// Create gbuffer
		FrameBufferProps gProps;
		gProps.width = width;
		gProps.height = height;
		gProps.colorAttachments = {
			TextureFormat::RGBA8, // RGB - Albedo, A - Occlussion
			TextureFormat::RGBA8, // RG - Unused, B - Metallic, A - Roughness
			TextureFormat::RG11B10F, // RGB - Emission (not affected by light)
			TextureFormat::RGB10A2, // RGB - World normal, A - Unused
			TextureFormat::RGB10A2 }; // RGB - World position offset, A - Unused
		gProps.depthAttachment = TextureFormat::Depth24; // Mutable

		gBuffer = FrameBuffer::Create(gProps);
	}
}