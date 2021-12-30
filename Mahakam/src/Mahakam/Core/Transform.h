#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	class Transform
	{
	private:
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 modelMatrix;

		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		void updateMatrix()
		{
			modelMatrix = glm::translate(glm::mat4(1.0f), position)
				* glm::mat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);

			forward = modelMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
			right = modelMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			up = modelMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		}

	public:
		Transform()
			: position({ 0.0f, 0.0f, 0.0f }), rotation({ 1.0f, 0.0f, 0.0f, 0.0f }), scale({ 1.0f, 1.0f, 1.0f })
		{
			updateMatrix();
		}

		Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
			: position(pos), rotation(rot), scale(scale)
		{
			updateMatrix();
		}

		inline void setPosition(const glm::vec3 pos) { position = pos; updateMatrix(); }
		inline void setRotation(const glm::quat rot) { rotation = rot; updateMatrix(); }
		inline void setScale(const glm::vec3 sc) { scale = sc; updateMatrix(); }

		inline const glm::vec3& getPosition() const { return position; }
		inline const glm::quat& getRotation() const { return rotation; }
		inline const glm::vec3& getScale() const { return scale; }

		inline const glm::vec3& getForward() const { return forward; }
		inline const glm::vec3& getRight() const { return right; }
		inline const glm::vec3& getUp() const { return up; }

		inline const glm::mat4& getModelMatrix() const { return modelMatrix; }
	};
}