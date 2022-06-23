#pragma once

#include <glm/glm.hpp>

namespace Mahakam
{
	class Rigidbody
	{
	public:
		virtual ~Rigidbody() = default;

		virtual void SetMass(float mass) = 0;
		virtual float GetMass() const = 0;

		virtual void SetVelocity(const glm::vec3& velocity) = 0;
		virtual const glm::vec3& GetVelocity() const = 0;
	};
}