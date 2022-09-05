#include "Mahakam/mhpch.h"
#include "Animation.h"

#include "Mahakam/Core/SharedLibrary.h"

#include <tiny_gltf/tiny_gltf.h>

#include <filesystem>

namespace Mahakam
{
	Animation::Animation(const std::filesystem::path& filepath)
	{
		MH_PROFILE_FUNCTION();

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool success;
		if (filepath.extension().string() == ".gltf")
			success = loader.LoadASCIIFromFile(&model, &err, &warn, filepath.string());
		else
			success = loader.LoadBinaryFromFile(&model, &err, &warn, filepath.string());

		if (!warn.empty())
			MH_CORE_WARN("[GLTF] Warning: {0}", warn);

		if (!err.empty())
			MH_CORE_ERROR("[GLTF] Error: {0}", err);

		if (!success) {
			MH_CORE_ERROR("[GLTF] Failed to parse glTF model at {0}", filepath.string());
			return;
		}

		auto& animations = model.animations;
		for (auto& animation : animations)
		{
			for (auto& channel : animation.channels)
			{
				auto& sampler = animation.samplers[channel.sampler];

				// Input data
				const auto& inputAccessor = model.accessors[sampler.input];
				const auto& inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const auto& inputBuffer = model.buffers[inputBufferView.buffer];

				const float* times = reinterpret_cast<const float*>(&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset]);

				// Output data
				const auto& outputAccessor = model.accessors[sampler.input];
				const auto& outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const auto& outputBuffer = model.buffers[outputBufferView.buffer];

				if (channel.target_path == "translation")
				{
					const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&inputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);
				}
				else if (channel.target_path == "rotation")
				{
					const glm::quat* values = reinterpret_cast<const glm::quat*>(&inputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);
				}
				else if (channel.target_path == "scale")
				{
					const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&inputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);
				}
				else if (channel.target_path == "weights")
				{
					MH_CORE_BREAK("Weight animations are not yet supported");
				}
			}
		}
	}

	//Asset<Animation> Animation::LoadImpl(const std::filesystem::path& filepath)
	MH_DEFINE_FUNC(Animation::LoadImpl, Asset<Animation>, const std::filesystem::path& filepath)
	{
		return Asset<Animation>(CreateRef<Animation>(filepath));
	};
}