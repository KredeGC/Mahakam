#include "Mahakam/mhpch.h"
#include "Random.h"

namespace Mahakam
{
	std::random_device Random::rd;
	std::default_random_engine Random::generator(rd());
	std::uniform_int_distribution<uint64_t> Random::distribution(0, 0xFFFFFFFFFFFFFFFF);

	uint64_t Random::GetRandomID64()
	{
		uint64_t id = 0;
		while (id == 0)
			id = distribution(generator);
		return id;
	}
}