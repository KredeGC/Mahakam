#pragma once

#include "Mahakam/Renderer/Camera.h"

#include "Mahakam/Renderer/RenderPasses.h"

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

		CameraComponent(Camera::ProjectionType projection, float fov, float nearPlane = 0.03f, float farPlane = 1000.0f,
			const std::initializer_list<RenderPass*>& renderpasses = { new GeometryRenderPass(), new LightingRenderPass(), new ParticleRenderPass(), new TonemappingRenderPass() })
			: camera(projection, fov, nearPlane, farPlane, renderpasses) {}

		operator Camera& () { return camera; }
		operator const Camera& () const { return camera; }

		void SetFixedAspectRatio(bool fixed) { fixedAspectRatio = fixed; }

		bool HasFixedAspectRatio() const { return fixedAspectRatio; }

		Camera& GetCamera() { return camera; }
	};
}