#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	class Transform
	{
	private:
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix;

		void updateMatrix()
		{
			modelMatrix = glm::translate(glm::mat4(1.0f), position)
				* glm::mat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
		}

	public:
		Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
			: position(pos), rotation(rot), scale(scale)
		{
			updateMatrix();
		}

		inline void setPosition(const glm::vec3 pos) { position = pos; updateMatrix(); }
		inline void setPosition(const glm::quat rot) { rotation = rot; updateMatrix(); }
		inline void setPosition(const glm::vec3 sc) { scale = sc; updateMatrix(); }

		inline const glm::vec3& getPosition() const { return position; }
		inline const glm::quat& getRotation() const { return rotation; }
		inline const glm::vec3& getScale() const { return scale; }
	};
}