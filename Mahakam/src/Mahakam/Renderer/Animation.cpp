#include "Mahakam/mhpch.h"
#include "Animation.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"

#include <tiny_gltf/tiny_gltf.h>

namespace Mahakam
{
	Animation::Animation(const std::filesystem::path& filepath, int index)
		: m_Filepath(filepath), m_AnimationIndex(index)
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
		auto& animation = animations[m_AnimationIndex];
		m_Name = animation.name;

		// Input data (timestamps)
		int timeInput = animation.samplers[0].input;
		const auto& inputAccessor = model.accessors[timeInput];
		const auto& inputBufferView = model.bufferViews[inputAccessor.bufferView];
		const auto& inputBuffer = model.buffers[inputBufferView.buffer];

		// Assume that all sampler inputs are the same
		const float* times = reinterpret_cast<const float*>(&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset]);

		m_Timestamps.assign(times, times + inputAccessor.count);

		// Each channel refers to a node/bone's target
		// channels.size() ~= bones.size() * [translation, rotation, scale, weights].size()
		for (auto& channel : animation.channels)
		{
			auto& sampler = animation.samplers[channel.sampler];

			// Make sure that the inputs match
			MH_CORE_ASSERT(sampler.input == timeInput, "Multiple time inputs are currently not supported. Consider simplifying your model's animations");

			// Output data (the actual values)
			const auto& outputAccessor = model.accessors[sampler.output];
			const auto& outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const auto& outputBuffer = model.buffers[outputBufferView.buffer];

			MH_CORE_ASSERT(outputAccessor.count == inputAccessor.count, "The number of keyframes and sampler values must match");

			if (channel.target_path == "translation")
			{
				// Extract translations from the buffer
				const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				m_Translations[channel.target_node] = std::vector<glm::vec3>(values, values + outputAccessor.count);

				/*MH_CORE_TRACE("{0}", model.nodes[channel.target_node].name);
				MH_CORE_TRACE("{0}, {1}, {2}", (*values).x, (*values).y, (*values).z);*/
			}
			else if (channel.target_path == "rotation")
			{
				// Extract rotations from the buffer
				const glm::quat* values = reinterpret_cast<const glm::quat*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				m_Rotations[channel.target_node] = std::vector<glm::quat>(values, values + outputAccessor.count);
			}
			else if (channel.target_path == "scale")
			{
				// Extract scales from the buffer
				const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				m_Scales[channel.target_node] = std::vector<glm::vec3>(values, values + outputAccessor.count);
			}
			else if (channel.target_path == "weights")
			{
				MH_CORE_BREAK("Weight animations are not yet supported");
			}
		}
	}

	//Asset<Animation> Animation::LoadImpl(const std::filesystem::path& filepath)
	MH_DEFINE_FUNC(Animation::LoadImpl, Asset<Animation>, const std::filesystem::path& filepath, int index)
	{
		return Asset<Animation>(CreateRef<Animation>(filepath, index));
	};
}