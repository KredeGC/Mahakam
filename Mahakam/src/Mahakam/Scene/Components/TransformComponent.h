#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Mahakam
{
	struct TransformComponent
	{
	private:
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 eulerAngles = { 0.0f, 0.0f, 0.0f };

		glm::mat4 modelMatrix;

		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		void updateMatrix()
		{
			modelMatrix = glm::translate(glm::mat4(1.0f), position)
				* glm::mat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);

			glm::eulerAngles(rotation);

			forward = modelMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
			right = modelMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			up = modelMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		}

	public:
		TransformComponent()
			: position({ 0.0f, 0.0f, 0.0f }), rotation({ 1.0f, 0.0f, 0.0f, 0.0f }), scale({ 1.0f, 1.0f, 1.0f })
		{
			updateMatrix();
		}

		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
			: position(pos), rotation(rot), scale(scale)
		{
			updateMatrix();
		}

		operator glm::mat4& () { return modelMatrix; }
		operator const glm::mat4& () const { return modelMatrix; }

		inline void setPosition(const glm::vec3 pos) { position = pos; updateMatrix(); }
		inline void setRotation(const glm::quat rot) { rotation = rot; eulerAngles = glm::eulerAngles(rotation); updateMatrix(); }
		inline void setEulerangles(const glm::vec3 euler) { eulerAngles = euler; rotation = glm::quat(eulerAngles); updateMatrix(); }
		inline void setScale(const glm::vec3 sc) { scale = sc; updateMatrix(); }

		inline const glm::vec3& getPosition() const { return position; }
		inline const glm::quat& getRotation() const { return rotation; }
		inline const glm::vec3& getEulerAngles() const { return eulerAngles; }
		inline const glm::vec3& getScale() const { return scale; }

		inline const glm::vec3& getForward() const { return forward; }
		inline const glm::vec3& getRight() const { return right; }
		inline const glm::vec3& getUp() const { return up; }

		inline const glm::mat4& getModelMatrix() const { return modelMatrix; }
	};
}