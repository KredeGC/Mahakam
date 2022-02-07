#include "mhpch.h"
#include "Light.h"

#include "FrameBuffer.h"

namespace Mahakam
{
	Light::Light(LightType lightType, const glm::vec3& color)
		: lightType(lightType), color(color)
	{
		FrameBufferProps shadowProps;
		shadowProps.width = 1024;
		shadowProps.height = 1024;
		shadowProps.depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, false };
		shadowFramebuffer = FrameBuffer::Create(shadowProps);
	}

	Light::Light(LightType lightType, float range, const glm::vec3& color)
		: lightType(lightType), range(range), color(color) {}

	Light::Light(LightType lightType, float fov, float range, const glm::vec3& color)
		: lightType(lightType), fov(fov), range(range), color(color) {}
}