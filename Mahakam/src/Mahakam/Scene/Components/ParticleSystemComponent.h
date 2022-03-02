#pragma once

#include "Mahakam/Renderer/ParticleSystem.h"

namespace Mahakam
{
	struct ParticleSystemComponent
	{
	private:
		ParticleSystem particleSystem;

	public:
		ParticleSystemComponent() = default;

		operator const ParticleSystem& () const { return particleSystem; }

		ParticleSystem& GetParticleSystem() { return particleSystem; }
	};
}