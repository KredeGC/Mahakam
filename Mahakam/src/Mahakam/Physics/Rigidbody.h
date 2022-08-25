#pragma once

#include "PhysicsEngine.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class Collider;

	class Rigidbody
	{
	public:
		virtual ~Rigidbody() = default;

		virtual void SetMass(float mass) = 0;
		virtual float GetMass() const = 0;

		virtual void AddCollider(const glm::vec3& position, const glm::quat& rotation, Collider* collider) = 0;
		virtual void RemoveCollider(Collider* collider) = 0;

		virtual void SetPositionRotation(const glm::vec3& pos, const glm::quat& rot) = 0;

		virtual void SetVelocity(const glm::vec3& velocity) = 0;
		virtual glm::vec3 GetVelocity() const = 0;

		inline static Ref<Rigidbody> Create() { return CreateImpl(PhysicsEngine::GetContext()); }

	private:
		static Ref<Rigidbody> CreateImpl(PhysicsContext* context);
	};
}