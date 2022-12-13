#pragma once

#include "Mahakam/Core/Core.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Mahakam
{
	// TODO: Compare the the performance of this more cache-friendly implementation

	struct TransformComponent
	{
	private:
		static constexpr uint8_t FLAG_DIRTY = BIT(0);
		static constexpr uint8_t FLAG_NO_MATRIX = BIT(1);

		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::quat m_Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 m_ModelMatrix{ 1.0f };

		uint32_t m_ParentHash = 0;
		uint8_t m_Flags = 0;

	public:
		TransformComponent() = default;

		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
			: m_Position(pos), m_Rotation(rot), m_Scale(scale)
		{ }

		operator const glm::mat4&() const { return m_ModelMatrix; }

		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; m_Flags |= FLAG_DIRTY; }
		inline void SetRotation(const glm::quat& rot) { m_Rotation = rot; m_Flags |= FLAG_DIRTY; }
		inline void SetEulerangles(const glm::vec3& euler) { m_Rotation = glm::quat(euler); m_Flags |= FLAG_DIRTY; }
		inline void SetScale(const glm::vec3& sc) { m_Scale = sc; m_Flags |= FLAG_DIRTY; }

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::quat& GetRotation() const { return m_Rotation; }
		inline glm::vec3 GetEulerAngles() const { return glm::eulerAngles(m_Rotation); }
		inline const glm::vec3& GetScale() const { return m_Scale; }

		inline void SetNoMatrix(bool enable)
		{
			if (enable != HasNoMatrix())
				m_Flags ^= FLAG_NO_MATRIX;
		}
		inline bool HasNoMatrix() const { return (m_Flags & FLAG_NO_MATRIX) != 0; }

		inline glm::vec3 GetForward() const { return m_Rotation * glm::vec3(0.0f, 0.0f, 1.0f); }
		inline glm::vec3 GetRight() const { return m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
		inline glm::vec3 GetUp() const { return m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

		inline void UpdateModelMatrix(const glm::mat4& modelMatrix)
		{
			if ((m_Flags & FLAG_NO_MATRIX) != 0)
				return;

			uint32_t hash = 2166136261U;
			const uint8_t* modelPtr = (const uint8_t*)glm::value_ptr(modelMatrix);
			for (uint32_t i = 0; i < sizeof(glm::mat4); ++i)
				hash = (hash * 16777619U) ^ *(modelPtr + i);

			if (m_ParentHash != hash || (m_Flags & FLAG_DIRTY) != 0)
			{
				m_ParentHash = hash;
				m_Flags ^= FLAG_DIRTY;
				m_ModelMatrix = modelMatrix * glm::translate(glm::mat4(1.0f), m_Position)
					* glm::toMat4(m_Rotation)
					* glm::scale(glm::mat4(1.0f), m_Scale);
			}
		}

		inline const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
	};
}