#pragma once

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/MeshProps.h"

#include "AssetSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::MeshNode>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::MeshNode> node) noexcept
		{
			// TODO: Should node name even be serialized?
			BS_ASSERT(stream.template serialize<std::string>(node.Name, 256U));

			BS_ASSERT(stream.template serialize<int>(node.ID));
			BS_ASSERT(stream.template serialize<int>(node.ParentID));
			BS_ASSERT(stream.template serialize<glm::mat4>(node.Offset));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::MeshProps>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::MeshProps> props) noexcept
		{
			size_t materialCount = props.Materials.size();
			BS_ASSERT(stream.template serialize<size_t>(materialCount));

			if constexpr (Stream::reading)
				props.Materials.resize(materialCount);

			for (size_t i = 0; i < materialCount; ++i)
			{
				BS_ASSERT(stream.template serialize<Mahakam::Asset<Mahakam::Material>>(props.Materials[i]));
			}

			// TODO: Is this needed? Could it not just be deduced from whether or not NodeHierarchy and BoneMap is empty?
			BS_ASSERT(stream.template serialize<bool>(props.IncludeBones));
			BS_ASSERT(stream.template serialize<bool>(props.IncludeNodes));

			return true;
		}
	};
}