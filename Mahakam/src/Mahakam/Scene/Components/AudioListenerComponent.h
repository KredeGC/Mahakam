#pragma once

namespace Mahakam
{
	struct AudioListenerComponent
	{
	private:
		bool m_Dummy = false;

	public:
		AudioListenerComponent() { }

		AudioListenerComponent(const AudioListenerComponent&) = default;
	};
}