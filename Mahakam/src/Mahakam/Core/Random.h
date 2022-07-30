#pragma once

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
	};
}