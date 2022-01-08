#pragma once

#include "Mahakam/Renderer/Camera.h"

#include "Mahakam/Renderer/Buffer.h"

namespace Mahakam
{
	struct TransformComponent;

	struct CameraComponent
	{
	private:
		Camera camera;

		bool fixedAspectRatio = false;

	public:
		CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;

		CameraComponent(Camera::ProjectionType projection, float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
			: camera(projection, fov, ratio, nearPlane, farPlane) {}

		operator Camera& () { return camera; }
		operator const Camera& () const { return camera; }

		void setFixedAspectRatio(bool fixed) { fixedAspectRatio = fixed; }

		bool hasFixedAspectRatio() const { return fixedAspectRatio; }

		Camera& getCamera() { return camera; }
	};
}