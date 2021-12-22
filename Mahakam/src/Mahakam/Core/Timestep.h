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

		float getSeconds() const { return time; }
		float getMilliSeconds() const { return time * 1000.0f; }
	};
}