#pragma once

#include "Mahakam/Core/Core.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct aiNodeAnim;

namespace Mahakam
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};

	class Bone
	{
	private:
		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;
		int m_NumPositions;
		int m_NumRotations;
		int m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;

	public:
		Bone() = default;

		Bone(const std::string& name, int ID, const aiNodeAnim* channel);

		void Update(float animationTime);

		inline const glm::mat4& GetLocalTransform() { return m_LocalTransform; }

		inline std::string GetBoneName() const { return m_Name; }

		inline int GetBoneID() { return m_ID; }

		int GetPositionIndex(float animationTime);

		int GetRotationIndex(float animationTime);

		int GetScaleIndex(float animationTime);

	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);

		glm::mat4 InterpolateRotation(float animationTime);

		glm::mat4 InterpolateScaling(float animationTime);
	};
}