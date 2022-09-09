#include "Mahakam/mhpch.h"
#include "RenderData.h"

#include "Camera.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
	CameraData::CameraData(const Camera& camera, const glm::vec2& screenSize, const glm::mat4& transform) :
		u_m4_V(glm::inverse(transform)),
		u_m4_P(camera.GetProjectionMatrix()),
		u_m4_IV(transform),
		u_m4_IP(glm::inverse(camera.GetProjectionMatrix())),
		u_m4_VP(u_m4_P * u_m4_V),
		u_m4_IVP(u_m4_IV * u_m4_IP),
		u_CameraPos(transform[3]),
		u_ScreenParams(screenSize.x, screenSize.y, 1.0f / screenSize.x, 1.0f / screenSize.y) {}

	CameraData::CameraData(const glm::mat4& projectionMatrix, const glm::vec2& screenSize, const glm::mat4& transform) :
		u_m4_V(glm::inverse(transform)),
		u_m4_P(projectionMatrix),
		u_m4_IV(transform),
		u_m4_IP(glm::inverse(projectionMatrix)),
		u_m4_VP(u_m4_P* u_m4_V),
		u_m4_IVP(u_m4_IV* u_m4_IP),
		u_CameraPos(transform[3]),
		u_ScreenParams(screenSize.x, screenSize.y, 1.0f / screenSize.x, 1.0f / screenSize.y) {}
}