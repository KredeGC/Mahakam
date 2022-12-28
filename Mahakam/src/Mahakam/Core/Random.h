#pragma once

#include <cstdint>
#include <random>

namespace Mahakam
{
	class Random
	{
	private:
		static std::random_device rd;
		static std::default_random_engine generator;
		static std::uniform_int_distribution<uint64_t> distribution;

	public:
		static uint64_t GetRandomID64();

		template<typename T>
		static T GetRandomDiscrete(T min, T max)
		{
			std::uniform_int_distribution<T> distribution(min, max);

			return distribution(generator);
		}

		template<typename T>
		static T GetRandomContinuous(T min, T max)
		{
			std::uniform_real_distribution<T> distribution(min, max);

			return distribution(generator);
		}
	};
}