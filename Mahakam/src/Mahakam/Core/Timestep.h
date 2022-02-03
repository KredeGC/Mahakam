#pragma once

namespace Mahakam
{
	class Timestep
	{
	private:
		float time;

	public:
		Timestep(float time = 0.0f) : time(time) {}

		operator float() const { return time; }

		float GetSeconds() const { return time; }
		float GetMilliSeconds() const { return time * 1000.0f; }
	};
}