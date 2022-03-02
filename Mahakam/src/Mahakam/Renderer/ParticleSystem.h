#pragma once

#include "Mahakam/Core/Core.h"

#include "Texture.h"

namespace Mahakam
{
	struct ParticleProps
	{

	};

	class ParticleSystem
	{
	private:
		ParticleProps props;

	public:
		ParticleSystem() = default;

		inline void BindBuffers(glm::mat4 transform)
		{

		}

		inline void Simulate(float dt)
		{

		}

		inline const ParticleProps& GetProps() const { return props; }
	};
}