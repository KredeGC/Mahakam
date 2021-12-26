#include "mhpch.h"
#include "Light.h"

namespace Mahakam
{
	Light::Light(const glm::vec3& pos, const glm::vec3& color)
		: position(pos), color(color) {}

	void Light::setPosition(const glm::vec3& pos)
	{
		position = pos;
	}

	void Light::setColor(const glm::vec3& col)
	{
		color = col;
	}
}