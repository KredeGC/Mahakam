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

		CameraComponent(bool perspective, float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
			: camera(perspective, fov, ratio, nearPlane, farPlane) {}

		operator Camera& () { return camera; }
		operator const Camera& () const { return camera; }

		//void setFov(float f) { fov = f; recalculatePerspectiveProjectionMatrix(); }

		//void setNearPlane(float nearPlane) { nearZ = nearPlane; }
		//void setFarPlane(float farPlane) { nearZ = farPlane; }

		bool hasFixedAspectRatio() const { return fixedAspectRatio; }

		Camera& getCamera() { return camera; }
	};
}