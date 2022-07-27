#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Mahakam
{
	// TODO: Compare the the performance of this more cache-friendly implementation

	struct TransformComponent
	{
	private:
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
		glm::quat m_Rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };

		uint8_t m_Flags = 0;

	public:
		TransformComponent() {}

		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
			: m_Position(pos), m_Rotation(rot), m_Scale(scale)
		{ }

		operator glm::mat4() { return GetModelMatrix(); }

		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		inline void SetRotation(const glm::quat& rot) { m_Rotation = rot; }
		inline void SetEulerangles(const glm::vec3& euler) { m_Rotation = glm::quat(euler); }
		inline void SetScale(const glm::vec3& sc) { m_Scale = sc; }
		inline void SetStatic(bool isStatic)
		{
			if (isStatic != IsStatic())
				m_Flags ^= 0x01;
		}
		inline void SetDirty(bool dirty)
		{
			if (dirty != IsStatic())
				m_Flags ^= 0x02;
		}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::quat& GetRotation() const { return m_Rotation; }
		inline glm::vec3 GetEulerAngles() const { return glm::eulerAngles(m_Rotation); }
		inline const glm::vec3& GetScale() const { return m_Scale; }
		inline bool IsStatic() const { return m_Flags & 0x01; }
		inline bool IsDirty() const { return m_Flags & 0x02; }

		inline glm::vec3 GetForward() const { return glm::mat4(m_Rotation) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); }
		inline glm::vec3 GetRight() const { return glm::mat4(m_Rotation) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); }
		inline glm::vec3 GetUp() const { return glm::mat4(m_Rotation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); }

		inline glm::mat4 GetModelMatrix() const
		{
			return glm::translate(glm::mat4(1.0f), m_Position)
				* glm::toMat4(m_Rotation)
				* glm::scale(glm::mat4(1.0f), m_Scale);
		}
	};

	//struct TransformComponent
	//{
	//private:
	//	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	//	glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
	//	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	//	glm::vec3 eulerAngles = { 0.0f, 0.0f, 0.0f };

	//	glm::mat4 modelMatrix;

	//	glm::vec3 forward;
	//	glm::vec3 right;
	//	glm::vec3 up;

	//public:
	//	TransformComponent()
	//		: position({ 0.0f, 0.0f, 0.0f }), rotation({ 1.0f, 0.0f, 0.0f, 0.0f }), scale({ 1.0f, 1.0f, 1.0f })
	//	{
	//		UpdateMatrix();
	//	}

	//	TransformComponent(const TransformComponent&) = default;

	//	TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
	//		: position(pos), rotation(rot), scale(scale)
	//	{
	//		UpdateMatrix();
	//	}

	//	operator glm::mat4& () { return modelMatrix; }
	//	operator const glm::mat4& () const { return modelMatrix; }

	//	inline void SetPosition(const glm::vec3 pos) { position = pos; UpdateMatrix(); }
	//	inline void SetRotation(const glm::quat rot) { rotation = rot; eulerAngles = glm::eulerAngles(rotation); UpdateMatrix(); }
	//	inline void SetEulerangles(const glm::vec3 euler) { eulerAngles = euler; rotation = glm::quat(eulerAngles); UpdateMatrix(); }
	//	inline void SetScale(const glm::vec3 sc) { scale = sc; UpdateMatrix(); }

	//	inline const glm::vec3& GetPosition() const { return position; }
	//	inline const glm::quat& GetRotation() const { return rotation; }
	//	inline const glm::vec3& GetEulerAngles() const { return eulerAngles; }
	//	inline const glm::vec3& GetScale() const { return scale; }

	//	inline const glm::vec3& GetForward() const { return forward; }
	//	inline const glm::vec3& GetRight() const { return right; }
	//	inline const glm::vec3& GetUp() const { return up; }

	//	inline const glm::mat4& GetModelMatrix() const { return modelMatrix; }

	//private:
	//	void UpdateMatrix()
	//	{
	//		modelMatrix = glm::translate(glm::mat4(1.0f), position)
	//			* glm::mat4(rotation)
	//			* glm::scale(glm::mat4(1.0f), scale);

	//		//glm::eulerAngles(rotation); // Unused?

	//		forward = modelMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	//		right = modelMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	//		up = modelMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	//	}
	//};
}