#pragma once

#include "Mahakam/Renderer/Camera.h"

namespace Mahakam
{
	struct TransformComponent;

	struct CameraComponent
	{
	private:
		Camera camera;

		bool m_FixedAspectRatio = false;

	public:
		CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;

		CameraComponent(Camera::ProjectionType projection, float fov, float nearPlane = 0.03f, float farPlane = 1000.0f)
			: camera(projection, fov, nearPlane, farPlane) {}

		inline operator Camera& () { return camera; }
		inline operator const Camera& () const { return camera; }

		inline void SetFixedAspectRatio(bool fixed) { m_FixedAspectRatio = fixed; }

		inline bool HasFixedAspectRatio() const { return m_FixedAspectRatio; }

		inline Camera& GetCamera() { return camera; }
	};
}