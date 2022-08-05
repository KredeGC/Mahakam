#pragma once

namespace Mahakam
{
	class Timestep
	{
	private:
		float time;

	public:
		Timestep(float time = 0.0f) : time(time) {}

		inline operator float() const { return time; }

		inline float GetSeconds() const { return time; }
		inline float GetMilliSeconds() const { return time * 1000.0f; }
	};
}