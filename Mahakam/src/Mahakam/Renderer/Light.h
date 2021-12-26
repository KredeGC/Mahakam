#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mahakam
{
	class Light
	{
	private:
		glm::vec3 position;
		//glm::quat rotation;
		glm::vec3 color;
		//LightType type;

	public:
		Light(const glm::vec3& pos, const glm::vec3& color);

		void setPosition(const glm::vec3& pos);
		//void setRotation(const glm::quat& rot);
		void setColor(const glm::vec3& col);

		const glm::vec3& getPosition() const { return position; }
		//const glm::quat& getRotation() const { return rotation; }
		const glm::vec3& getColor() const { return color; }
	};
}