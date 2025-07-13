#include "Mahakam/mhpch.h"
#include "Random.h"

namespace Mahakam
{
	std::random_device Random::rd;
	std::default_random_engine Random::generator(rd());
	std::uniform_int_distribution<uint64_t> Random::distribution(1, 0xFFFFFFFFFFFFFFFF);

	uint64_t Random::GetRandomID64()
	{
		return distribution(generator);
	}
}