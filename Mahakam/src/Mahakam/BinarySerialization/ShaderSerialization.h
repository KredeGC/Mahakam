#pragma once

#include "Mahakam/Renderer/Shader.h"

#include "ContainerSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::Shader>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Mahakam::Asset<Mahakam::Shader>> shader) noexcept
		{
			using namespace Mahakam;

			for (auto& property : shader->GetProperties())
			{

			}

			return true;
		}
	};
}